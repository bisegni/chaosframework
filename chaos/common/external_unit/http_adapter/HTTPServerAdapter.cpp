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
    setting.publishing_port = "8080";
    setting.thread_number = GlobalConfiguration::getInstance()->getOption<unsigned int>(InitOption::OPT_UNIT_GATEWAY_WORKER_THREAD_NUMBER);
    if(GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_UNIT_GATEWAY_ADAPTER_KV_PARAM)) {
        std::map<string, string> http_param;
        GlobalConfiguration::fillKVParameter(http_param,
                                             GlobalConfiguration::getInstance()->getOption< std::vector<std::string> >(InitOption::OPT_UNIT_GATEWAY_ADAPTER_KV_PARAM),
                                             "");
        //check publishing port paramter
        if(http_param.count(OPT_HTTP_PUBLISHING_PORT)) {
            setting.publishing_port = http_param[OPT_HTTP_PUBLISHING_PORT];
        }
    }
    run = true;
    mg_mgr_init(&mgr, NULL);
    const std::string http_port_str = boost::lexical_cast<std::string>(InetUtility::scanForLocalFreePort(boost::lexical_cast<int>(setting.publishing_port)));
    root_connection = mg_bind(&mgr, http_port_str.c_str(), HTTPServerAdapter::eventHandler);
    if(root_connection == NULL) {throw CException(-1, "Error creating http connection", __PRETTY_FUNCTION__);}
    root_connection->user_data = this;
    
    mg_set_protocol_http_websocket(root_connection);
    s_http_server_opts.document_root = "";  // Serve current directory
    s_http_server_opts.enable_directory_listing = "no";
    //
    CObjectProcessingQueue<ServerWorkRequest>::init(setting.thread_number);
    
    thread_poller.reset(new boost::thread(boost::bind(&HTTPServerAdapter::poller, this)));
}

void HTTPServerAdapter::deinit() throw (chaos::CException) {
    run = false;
    thread_poller->join();
    CObjectProcessingQueue<ServerWorkRequest>::deinit();
    CObjectProcessingQueue<ServerWorkRequest>::clear();
    mg_mgr_free(&mgr);
    map_connection().clear();
}

void HTTPServerAdapter::poller() {
    INFO << "Entering thread poller";
    while (run) {
        mg_mgr_poll(&mgr, 1);
        
        //consume opcode queue
        {
            LOpcodeShrdPtrQueueReadLock wconnl = post_evt_op_queue.getReadLockObject();
            while(post_evt_op_queue().empty() == false) {
                OpcodeShrdPtr op = post_evt_op_queue().front();
                post_evt_op_queue().pop();
                wconnl->unlock();
                
                switch(op->op_type) {
                    case OpcodeInfoTypeCloseConnection:{
                        //!op->identifier now is the identifier of the external connection
                        //search real connection
                        struct mg_connection *c = NULL;
                        for (c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c)) {
                            if(!c->user_data) continue;
                            ConnectionMetadata<HTTPServerAdapter> *conn_metadata = static_cast<ConnectionMetadata<HTTPServerAdapter> * >(c->user_data);
                            if(conn_metadata->conn_uuid.compare(op->identifier) == 0) {
                                break;
                            }
                        }
                        if(c) {
                            ConnectionMetadata<HTTPServerAdapter> *conn_metadata = static_cast<ConnectionMetadata<HTTPServerAdapter> * >(c->user_data);
                            LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
                            map_connection().erase(op->identifier);
                            c->flags |= MG_F_CLOSE_IMMEDIATELY;
                            delete(conn_metadata);
                            c->user_data = NULL;
                        }
                        break;
                    }
                        
                    case OpcodeInfoTypeCloseConnectionForEndpoint:{
                        LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
                        for(MapConnectionIterator it = map_connection().begin(),
                            end = map_connection().end();
                            it != end;) {
                            //!op->identifier now is the identifier of the enpoint
                            if(it->second->external_connection->getEndpointIdentifier().compare(op->identifier) == 0) {
                                //connection below to the endpoint that need to be removed
                                struct mg_connection *c = NULL;
                                for (c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c)) {
                                    if(!c->user_data) continue;
                                    ConnectionMetadata<HTTPServerAdapter> *conn_metadata = static_cast<ConnectionMetadata<HTTPServerAdapter> * >(c->user_data);
                                    if(conn_metadata->conn_uuid.compare(it->second->external_connection->connection_identifier) == 0) {
                                        break;
                                    }
                                }
                                
                                
                                if(c) {
                                    c->flags |= MG_F_CLOSE_IMMEDIATELY;
                                }
                                //remove connection
                                map_connection().erase(it++);
                            } else {
                                ++it;
                            }
                        }
                        break;
                    }
                    default:{break;}
                }
                op->wait_termination_semaphore.unlock();
                wconnl->lock();
            }
        }
    }
    INFO << "Leaving thread poller";
}

const std::string HTTPServerAdapter::getSerializationType(http_message *http_message) {
    struct mg_str *value = mg_get_http_header(http_message, "Content-Type");
    if(value == NULL) {
        value = mg_get_http_header(http_message, "content-type");
        if(value == NULL) return "";
    }
    
    std::string ser_type(value->p, value->len);
    std::transform(ser_type.begin(), ser_type.end(), ser_type.begin(), ::tolower);
    return ser_type;
}

void  HTTPServerAdapter::manageWSHandshake(mg_connection *nc,
                                           http_message *message) {
    
    const std::string s_type = getSerializationType(message);
    if(s_type.size() == 0) {
        std::string error = "Serialization type not found";
        mg_send_head(nc, 400, error.size(), "Content-Type: text/plain");
        mg_printf(nc, "%s", error.c_str());
        return;
    }
    
    char addr[32];
    std::string uri(message->uri.p, message->uri.len);
    mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
    INFO << CHAOS_FORMAT("Received new connection for endoint %1% from %2%", %uri%addr);
    LMapEndpointReadLock wl = map_endpoint.getReadLockObject();
    MapEndpointIterator endpoint_it = map_endpoint().find(uri);
    if(endpoint_it == map_endpoint().end()) {
        sendWSJSONError(nc,
                        -1,
                        CHAOS_FORMAT("No endpoint found for '%1%'", %uri),
                        false);
        sendWSJSONAcceptedConnection(nc,
                                     false,
                                     true);
        return;
    }
    
    //check if endpoint can accept more connection
    if(endpoint_it->second->canAcceptMoreConnection() == false) {
        //write error for no more connection accepted by endpoint
        sendWSJSONError(nc,
                        -2,
                        CHAOS_FORMAT("No more connection accepted by endpoint '%1%'", %uri),
                        false);
        sendWSJSONAcceptedConnection(nc,
                                     false,
                                     true);
    } else {
        //get instance for serializer
        ChaosUniquePtr<serialization::AbstractExternalSerialization> serializer = ExternalUnitManager::getInstance()->getNewSerializationInstanceForType(s_type);
        if(!serializer.get()) {
            //write error for no more connection accepted by endpoint
            sendWSJSONError(nc,
                            -3,
                            CHAOS_FORMAT("Unable to find the serialization plugin for '%1%'", %s_type),
                            false);
            sendWSJSONAcceptedConnection(nc,
                                         false,
                                         true);
        } else {
            //we can create a new connection
            ChaosSharedPtr<ExternalUnitConnection> conn_ptr(new ExternalUnitConnection(this,
                                                                                       endpoint_it->second,
                                                                                       ChaosMoveOperator(serializer)));
            
            ChaosUniquePtr< ConnectionMetadata<HTTPServerAdapter> > connection_metadata = ChaosUniquePtr< ConnectionMetadata<HTTPServerAdapter> >(new ConnectionMetadata<HTTPServerAdapter>(conn_ptr->connection_identifier, this));
            LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
            map_connection().insert(MapConnectionPair(conn_ptr->connection_identifier,
                                                      ChaosSharedPtr<RemoteConnectionInfo>(new RemoteConnectionInfo(conn_ptr))));
            //all is gone well so we need to associate the connection metadata to the real connection
            nc->user_data = connection_metadata.release();
            sendWSJSONAcceptedConnection(nc,
                                         true,
                                         false);
        }
    }
}

void HTTPServerAdapter::processBufferElement(ServerWorkRequest *request,
                                             ElementManagingPolicy& policy) throw(CException) {
    switch(request->r_type) {
        case WorkRequestTypeWSFrame: {
            int err = 0;
            LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
            MapConnectionIterator conn_it = map_connection().find(request->connection_uuid);
            if(conn_it == map_connection().end()) {
                return;
            }
            if((err = sendDataToEndpoint(*conn_it->second->external_connection,
                                         ChaosMoveOperator(request->buffer)))) {
                //add error message to the queue
//                const std::string error = CHAOS_FORMAT("{error:%1%,message:\"%2%\"}", %err%map_connection()[reinterpret_cast<uintptr_t>(request->nc)]->getEndpointIdentifier());
//                mg_send_websocket_frame(request->nc, WEBSOCKET_OP_TEXT, error.c_str(), error.size());
            }
            break;
        }
        default:{break;}
    }
}

void HTTPServerAdapter::consumeConenctionMessageQueue(mg_connection *nc) {
    ConnectionMetadata<HTTPServerAdapter> *connection_metadata = static_cast< ConnectionMetadata<HTTPServerAdapter>* >(nc->user_data);
    CHAOS_ASSERT(connection_metadata)
    LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
    MapConnectionIterator conn_it = map_connection().find(connection_metadata->conn_uuid);
    if(conn_it == map_connection().end()) {
        return;
    }
    while(conn_it->second->message_queue.empty() == false) {
        OpcodeShrdPtr op = conn_it->second->message_queue.front();
        conn_it->second->message_queue.pop();
        switch(op->op_type) {
            case OpcodeInfoTypeSend:{
                switch (op->data_opcode) {
                    case EUCMessageOpcodeWhole:
                        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, op->data->getBuffer(), op->data->getBufferSize());
                        break;
                    case EUCPhaseStartFragment:
                        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, op->data->getBuffer(), op->data->getBufferSize());
                        break;
                        
                    case EUCPhaseContinueFragment:
                        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, op->data->getBuffer(), op->data->getBufferSize());
                        break;
                        
                    case EUCPhaseEndFragment:
                        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, op->data->getBuffer(), op->data->getBufferSize());
                        break;
                }
                break;
            }
            default:{break;}
        }
        op->wait_termination_semaphore.unlock();
    }
}

void HTTPServerAdapter::eventHandler(mg_connection *nc, int ev, void *ev_data) {
    switch (ev) {
        case MG_EV_ACCEPT:{
            //new conenction has been accepted
            break;
        }
        case MG_EV_HTTP_REQUEST: {
            HTTPServerAdapter *adapter = static_cast<HTTPServerAdapter*>(nc->user_data);
            if(!adapter->run) return;
            mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
            mg_printf_http_chunk(nc, "!CHAOS Control External gateway not support http get or post");
            mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */

            break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
            HTTPServerAdapter *adapter = static_cast<HTTPServerAdapter*>(nc->user_data);
            if(!adapter->run) return;
            adapter->manageWSHandshake(nc,
                                       static_cast<http_message*>(ev_data));
            
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            ConnectionMetadata<HTTPServerAdapter> *connection_metadata = static_cast< ConnectionMetadata<HTTPServerAdapter>* >(nc->user_data);
            websocket_message *message = static_cast<websocket_message*>(ev_data);
            ServerWorkRequest *req = new ServerWorkRequest(connection_metadata->conn_uuid,
                                                           (const char *)message->data,
                                                           (uint32_t)message->size);
            connection_metadata->class_instance->push(req);
            break;
        }
            
        case MG_EV_CLOSE:{
            ConnectionMetadata<HTTPServerAdapter> *connection_metadata = static_cast< ConnectionMetadata<HTTPServerAdapter>* >(nc->user_data);
            LMapConnectionWriteLock wconnl = connection_metadata->class_instance->map_connection.getWriteLockObject();
            connection_metadata->class_instance->map_connection().erase(connection_metadata->conn_uuid);
            delete(connection_metadata);
            nc->user_data = NULL;
            break;
        }
            
        case MG_EV_POLL:
            ConnectionMetadata<HTTPServerAdapter> *connection_metadata = static_cast< ConnectionMetadata<HTTPServerAdapter>* >(nc->user_data);
            connection_metadata->class_instance->consumeConenctionMessageQueue(nc);
            break;
    }
}

void HTTPServerAdapter::sendWSJSONAcceptedConnection(mg_connection *nc,
                                                     bool accepted,
                                                     bool close_connection) {
    chaos::common::data::CDataWrapper err_data_pack;
    err_data_pack.addInt32Value("accepted_connection", accepted);
    const std::string accepted_json = err_data_pack.getCompliantJSONString();
    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, accepted_json.c_str(), accepted_json.size());
    if(close_connection){mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);}
}

int HTTPServerAdapter::registerEndpoint(ExternalUnitServerEndpoint& endpoint) {
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    if(map_endpoint().count(endpoint.getIdentifier()) != 0) return 0;
    map_endpoint().insert(MapEndpointPair(endpoint.getIdentifier(), &endpoint));
    return 0;
}

int HTTPServerAdapter::deregisterEndpoint(ExternalUnitServerEndpoint& endpoint) {
    //lock for write conenction and endpoint
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    MapEndpointIterator me_it = map_endpoint().find(endpoint.getIdentifier());
    if(me_it == map_endpoint().end()) return 0;
    //at this point no new conneciton can be associated to the endpoint
    me_it->second = NULL;
    map_endpoint().erase(me_it);
    //set opcode to remove all connection to this endpoint
    OpcodeShrdPtr op(new Opcode());
    //    map_endpoint().erase(endpoint.getIdentifier());
    {
        LOpcodeShrdPtrQueueWriteLock wconnl = post_evt_op_queue.getWriteLockObject();
        op->identifier = endpoint.getIdentifier();
        op->op_type = OpcodeInfoTypeCloseConnectionForEndpoint;
        post_evt_op_queue().push(op);
    }
    //we need to wait that opcode has terminated
    op->wait_termination_semaphore.wait();
    
    return 0;
}

int HTTPServerAdapter::sendDataToConnection(const std::string& connection_identifier,
                                            chaos::common::data::CDBufferUniquePtr data,
                                            const EUCMessageOpcode opcode) {
    LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
    MapConnectionIterator conn_it = map_connection().find(connection_identifier);
    if(conn_it == map_connection().end()) {
        return 0;
    }
    OpcodeShrdPtr op(new Opcode());
    op->identifier = connection_identifier;
    op->op_type = OpcodeInfoTypeSend;
    op->data = ChaosMoveOperator(data);
    op->data_opcode = opcode;
    conn_it->second->message_queue.push(op);
    return 0;
}

int HTTPServerAdapter::closeConnection(const std::string& connection_identifier) {
    LOpcodeShrdPtrQueueWriteLock wconnl = post_evt_op_queue.getWriteLockObject();
    OpcodeShrdPtr op(new Opcode());
    op->identifier = connection_identifier;
    op->op_type = OpcodeInfoTypeCloseConnection;
    post_evt_op_queue().push(op);
    return 0;
}
