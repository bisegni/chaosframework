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

#include <chaos/common/exception/CException.h>
#include <chaos/common/external_unit/ExternalUnitManager.h>
#include <chaos/common/external_unit/http_adapter/HTTPClientAdapter.h>
#include <chaos/common/ChaosCommon.h>
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
run(false),
message_broadcasted(0){
    if(GlobalConfiguration::getInstance()->hasOption(chaos::InitOption::OPT_REST_POLL_TIME_US)){
        rest_poll_time=GlobalConfiguration::getInstance()->getOption<uint32_t>(chaos::InitOption::OPT_REST_POLL_TIME_US);
    } else {
        rest_poll_time=1000;
    }
}

HTTPClientAdapter::~HTTPClientAdapter() {}

void HTTPClientAdapter::init(void *init_data)  {
    mg_mgr_init(&mgr, NULL);
    run = true;
    thread_poller.reset(new boost::thread(boost::bind(&HTTPClientAdapter::poller, this)));
}

void HTTPClientAdapter::deinit()  {
    run = false;
    DBG<<" HTTPClientAdapter DEINIT";
    thread_poller->join();
    mg_mgr_free(&mgr);
}

void HTTPClientAdapter::poller() {
    INFO << "Entering thread poller";
    poll_counter = 0;
    while (run) {
        mg_mgr_poll(&mgr, 0);
        if(rest_poll_time>0){
            usleep(rest_poll_time);
            if(poll_counter++ % (rest_poll_time)*10000000){performReconnection();}
        }
        
        //check connection close opcode
        {
            LOpcodeShrdPtrQueueWriteLock wlm = post_evt_op_queue.getWriteLockObject();
            while(post_evt_op_queue().empty() == false) {
                OpcodeShrdPtr op = post_evt_op_queue().front();
                post_evt_op_queue().pop();
                {
                    //close real connection
                    struct mg_connection *c = NULL;
                    for (c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c)) {
                        if(!c->user_data) continue;
                        ConnectionMetadata<HTTPClientAdapter> *conn_metadata = static_cast<ConnectionMetadata<HTTPClientAdapter> * >(c->user_data);
                        if(conn_metadata->conn_uuid.compare(op->identifier) == 0) {
                            DBG<<" HTTPClientAdapter Close Connection";
                            c->flags |= MG_F_CLOSE_IMMEDIATELY;
                            delete(conn_metadata);
                            c->user_data = NULL;
                            break;
                        }
                    }
                    //remove virtual one
                    LMapConnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
                    MapConnectionInfoIterator conn_it = map_connection().find(op->identifier);
                    if(conn_it != map_connection().end()){
                        map_connection().erase(conn_it);
                    }
                }
                op->wait_termination_semaphore.unlock();
            }
        }
    }
    DBG<<" HTTPClientAdapter POLL EXIT";
    INFO << "Leaving thread poller";
}

int HTTPClientAdapter::addNewConnectionForEndpoint(ExternalUnitClientEndpoint *endpoint,
                                                   const std::string& endpoint_url,
                                                   const std::string& serialization) {
    LMapConnectionInfoWriteLock wlm = map_connection.getWriteLockObject();
    
    ChaosUniquePtr<serialization::AbstractExternalSerialization> serializer = ExternalUnitManager::getInstance()->getNewSerializationInstanceForType(serialization);
    if(!serializer.get()) {
        return -1;
    }
    try{
        ConnectionInfoShrdPtr ci = ConnectionInfoShrdPtr(new ConnectionInfo(endpoint_url));
        ci->ext_unit_conn = ChaosSharedPtr<ExternalUnitConnection>(new ExternalUnitConnection(this,
                                                                                              endpoint,
                                                                                              MOVE(serializer)));
        
        
        
        //!associate id to connection
        map_connection().insert(MapConnectionInfoPair(ci->ext_unit_conn->connection_identifier, ci));
        
        mg_connection *conn =  mg_connect_ws(&mgr,
                                             HTTPClientAdapter::ev_handler,
                                             ci->endpoint_url.c_str(),
                                             "ChaosExternalUnit",
                                             web_socket_option);
        ci->ext_unit_conn->online = true;
        conn->user_data = new ConnectionMetadata<HTTPClientAdapter>(ci->ext_unit_conn->connection_identifier, this);
    } catch(chaos::CException& ex) {
        return -2;
    }
    return 0;
}

int HTTPClientAdapter::sendDataToConnection(const std::string& connection_identifier,
                                            chaos::common::data::CDBufferUniquePtr data,
                                            const EUCMessageOpcode opcode) {
    LMapConnectionInfoReadLock wlm = map_connection.getReadLockObject();
    MapConnectionInfoIterator conn_it = map_connection().find(connection_identifier);
    if(conn_it == map_connection().end()) return 0;
    OpcodeShrdPtr op(new Opcode());
    op->identifier = connection_identifier;
    op->op_type = OpcodeInfoTypeSend;
    op->data = MOVE(data);
    op->data_opcode = opcode;
    ChaosWriteLock conn_wl(conn_it->second->smutex);
    conn_it->second->opcode_queue.push(op);
    return 0;
}

int HTTPClientAdapter::removeConnectionsFromEndpoint(ExternalUnitClientEndpoint *target_endpoint) {
    //remove endpoint from coon abstraction
    LOpcodeShrdPtrQueueWriteLock wlm = post_evt_op_queue.getWriteLockObject();
    OpcodeShrdPtr op(new Opcode());
    op->identifier = target_endpoint->getConnectionIdentifier();
    op->op_type = OpcodeInfoTypeCloseConnection;
    post_evt_op_queue().push(op);
    wlm->unlock();
    op->wait_termination_semaphore.wait();
    return 0;
}

int HTTPClientAdapter::closeConnection(const std::string& connection_identifier) {
    //remove endpoint from coon abstraction
    LOpcodeShrdPtrQueueWriteLock wlm = post_evt_op_queue.getWriteLockObject();
    OpcodeShrdPtr op(new Opcode());
    op->identifier = connection_identifier;
    op->op_type = OpcodeInfoTypeCloseConnection;
    post_evt_op_queue().push(op);
    wlm->unlock();
    op->wait_termination_semaphore.wait();
    return 0;
}

void HTTPClientAdapter::performReconnection() {
    uint64_t cur_ts = TimingUtil::getTimeStamp();
    //!reconnection
    LMapConnectionInfoReadLock wlm = map_connection.getReadLockObject();
    for(MapConnectionInfoIterator it = map_connection().begin(), end = map_connection().end();
        it != end;
        it++) {
        if(it->second->ext_unit_conn.get() &&
           it->second->ext_unit_conn->online == false &&
           cur_ts >= it->second->next_reconnection_retry_ts) {
            //try to reconnect
            mg_connection * conn = mg_connect_ws(&mgr,
                                                 HTTPClientAdapter::ev_handler,
                                                 it->second->endpoint_url.c_str(),
                                                 "ChaosExternalUnit",
                                                 web_socket_option);
            if(conn) {
                it->second->ext_unit_conn->online = true;
                conn->user_data = new ConnectionMetadata<HTTPClientAdapter>(it->second->ext_unit_conn->connection_identifier, this);
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
    if(!conn->user_data) return;
    ConnectionMetadata<HTTPClientAdapter> *conn_metadata = static_cast<ConnectionMetadata<HTTPClientAdapter> * >(conn->user_data);
    
    //get connection info
    LMapConnectionInfoReadLock wlm = conn_metadata->class_instance->map_connection.getReadLockObject();
    MapConnectionInfoIterator conn_it = conn_metadata->class_instance->map_connection().find(conn_metadata->conn_uuid);
    if(conn_it == conn_metadata->class_instance->map_connection().end()) return;
    
    ConnectionInfoShrdPtr conn_info = conn_it->second;
    switch (event) {
        case MG_EV_CONNECT: {
            DBG<<" HTTP Client Connection event";
            int status = *((int *) event_data);
            conn_info->ext_unit_conn->online = (status==0);
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            int err = 0;
            struct websocket_message *wm = (struct websocket_message *) event_data;
            if(conn_info->ext_unit_conn->accepted_state <= 0) {
                bool is_accept_response = false;
                int accept_result = -1;
                //check accepted state
                checkAcceptResponse(wm, is_accept_response, accept_result);
                if(is_accept_response) {
                    conn_info->ext_unit_conn->accepted_state = accept_result;
                } else {
                    conn_info->ext_unit_conn->accepted_state = accept_result;
                    std::string json_string((const char *)wm->data, wm->size);
                    DBG << json_string;
                }
            } else {
                //accepted connection can received data
                ChaosUniquePtr<CDataBuffer> buffer(new CDataBuffer((const char *)wm->data,
                                                                   (uint32_t)wm->size));
                if((err = conn_metadata->class_instance->sendDataToEndpoint(*conn_info->ext_unit_conn,
                                                                            MOVE(buffer)))) {
                    //weh don't have found the sriealizer
                    ERR<< CHAOS_FORMAT("Error forwading data from connection uuid %1%", %conn_info->ext_unit_conn->connection_identifier);
                } else {
                    conn_metadata->class_instance->message_broadcasted++;
                }
            }
            break;
        }
        case MG_EV_CLOSE: {
            //manage the reconnection
            DBG<<" HTTP Client CLOSE event";
            //in this case concnretion info need to be put into  reconnection_queue
            //!beause conenciton need to be reopend
            //reset real connection
            CHAOS_ASSERT(conn_info->ext_unit_conn.get());
            conn_info->ext_unit_conn->online = false;
            conn_info->ext_unit_conn->accepted_state = -1;
            //set retry timeout after five seconds
            conn_info->next_reconnection_retry_ts = TimingUtil::getTimestampWithDelay(5000, true);
            delete(conn_metadata);
            break;
        }
            
        case MG_EV_POLL:{
            //execute opcode for connection
            ChaosWriteLock conn_wl(conn_info->smutex);
            while(conn_info->opcode_queue.empty() == false) {
                OpcodeShrdPtr op = conn_info->opcode_queue.front();
                conn_info->opcode_queue.pop();
                switch(op->op_type) {
                    case OpcodeInfoTypeSend: {
                        switch (op->data_opcode) {
                            case EUCMessageOpcodeWhole:
                                mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, op->data->getBuffer(), op->data->getBufferSize());
                                break;
                            case EUCPhaseStartFragment:
                                mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, op->data->getBuffer(), op->data->getBufferSize());
                                break;
                            case EUCPhaseContinueFragment:
                                mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, op->data->getBuffer(), op->data->getBufferSize());
                                break;
                            case EUCPhaseEndFragment:
                                mg_send_websocket_frame(conn, WEBSOCKET_OP_TEXT, op->data->getBuffer(), op->data->getBufferSize());
                                break;
                        }
                        break;
                    }
                    default:{break;}
                }
                op->wait_termination_semaphore.unlock();
            }
            break;
        }
    }
}
