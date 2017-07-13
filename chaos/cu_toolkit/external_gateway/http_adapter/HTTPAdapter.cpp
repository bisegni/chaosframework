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
root_connection(0){}

HTTPAdapter::~HTTPAdapter() {
    
}

void HTTPAdapter::init(void *init_data) throw (chaos::CException) {
    run = true;
    mg_mgr_init(&mgr, NULL);
    
    root_connection = mg_bind(&mgr, "8080", HTTPAdapter::eventHandler);
    if(root_connection == NULL) {CException(-1, "Error creating http connection", __PRETTY_FUNCTION__);}
    root_connection->user_data = this;
    
    mg_set_protocol_http_websocket(root_connection);
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
        mg_mgr_poll(&mgr, 10);
    }
    INFO << "Leaving thread poller";
}

void HTTPAdapter::processBufferElement(WorkRequest *request,
                                       ElementManagingPolicy& policy) throw(CException) {
    switch(request->type) {
        case WorkRequestTypeHttpRequest: {
            //http_message *message = static_cast<http_message*>(request->message);
            mg_printf(request->nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
            mg_printf_http_chunk(request->nc, "{ \"result\": %d }", 25);
            mg_send_http_chunk(request->nc, "", 0); /* Send empty chunk, the end of response */
            break;
        }
        case WorkRequestTypeWSHandshakeRequest: {
            char addr[32];
            mg_sock_addr_to_str(&request->nc->sa, addr, sizeof(addr),
                                MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
            INFO << CHAOS_FORMAT("Received new connection for endoint %1% from %2%", %request->uri%addr);
            LMapEndpointReadLock wl = map_endpoint.getReadLockObject();
            if(map_endpoint().count(request->uri) == 0) {
                ERR << CHAOS_FORMAT("No class registered to endpoint %1%", %request->uri);
                mg_send_websocket_frame(request->nc, WEBSOCKET_OP_CLOSE, NULL, 0);
                return;
            }
            
            //we can create a new connection
            LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
            map_connection().insert(MapConnectionPair(reinterpret_cast<uintptr_t>(request->nc),
                                                      ChaosSharedPtr<HTTPExternalUnitConnection>(new HTTPExternalUnitConnection(request->nc,
                                                                                                                                map_endpoint()[request->uri]))));
            break;
        }
        case WorkRequestTypeWSFrame: {
            policy.elementHasBeenDetached = true;
            LMapConnectionReadLock wconnl = map_connection.getReadLockObject();
            map_connection()[reinterpret_cast<uintptr_t>(request->nc)]->handleWSIncomingData(ChaosUniquePtr<WorkRequest>(request));
            break;
        }
        case WorkRequestTypeWSCloseEvent: {
            LMapConnectionWriteLock wconnl = map_connection.getWriteLockObject();
            map_connection().erase(reinterpret_cast<uintptr_t>(request->nc));
            break;
        }
    }
    
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
            req->type = WorkRequestTypeHttpRequest;
            req->nc = nc;
            req->uri.assign(message->uri.p, message->uri.len);
            req->buffer.assign(message->message.p, message->message.p+message->message.len);
            adapter->push(req);
            break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
            http_message *message = static_cast<http_message*>(ev_data);
            WorkRequest *req = new WorkRequest();
            req->type = WorkRequestTypeWSHandshakeRequest;
            req->nc = nc;
            req->uri.assign(message->uri.p, message->uri.len);
            req->buffer.assign(message->message.p, message->message.p+message->message.len);
            adapter->push(req);
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            websocket_message *message = static_cast<websocket_message*>(ev_data);
            WorkRequest *req = new WorkRequest();
            req->type = WorkRequestTypeWSFrame;
            req->buffer.assign(message->data, message->data+message->size);
            req->nc = nc;
            adapter->push(req);
            break;
        }
        case MG_EV_CLOSE:{
            WorkRequest *req = new WorkRequest();
            req->type = WorkRequestTypeWSCloseEvent;
            req->nc = nc;
            adapter->push(req);
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
