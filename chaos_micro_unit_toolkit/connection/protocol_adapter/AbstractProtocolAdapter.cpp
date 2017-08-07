/*
 *	AbstractProtocolAdapter.cpp
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

#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

AbstractProtocolAdapter::AbstractProtocolAdapter(const std::string& _protocol_endpoint,
                                                 const std::string& _protocol_option):
protocol_endpoint(_protocol_endpoint),
protocol_option(_protocol_option),
connection_status(ConnectionStateDisconnected),
adapter_request_id(0){}

AbstractProtocolAdapter::~AbstractProtocolAdapter() {}

void AbstractProtocolAdapter::handleReceivedMessage(data::DataPackSharedPtr& received_message) {
    //checn whenever the message is a response or spontaneus message
    if(received_message->hasKey("request_id") &&
       received_message->isInt32("request_id")) {
        map_req_id_response.insert(MapRequestIDResponsePair((uint32_t)received_message->getInt32("request_id"),
                                                            received_message));
    } else {
        queue_received_messages.push(received_message);
    }
}

int AbstractProtocolAdapter::sendMessage(data::DataPackUniquePtr& message) {
    if(connection_status != ConnectionStateConnected){
        printf("No connection to '%s'", protocol_endpoint.c_str());
        return -1;
    }
    return sendRawMessage(message);
}

int AbstractProtocolAdapter::sendRequest(data::DataPackUniquePtr& message,
                                         uint32_t& request_id) {
    request_id = adapter_request_id++;
    message->addInt32("request_id", request_id);
    return sendMessage(message);
}

bool AbstractProtocolAdapter::hasMoreMessage() {
    return queue_received_messages.size()>0;
}

DataPackSharedPtr AbstractProtocolAdapter::getNextMessage() {
    if(queue_received_messages.size()) {
        return DataPackSharedPtr();
    } else {
        DataPackSharedPtr result = queue_received_messages.front();
        queue_received_messages.pop();
        return result;
    }
}

bool AbstractProtocolAdapter::hasResponse() {
    return map_req_id_response.size()>0;
}

bool AbstractProtocolAdapter::hasResponseAvailable(uint32_t request_id) {
    return map_req_id_response.find(request_id) != map_req_id_response.end();
}

DataPackSharedPtr AbstractProtocolAdapter::retrieveRequestResponse(uint32_t request_id) {
    return DataPackSharedPtr();
}

ConnectionState AbstractProtocolAdapter::getConnectionState() const {
    return connection_status;
}

int AbstractProtocolAdapter::sendRawMessage(chaos::micro_unit_toolkit::data::DataPackUniquePtr& message) {
    return 0;
}

void AbstractProtocolAdapter::poll(int32_t milliseconds_wait) {
    
}
