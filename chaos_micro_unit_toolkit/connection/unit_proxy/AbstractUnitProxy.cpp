/*
 *	AbstractUnitProxy.cpp
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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;

RemoteMessage::RemoteMessage(const data::DataPackSharedPtr& _message):
message(_message),
is_request((message->hasKey("request_id") && message->isInt32("request_id"))),
message_id(is_request?message->getInt32("request_id"):0){}

AbstractUnitProxy::AbstractUnitProxy(protocol_adapter::AbstractProtocolAdapter *_protocol_adapter):
protocol_adapter(_protocol_adapter){}

AbstractUnitProxy::~AbstractUnitProxy() {}

int AbstractUnitProxy::sendMessage(data::DataPackUniquePtr& message_data) {
    return protocol_adapter->sendMessage(message_data);
}

int AbstractUnitProxy::sendAnswer(RemoteMessageUniquePtr& message,
                                  data::DataPackUniquePtr& message_data) {
    if(message->is_request == false) return - 1;
    message_data->addInt32("request_id", message->is_request);
    return protocol_adapter->sendMessage(message_data);
}

bool AbstractUnitProxy::hasMoreMessage() {
    return protocol_adapter->hasMoreMessage();
}

RemoteMessageUniquePtr AbstractUnitProxy::getNextMessage() {
    if(protocol_adapter->hasMoreMessage() == false) return RemoteMessageUniquePtr();
    RemoteMessageUniquePtr next_message(new RemoteMessage(protocol_adapter->getNextMessage()));
    return next_message;
}
