/*
 * Copyright 2012, 10/10/2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/external_unit/ExternalUnitManager.h>
#include <chaos/common/external_unit/http_adapter/HTTPClientAdapter.h>

#include <chaos/common/utility/TimingUtil.h>

#define INFO    INFO_LOG(HTTPClientAdapter)
#define DBG     DBG_LOG(HTTPClientAdapter)
#define ERR     ERR_LOG(HTTPClientAdapter)

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::external_unit::http_adapter;

static const char *web_socket_option="Content-Type: application/bson-json\r\n";

HTTPClientAdapter::HTTPClientAdapter():
run(false){}

HTTPClientAdapter::~HTTPClientAdapter() {
    
}

void HTTPClientAdapter::init(void *init_data) throw (chaos::CException) {
    mg_mgr_init(&mgr, NULL);
    run = true;
    thread_poller.reset(new boost::thread(boost::bind(&HTTPClientAdapter::poller, this)));
}

void HTTPClientAdapter::deinit() throw (chaos::CException) {
    run = false;
    thread_poller->join();
    mg_mgr_free(&mgr);
}

void HTTPClientAdapter::poller() {
    INFO << "Entering thread poller";
    poll_counter = 0;
    while (run) {
        mg_mgr_poll(&mgr, 1);
        if(poll_counter++ % 1000){performReconnection();}
    }
    INFO << "Leaving thread poller";
}

int HTTPClientAdapter::addNewConnectionForEndpoint(ExternalUnitClientEndpoint *endpoint,
                                                   const std::string& endpoint_url,
                                                   const std::string& serialization) {
    LMapReconnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
    
    ChaosUniquePtr<serialization::AbstractExternalSerialization> serializer = ExternalUnitManager::getInstance()->getNewSerializationInstanceForType(serialization);
    if(!serializer.get()) {
        return -1;
    }
    
    ChaosSharedPtr<ExternalUnitConnection> conn_ptr(new ExternalUnitConnection(this,
                                                                               endpoint,
                                                                               ChaosMoveOperator(serializer)));
    
    
    
    //!associate id to connection
    ConnectionInfoShrdPtr ci(new ConnectionInfo());
    map_connection().insert(MapReconnectionInfoPair(conn_ptr->connection_identifier, ci));
    ci->class_instance = this;
    ci->endpoint_url = endpoint_url;
    ci->ext_unit_conn = conn_ptr;
    ci->conn =  mg_connect_ws(&mgr,
                              HTTPClientAdapter::ev_handler,
                              ci->endpoint_url.c_str(),
                              "ChaosExternalUnit",
                              web_socket_option);
    ci->conn->user_data = ci.get();
    return 0;
}

int HTTPClientAdapter::removeConnectionsFromEndpoint(ExternalUnitClientEndpoint *target_endpoint) {
    LMapReconnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
    
    MapReconnectionInfoIterator conn_it =  map_connection().find(target_endpoint->getConnectionIdentifier());
    if(conn_it == map_connection().end()) return 0;
    //!remove from active connection map
    map_connection().erase(conn_it);
    //now real conenction is fre to go away
    if(conn_it->second->conn) {
        if(conn_it->second->ext_unit_conn->online == false) return -1;
        mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_CLOSE, "", 0);
    }
    return 0;
}

void HTTPClientAdapter::performReconnection() {
    uint64_t cur_ts = TimingUtil::getTimeStamp();
    //!reconnection
    LMapReconnectionInfoReadLock wlm = map_connection.getReadLockObject();
    for(MapReconnectionInfoIterator it = map_connection().begin(), end = map_connection().end();
        it != end;
        it++) {
        if(it->second->conn == NULL &&
           cur_ts >= it->second->next_reconnection_retry_ts) {
            //try to reconnect
            it->second->conn = mg_connect_ws(&mgr,
                                             HTTPClientAdapter::ev_handler,
                                             it->second->endpoint_url.c_str(),
                                             "ChaosExternalUnit",
                                             web_socket_option);
            if(it->second->conn) {
                it->second->conn->user_data = it->second.get();
            } else {
                //retry to reconnect
                it->second->next_reconnection_retry_ts = TimingUtil::getTimestampWithDelay(5000, true);
            }
        }
    }
}

void HTTPClientAdapter::checkAcceptResponse(struct websocket_message *wm,
                                            bool& is_accept_response,
                                            int& accept_result) {
    std::string json_string((const char *)wm->data,
                            wm->size);
    ChaosUniquePtr<CDataWrapper> check_accepted_res = CDataWrapper::instanceFromJson(json_string);
    is_accept_response = check_accepted_res->hasKey("accepted_connection");
    accept_result = is_accept_response?check_accepted_res->getInt32Value("accepted_connection"):-1;
}

void HTTPClientAdapter::ev_handler(struct mg_connection *conn,
                                   int event,
                                   void *event_data) {
    ConnectionInfo *ci = static_cast<ConnectionInfo*>(conn->user_data);
    if(ci == NULL) return;
    switch (event) {
        case MG_EV_CONNECT: {
            ChaosWriteLock wl(ci->smux);
            int status = *((int *) event_data);
            ci->ext_unit_conn->online = (status==0);
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            CHAOS_ASSERT(ci->ext_unit_conn.get() != NULL);
            int err = 0;
            struct websocket_message *wm = (struct websocket_message *) event_data;
            if(ci->ext_unit_conn->accepted_state <= 0) {
                bool is_accept_response = false;
                int accept_result = -1;
                //check accepted state
                checkAcceptResponse(wm, is_accept_response, accept_result);
                if(is_accept_response) {
                    ci->ext_unit_conn->accepted_state = accept_result;
                } else {
                    ci->class_instance->sendWSJSONError(conn, -2, "Accept response is not well formed!");
                }
            } else {
                //accepted connection ca received data
                ChaosUniquePtr<CDataBuffer> buffer(new CDataBuffer((const char *)wm->data,
                                                                   (uint32_t)wm->size,
                                                                   true));
                if((err = ci->class_instance->sendDataToEndpoint(*ci->ext_unit_conn,
                                                                 ChaosMoveOperator(buffer)))) {
                    //weh don't have found the sriealizer
                    ci->class_instance->sendWSJSONError(conn, err, "Error sending data to endpoint");
                }
            }
            break;
        }
        case MG_EV_CLOSE: {
            //manage the reconnection
            LMapReconnectionInfoReadLock wlm = ci->class_instance->map_connection.getReadLockObject();
            if(ci->class_instance->map_connection().count(ci->ext_unit_conn->connection_identifier) !=0) {
                //in this case concnretion info need to be put into  reconnection_queue
                //!beause conenciton need to be reopend
                //reset real connection
                ci->conn = NULL;
                ci->ext_unit_conn->online  = false;
                ci->ext_unit_conn->accepted_state = -1;
                //set retry timeout after five seconds
                ci->next_reconnection_retry_ts = TimingUtil::getTimestampWithDelay(5000, true);
            }
            break;
        }
    }
}

int HTTPClientAdapter::sendDataToConnection(const std::string& connection_identifier,
                                            const chaos::common::data::CDBufferUniquePtr data,
                                            const EUCMessageOpcode opcode) {
    LMapReconnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
    
    MapReconnectionInfoIterator conn_it =  map_connection().find(connection_identifier);
    if(conn_it == map_connection().end()) return -1;
    
    if(conn_it->second->conn) {
        if(conn_it->second->ext_unit_conn->online == false) return -3;
        switch (opcode) {
            case EUCMessageOpcodeWhole:
                mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
                break;
            case EUCPhaseStartFragment:
                mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
                break;
            case EUCPhaseContinueFragment:
                mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
                break;
            case EUCPhaseEndFragment:
                mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
                break;
        }
    } else {
        return -2;
    }
    return 0;
}

int HTTPClientAdapter::closeConnection(const std::string& connection_identifier) {
    LMapReconnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
    
    MapReconnectionInfoIterator conn_it =  map_connection().find(connection_identifier);
    if(conn_it == map_connection().end()) return 0;
    
    if(conn_it->second->conn) {
        if(conn_it->second->ext_unit_conn->online == false) return -1;
        mg_send_websocket_frame(conn_it->second->conn, WEBSOCKET_OP_CLOSE, "", 0);
    }
    else{return -2;}
    
    //!remove from active connection map
    map_connection().erase(conn_it);
    return 0;
}