/*
 * Copyright 2012, 2017 INFN
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

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/external_unit/ExternalUnitManager.h>
#include <chaos/common/external_unit/http_adapter/HTTPServerAdapter.h>
#include <chaos/common/external_unit/external_unit_constants.h>

#include <boost/lexical_cast.hpp>

#include <string.h>
#include <algorithm>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::external_unit::http_adapter;

#define INFO    INFO_LOG(HTTPServerAdapter)
#define DBG     DBG_LOG(HTTPServerAdapter)
#define ERR     ERR_LOG(HTTPServerAdapter)


HTTPServerAdapter::HTTPServerAdapter():
run(false),
root_connection(0){}

HTTPServerAdapter::~HTTPServerAdapter() {}

void HTTPServerAdapter::init(void *init_data) throw (chaos::CException) {
    //scsan configuration
    setting.thread_number = GlobalConfiguration::getInstance()->getOption<unsigned int>(CU_EG_OPT_WORKER_THREAD_NUMBER);
    if(GlobalConfiguration::getInstance()->hasOption(CU_EG_OPT_WORKER_KV_PARAM)) {
        std::map<string, string> http_param;
        GlobalConfiguration::fillKVParameter(http_param,
                                             GlobalConfiguration::getInstance()->getOption< std::vector<std::string> >(CU_EG_OPT_WORKER_KV_PARAM),
                                             "");
        //check publishing port paramter
        if(http_param.count(CU_EG_HTTP_PUBLISHING_PORT)) {
            setting.publishing_port = http_param[CU_EG_HTTP_PUBLISHING_PORT];
        }
    }
    run = true;
    mg_mgr_init(&mgr, NULL);
    
    int http_port = InetUtility::scanForLocalFreePort(boost::lexical_cast<int>(setting.publishing_port));
    const std::string http_port_str = boost::lexical_cast<std::string>(http_port);
    root_connection = mg_bind(&mgr, http_port_str.c_str(), HTTPServerAdapter::eventHandler);
    if(root_connection == NULL) {throw CException(-1, "Error creating http connection", __PRETTY_FUNCTION__);}
    root_connection->user_data = this;
    
    mg_set_protocol_http_websocket(root_connection);
    s_http_server_opts.document_root = "";  // Serve current directory
    s_http_server_opts.enable_directory_listing = "no";
    //
    CObjectProcessingQueue<WorkRequest>::init(setting.thread_number);
    
    thread_poller.reset(new boost::thread(boost::bind(&HTTPServerAdapter::poller, this)));
}

void HTTPServerAdapter::deinit() throw (chaos::CException) {
    run = false;
    CObjectProcessingQueue<WorkRequest>::deinit();
    CObjectProcessingQueue<WorkRequest>::clear();
    thread_poller->join();
    
    mg_mgr_free(&mgr);
}

void HTTPServerAdapter::poller() {
    INFO << "Entering thread poller";
    while (run) {
        mg_mgr_poll(&mgr, 1);
    }
    INFO << "Leaving thread poller";
}

const std::string HTTPServerAdapter::getSerializationType(http_message *http_message) {
    CHAOS_ASSERT(http_message);
    struct mg_str *value = mg_get_http_header(http_message, "Content-Type");
    if(value == NULL) {
        value = mg_get_http_header(http_message, "content-type");
        if(value == NULL) return "";
    }
    
    std::string ser_type(value->p, value->len);
    std::transform(ser_type.begin(), ser_type.end(), ser_type.begin(), ::tolower);
    return ser_type;
}

void  HTTPServerAdapter::manageWSHandshake(WorkRequest& wr) {
    char addr[32];
    mg_sock_addr_to_str(&wr.nc->sa, addr, sizeof(addr),
                        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
    INFO << CHAOS_FORMAT("Received new connection for endoint %1% from %2%", %wr.uri%addr);
    LMapEndpointReadLock wl = map_endpoint.getReadLockObject();
    MapEndpointIterator endpoint_it = map_endpoint().find(wr.uri);
    if(endpoint_it == map_endpoint().end()) {
        sendWSJSONError(wr.nc,
                        -1,
                        CHAOS_FORMAT("No endpoint found for '%1%'", %wr.uri),
                        false);
        sendWSJSONAcceptedConnection(wr.nc,
                                     false,
                                     true);
        return;
    }
    
    //check if endpoint can accept more connection
    if(endpoint_it->second->canAcceptMoreConnection() == false) {
        //write error for no more connection accepted by endpoint
        sendWSJSONError(wr.nc,
                        -2,
                        CHAOS_FORMAT("No more connection accepted by endpoint '%1%'", %wr.uri),
                        false);
        sendWSJSONAcceptedConnection(wr.nc,
                                     false,
                                     true);
    } else {
        //get instance for serializer
        ChaosUniquePtr<serialization::AbstractExternalSerialization> serializer = ExternalUnitManager::getInstance()->getNewSerializationInstanceForType(wr.s_type);
        if(!serializer.get()) {
            //write error for no more connection accepted by endpoint
            sendWSJSONError(wr.nc,
                            -3,
                            CHAOS_FORMAT("Unable to find the serialization plugin for '%1%'", %wr.s_type),
                            false);
            sendWSJSONAcceptedConnection(wr.nc,
                                         false,
                                         true);
        } else {
            //we can create a new connection
            ChaosSharedPtr<ExternalUnitConnection> conn_ptr(new ExternalUnitConnection(this,
                                                                                       endpoint_it->second,
                                                                                       ChaosMoveOperator(serializer)));
            LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
            map_connection().insert(MapConnectionPair(reinterpret_cast<uintptr_t>(wr.nc),
                                                      conn_ptr));
            //add the mapping from mongoose conenciton and ext unit one
            map_m_conn_ext_conn.insert(reinterpret_cast<uintptr_t>(wr.nc),
                                       conn_ptr->connection_identifier);
            sendWSJSONAcceptedConnection(wr.nc,
                                         true,
                                         false);
        }
    }
}

void HTTPServerAdapter::processBufferElement(WorkRequest *request,
                                             ElementManagingPolicy& policy) throw(CException) {
    switch(request->r_type) {
        case WorkRequestTypeHttpRequest: {
            //http_message *message = static_cast<http_message*>(wr.message);
            mg_printf(request->nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
            mg_printf_http_chunk(request->nc, "!CHAOS Control External gateway not support http get or post");
            mg_send_http_chunk(request->nc, "", 0); /* Send empty chunk, the end of response */
            break;
        }
        case WorkRequestTypeWSHandshakeRequest: {
            manageWSHandshake(*request);
            break;
        }
        case WorkRequestTypeWSFrame: {
            int err = 0;
            policy.elementHasBeenDetached = true;
            LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
            if(map_connection().count(reinterpret_cast<uintptr_t>(request->nc))) {
                if((err = sendDataToEndpoint(*map_connection()[reinterpret_cast<uintptr_t>(request->nc)],
                                             ChaosMoveOperator(request->buffer)))) {
                    //weh don't have found the sriealizer
                    const std::string error = CHAOS_FORMAT("{error:%1%,message:\"%2%\"}", %err%map_connection()[reinterpret_cast<uintptr_t>(request->nc)]->getEndpointIdentifier());
                    mg_send_websocket_frame(request->nc, WEBSOCKET_OP_TEXT, error.c_str(), error.size());
                }
            }
            break;
        }
        case WorkRequestTypeWSCloseEvent: {
            LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
            map_connection().erase(reinterpret_cast<uintptr_t>(request->nc));
            map_m_conn_ext_conn.removebyLeftKey(reinterpret_cast<uintptr_t>(request->nc));
            break;
        }
            
        default:{break;}
    }
}

void HTTPServerAdapter::eventHandler(mg_connection *nc, int ev, void *ev_data) {
    (void) nc;
    (void) ev_data;
    HTTPServerAdapter *adapter = static_cast<HTTPServerAdapter*>(nc->user_data);
    switch (ev) {
        case MG_EV_ACCEPT:{
            break;
        }
        case MG_EV_HTTP_REQUEST: {
            http_message *message = static_cast<http_message*>(ev_data);
            WorkRequest *req = new WorkRequest(message->message.p,
                                               (uint32_t)message->message.len);
            req->r_type = WorkRequestTypeHttpRequest;
            req->s_type = getSerializationType(message);
            req->nc = nc;
            req->uri.assign(message->uri.p, message->uri.len);
            adapter->push(req);
            break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
            http_message *message = static_cast<http_message*>(ev_data);
            WorkRequest *req = new WorkRequest(message->message.p,
                                               (uint32_t)message->message.len);
            req->r_type = WorkRequestTypeWSHandshakeRequest;
            req->s_type = getSerializationType(message);
            req->nc = nc;
            req->uri.assign(message->uri.p, message->uri.len);
            if(req->s_type.size() == 0) {
                std::string error = "Serialization type not found";
                mg_send_head(nc, 400, error.size(), "Content-Type: text/plain");
                mg_printf(nc, "%s", error.c_str());
            }else {
                adapter->push(req);
            }
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            websocket_message *message = static_cast<websocket_message*>(ev_data);
            WorkRequest *req = new WorkRequest((const char *)message->data,
                                               (uint32_t)message->size);
            req->r_type = WorkRequestTypeWSFrame;
            req->nc = nc;
            adapter->push(req);
            break;
        }
        case MG_EV_CLOSE:{
            WorkRequest *req = new WorkRequest();
            req->r_type = WorkRequestTypeWSCloseEvent;
            req->nc = nc;
            adapter->push(req);
            break;
        }
    }
}

int HTTPServerAdapter::registerEndpoint(ExternalUnitEndpoint& endpoint) {
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    if(map_endpoint().count(endpoint.getIdentifier()) != 0) return 0;
    map_endpoint().insert(MapEndpointPair(endpoint.getIdentifier(), &endpoint));
    return 0;
}

int HTTPServerAdapter::deregisterEndpoint(ExternalUnitEndpoint& endpoint) {
    //lock for write conenction and endpoint
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    
    if(map_endpoint().count(endpoint.getIdentifier()) == 0) return 0;
    
    LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
    for(MapConnectionIterator it = map_connection().begin(),
        end = map_connection().end();
        it != end;) {
        if(it->second->getEndpointIdentifier().compare(endpoint.getIdentifier()) == 0) {
            //iterator need to be removed so we force to close the connection
            it->second->closeConnection();
            //remove connection
            map_connection().erase(it++);
        } else {
            ++it;
        }
    }
    map_endpoint().erase(endpoint.getIdentifier());
    return 0;
}

int HTTPServerAdapter::sendDataToConnection(const std::string& connection_identifier,
                                            const chaos::common::data::CDBufferUniquePtr data,
                                            const EUCMessageOpcode opcode) {
    LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
    if(!map_m_conn_ext_conn.hasRightKey(static_cast<std::string>(connection_identifier))) return -1;
    mg_connection *nc = reinterpret_cast<mg_connection*>(map_m_conn_ext_conn.findByRightKey(static_cast<std::string>(connection_identifier)));
    switch (opcode) {
        case EUCMessageOpcodeWhole:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
            break;
        case EUCPhaseStartFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
            break;
            
        case EUCPhaseContinueFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
            break;
            
        case EUCPhaseEndFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
            break;
    }
    return 0;
}

int HTTPServerAdapter::closeConnection(const std::string& connection_identifier) {
    LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
    if(!map_m_conn_ext_conn.hasRightKey(static_cast<std::string>(connection_identifier))) return -1;
    mg_connection *nc = reinterpret_cast<mg_connection*>(map_m_conn_ext_conn.findByRightKey(static_cast<std::string>(connection_identifier)));
    mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, "", 0);
    return 0;
}
