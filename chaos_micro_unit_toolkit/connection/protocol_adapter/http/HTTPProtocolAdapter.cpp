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

#include <chaos_micro_unit_toolkit/connection/protocol_adapter/http/HTTPProtocolAdapter.h>

#define ACCEPTED_CONNECTION_KEY "accepted_connection"

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
    if(connection_status != ConnectionStateDisconnected) return -1;
    connection_status = ConnectionStateConnecting;
    mg_mgr_init(&mgr, NULL);
    root_conn = mg_connect_ws(&mgr,
                              HTTPProtocolAdapter::ev_handler,
                              AbstractProtocolAdapter::protocol_endpoint.c_str(),
                              "ChaosExternalUnit", AbstractProtocolAdapter::protocol_option.c_str());
    if (root_conn != NULL) {
        root_conn->user_data = this;
    } else {
        connection_status = ConnectionStateDisconnected;
        err = -1;
    }
    return err;
    
}

void HTTPProtocolAdapter::poll(int32_t milliseconds_wait) {
    mg_mgr_poll(&mgr, milliseconds_wait);
}

int HTTPProtocolAdapter::close() {
    if(connection_status != ConnectionStateConnected) return -1;
    mg_send_websocket_frame(root_conn, WEBSOCKET_OP_CLOSE, NULL, 0);
    mg_mgr_free(&mgr);
    return 0;
}

#pragma mark PrivateMethod

int HTTPProtocolAdapter::sendRawMessage(DataPackUniquePtr& message) {
    std::string to_send = message->toUnformattedString();
    mg_send_websocket_frame(root_conn, WEBSOCKET_OP_TEXT, to_send.c_str(), to_send.size());
    return 0;
}

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
            data::DataPackSharedPtr received_msg_shrd_ptr(DataPack::newFromBuffer((const char*)wm->data, wm->size).release());
            if(http_instance->connection_status != ConnectionStateAccepted) {
                //at this time we need to manage the receivedm of the data unitl we not received
                //the dapack taht informa us for the connection accepted
                if(received_msg_shrd_ptr->hasKey(ACCEPTED_CONNECTION_KEY) &&
                   received_msg_shrd_ptr->isInt32(ACCEPTED_CONNECTION_KEY)) {
                    int32_t accepted_value = received_msg_shrd_ptr->getInt32(ACCEPTED_CONNECTION_KEY);
                    switch (accepted_value) {
                        case 1:
                            //ok
                            http_instance->connection_status = ConnectionStateAccepted;
                            break;
                            
                        default:
                            http_instance->connection_status = ConnectionStateNotAccepted;
                            break;
                    }
                    //this datapack must not be added to the user queue message because is a pack for sincronization
                    break;
                }
            }
            http_instance->handleReceivedMessage(received_msg_shrd_ptr);
            break;
        }
        case MG_EV_CLOSE: {
            http_instance->connection_status = ConnectionStateDisconnected;
            break;
        }
    }
}
