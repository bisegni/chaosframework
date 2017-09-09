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

#include <chaos_micro_unit_toolkit/connection/connection_adapter/AbstractConnectionAdapter.h>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::connection_adapter;

AbstractConnectionAdapter::AbstractConnectionAdapter(const std::string& _protocol_endpoint,
                                                 const std::string& _protocol_option):
protocol_endpoint(_protocol_endpoint),
protocol_option(_protocol_option),
connection_status(ConnectionStateDisconnected),
adapter_request_id(0){}

AbstractConnectionAdapter::~AbstractConnectionAdapter() {}

void AbstractConnectionAdapter::handleReceivedMessage(data::DataPackSharedPtr& received_message) {
    //checn whenever the message is a response or spontaneus message
    if(received_message->hasKey("etx_request_id") &&
       received_message->isInt32("etx_request_id")) {
        map_req_id_response.insert(MapRequestIDResponsePair((uint32_t)received_message->getInt32("etx_request_id"),
                                                            received_message));
    } else {
        queue_received_messages.push(received_message);
    }
}

int AbstractConnectionAdapter::sendMessage(data::DataPackUniquePtr& message) {
    if(connection_status != ConnectionStateAccepted){
        return -1;
    }
    return sendRawMessage(message);
}

int AbstractConnectionAdapter::sendRequest(data::DataPackUniquePtr& message,
                                         uint32_t& request_id) {
    request_id = adapter_request_id++;
    message->addInt32("ext_request_id", request_id);
    return sendMessage(message);
}

bool AbstractConnectionAdapter::hasMoreMessage() {
    return queue_received_messages.size();
}

DataPackSharedPtr AbstractConnectionAdapter::getNextMessage() {
    if(queue_received_messages.size() == 0) {
        return DataPackSharedPtr();
    } else {
        DataPackSharedPtr result = queue_received_messages.front();
        queue_received_messages.pop();
        return result;
    }
}

bool AbstractConnectionAdapter::hasResponse() {
    return map_req_id_response.size()>0;
}

bool AbstractConnectionAdapter::hasResponseAvailable(uint32_t request_id) {
    return map_req_id_response.find(request_id) != map_req_id_response.end();
}

DataPackSharedPtr AbstractConnectionAdapter::retrieveRequestResponse(uint32_t request_id) {
    return DataPackSharedPtr();
}

const ConnectionState& AbstractConnectionAdapter::getConnectionState() const {
    return connection_status;
}
