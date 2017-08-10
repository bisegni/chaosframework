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
#include <cassert>
#include <iostream>
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

RemoteMessage::RemoteMessage(const DataPackSharedPtr& _message):
message(_message),
is_request((message->hasKey("request_id") && message->isInt32("request_id"))),
message_id(is_request?message->getInt32("request_id"):0){
    if(is_request &&
       message->hasKey("message") &&
       message->isDataPack("message")) {
        request_message.reset(message->getDataPack("message").release());
    }
}

bool RemoteMessage::isError() const {
    return message.get() && message->hasKey("error_code");
}

int32_t RemoteMessage::getErrorCode() const {
    return message.get()?message->getInt32("error_code"):0;
}

std::string RemoteMessage::getErrorMessage() const {
    return message.get()?message->getString("error_message"):"";
}

std::string RemoteMessage::getErrorDomain() const {
    return message.get()?message->getString("error_domain"):"";
}

AbstractUnitProxy::AbstractUnitProxy(ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>& _protocol_adapter):
protocol_adapter(ChaosMoveOperator(_protocol_adapter)),
authorization_state(AuthorizationStateUnknown){assert(protocol_adapter.get());}

AbstractUnitProxy::~AbstractUnitProxy() {
    std::cout <<"exit";
}

int AbstractUnitProxy::sendMessage(DataPackUniquePtr& message_data) {
    return protocol_adapter->sendMessage(message_data);
}

int AbstractUnitProxy::sendAnswer(RemoteMessageUniquePtr& message,
                                  DataPackUniquePtr& message_data) {
    if(message->is_request == false) return - 1;
    DataPackUniquePtr answer(new DataPack());
    answer->addInt32("request_id", message->message_id);
    answer->addDataPack("message", *message_data);
    return protocol_adapter->sendMessage(answer);
}

bool AbstractUnitProxy::hasMoreMessage() {
    return protocol_adapter->hasMoreMessage();
}

RemoteMessageUniquePtr AbstractUnitProxy::getNextMessage() {
    if(protocol_adapter->hasMoreMessage() == false) return RemoteMessageUniquePtr();
    RemoteMessageUniquePtr next_message(new RemoteMessage(protocol_adapter->getNextMessage()));
    return next_message;
}

const AuthorizationState& AbstractUnitProxy::getAuthorizationState() const {
    return authorization_state;
}

int AbstractUnitProxy::connect() {
    return protocol_adapter->connect();
}

void AbstractUnitProxy::poll(int32_t milliseconds_wait) {
    protocol_adapter->poll(milliseconds_wait);
}

int AbstractUnitProxy::close() {
    return protocol_adapter->close();
}

const ConnectionState& AbstractUnitProxy::getConnectionState() const {
    return protocol_adapter->getConnectionState();
}

void AbstractUnitProxy::resetAuthorization() {
    authorization_state = AuthorizationStateUnknown;
}
