/*
 *	HTTPAdapter.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/06/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/external_gateway/http_adapter/HTTPAdapter.h>

using namespace chaos::cu::external_gateway::http_adapter;

#define INFO    INFO_LOG(HTTPHelper)
#define DBG     DBG_LOG(HTTPHelper)
#define ERR     ERR_LOG(HTTPHelper)

HTTPAdapter::HTTPAdapter():
run(false),
nc(0){}

HTTPAdapter::~HTTPAdapter() {
    
}

void HTTPAdapter::init(void *init_data) throw (chaos::CException) {
    run = true;
    mg_mgr_init(&mgr, NULL);
    
    nc = mg_bind(&mgr, "8080", HTTPAdapter::eventHandler);
    if(nc == NULL) {CException(-1, "Error creating http connection", __PRETTY_FUNCTION__);}
    nc->user_data = this;
    
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = "";  // Serve current directory
    s_http_server_opts.enable_directory_listing = "no";
    //
    CObjectProcessingQueue<WorkRequest>::init(2);
    
    thread_poller.reset(new boost::thread(boost::bind(&HTTPAdapter::poller, this)));
}

void HTTPAdapter::deinit() throw (chaos::CException) {
    run = false;
    CObjectProcessingQueue<WorkRequest>::deinit();
    CObjectProcessingQueue<WorkRequest>::clear();
    thread_poller->join();
    
    mg_mgr_free(&mgr);
}

void HTTPAdapter::poller() {
    INFO << "Entering thread poller";
    while (run) {
        mg_mgr_poll(&mgr, 5);
    }
    INFO << "Leaving thread poller";
}

void HTTPAdapter::processBufferElement(WorkRequest *request,
                                       ElementManagingPolicy& policy) throw(CException) {
    mg_printf(request->nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(request->nc, "{ \"result\": %d }", 25);
    mg_send_http_chunk(request->nc, "", 0); /* Send empty chunk, the end of response */
}

void HTTPAdapter::eventHandler(struct mg_connection *nc, int ev, void *ev_data) {
    (void) nc;
    (void) ev_data;
    HTTPAdapter *adapter = static_cast<HTTPAdapter*>(nc->user_data);
    switch (ev) {
        case MG_EV_ACCEPT:{
            
            break;
        }
        case MG_EV_HTTP_REQUEST: {
            http_message *message = static_cast<http_message*>(ev_data);
            WorkRequest *req = new WorkRequest();
            req->message = message;
            req->nc = nc;
            adapter->push(req);
            //            mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
            //            mg_printf_http_chunk(nc, "{ \"result\": %d }", 25);
            //            mg_send_http_chunk(nc, "", 0);
            break;
        }
            
        case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
            char addr[32];
            http_message *message = static_cast<http_message*>(ev_data);
            const std::string uri(message->uri.p, message->uri.len);
            mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                                MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
            INFO << CHAOS_FORMAT("Received new connection for endoint %1% from %2%", %uri%addr);
            LMapEndpointReadLock wl = adapter->map_endpoint.getReadLockObject();
            if(adapter->map_endpoint().count(uri) == 0) {
                ERR << CHAOS_FORMAT("No class registered to endpoint %1%", %uri);
                mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);
                return;
            }
            
            //we can create a new connection
            
            LMapConnectionWriteLock wconnl = adapter->map_connection.getWriteLockObject();
            adapter->map_connection().insert(MapConnectionPair(reinterpret_cast<uintptr_t>(nc),
                                                               ChaosSharedPtr<HTTPExternalUnitConnection>(new HTTPExternalUnitConnection(nc,
                                                                                                                                         adapter->map_endpoint()[uri]))));
            break;
        }
            
        case MG_EV_WEBSOCKET_FRAME: {
            websocket_message *wm = (struct websocket_message *) ev_data;
            INFO << std::string((const char*)wm->data, wm->size);
            LMapConnectionReadLock wconnl = adapter->map_connection.getReadLockObject();
            adapter->map_connection()[reinterpret_cast<uintptr_t>(nc)]->handleWSIncomingData(wm);
            //mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, wm->data, wm->size);
            break;
        }
            
        case MG_EV_CLOSE:{
            LMapConnectionWriteLock wconnl = adapter->map_connection.getWriteLockObject();
            adapter->map_connection().erase(reinterpret_cast<uintptr_t>(nc));
            /* Disconnect. Tell everybody. */
            //            if (nc->flags & MG_F_IS_WEBSOCKET) {
            //                broadcast(nc, mg_mk_str("-- left"));
            //            }
            break;
        }
    }
}

int HTTPAdapter::registerEndpoint(ExternalUnitEndpoint& endpoint) {
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    if(map_endpoint().count(endpoint.getIdentifier()) != 0) return 0;
    map_endpoint().insert(MapEndpointPair(endpoint.getIdentifier(), &endpoint));
    return 0;
}

int HTTPAdapter::deregisterEndpoint(ExternalUnitEndpoint& endpoint) {
    LMapEndpointWriteLock wl = map_endpoint.getWriteLockObject();
    if(map_endpoint().count(endpoint.getIdentifier()) == 0) return 0;
    map_endpoint().erase(endpoint.getIdentifier());
    return 0;
}
