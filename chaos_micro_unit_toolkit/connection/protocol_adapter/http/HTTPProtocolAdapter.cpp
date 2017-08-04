/*
 *	HTTPProtocolAdapter.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos_micro_unit_toolkit/connection/protocol_adapter/http/HTTPProtocolAdapter.h>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter::http;

const chaos::micro_unit_toolkit::connection::ProtocolType HTTPProtocolAdapter::protocol_type = ProtocolTypeHTTP;

HTTPProtocolAdapter::HTTPProtocolAdapter(const std::string& endpoint,
                                         const std::string& connection_header):
AbstractProtocolAdapter(endpoint,
                        connection_header),
root_conn(NULL){}

HTTPProtocolAdapter::~HTTPProtocolAdapter() {}

int HTTPProtocolAdapter::connect() {
    int err = 0;
    connection_status = ConnectionStateConnecting;
    mg_mgr_init(&mgr, NULL);
    root_conn = mg_connect_ws(&mgr,
                              HTTPProtocolAdapter::ev_handler,
                              AbstractProtocolAdapter::protocol_endpoint.c_str(),
                              "ChaosExternalUnit", NULL);
    if (root_conn != NULL) {
        root_conn->user_data = this;
    } else {
        connection_status = ConnectionStateDisconnected;
        err = -1;
    }
    return err;

}

int HTTPProtocolAdapter::sendRawMessage(DataPackUniquePtr& message) {
    std::string to_send = message->toUnformattedString();
    mg_send_websocket_frame(root_conn, WEBSOCKET_OP_TEXT, to_send.c_str(), to_send.size());
    return 0;
}

int HTTPProtocolAdapter::close() {
    mg_send_websocket_frame(root_conn, WEBSOCKET_OP_CLOSE, NULL, 0);
    mg_mgr_free(&mgr);
    return 0;
}

#pragma mark PrivateMethod
void HTTPProtocolAdapter::ev_handler(struct mg_connection *conn,
                                     int event,
                                     void *event_data) {
    HTTPProtocolAdapter *http_instance = static_cast<HTTPProtocolAdapter*>(conn->user_data);
    
    switch (event) {
        case MG_EV_CONNECT: {
            int status = *((int *) event_data);
            if (status != 0) {
                printf("Connection to '%s' failed with error: %d\n", http_instance->protocol_endpoint.c_str(), status);
                http_instance->connection_status = ConnectionStateConnectionError;
            }
            break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            http_instance->connection_status = ConnectionStateConnected;
            break;
        }

        case MG_EV_WEBSOCKET_FRAME: {
            struct websocket_message *wm = (struct websocket_message *) event_data;
            data::DataPackSharedPtr rece_msg_shrd_ptr(DataPack::newFromBuffer((const char*)wm->data, wm->size).release());
            http_instance->handleReceivedMessage(rece_msg_shrd_ptr);
            break;
        }
        case MG_EV_CLOSE: {
            http_instance->connection_status = ConnectionStateDisconnected;
            break;
        }
    }
}
