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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
#include <cassert>
#include <iostream>
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::connection_adapter;

RemoteMessage::RemoteMessage(const CDWShrdPtr& _remote_message):
remote_message(_remote_message),
is_request((remote_message->hasKey(REQUEST_KEY) && remote_message->isInt32Value(REQUEST_KEY))),
message_id(is_request?remote_message->getInt32Value(REQUEST_KEY):0){
    if(remote_message->hasKey(MSG_KEY) &&
       remote_message->isCDWValue(MSG_KEY)) {
        message.reset(remote_message->getCDWValue(MSG_KEY).release());
    }
}

bool RemoteMessage::isError() const {
    return message.get() && message->hasKey(ERR_CODE);
}

int32_t RemoteMessage::getErrorCode() const {
    return message.get()?message->getInt32Value(ERR_CODE):0;
}

std::string RemoteMessage::getErrorMessage() const {
    return message.get()?message->getStringValue(ERR_DOM):"";
}

std::string RemoteMessage::getErrorDomain() const {
    return message.get()?message->getStringValue("error_domain"):"";
}

AbstractUnitProxy::AbstractUnitProxy(const std::string& _authorization_key,
                                     ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& _protocol_adapter):
connection_adapter(MOVE(_protocol_adapter)),
unit_state(UnitStateUnknown),
authorization_key(_authorization_key){assert(connection_adapter.get());}

AbstractUnitProxy::~AbstractUnitProxy() {
    std::cout <<"exit";
}

int AbstractUnitProxy::sendMessage(CDWUniquePtr& message_data) {
    return connection_adapter->sendMessage(message_data);
}

bool AbstractUnitProxy::hasMoreMessage() {
    return connection_adapter->hasMoreMessage();
}

RemoteMessageUniquePtr AbstractUnitProxy::getNextMessage() {
    if(connection_adapter->hasMoreMessage() == false) return RemoteMessageUniquePtr();
    data::CDWShrdPtr next_msg = connection_adapter->getNextMessage();
    RemoteMessageUniquePtr next_message(new RemoteMessage(next_msg));
    return next_message;
}

const UnitState& AbstractUnitProxy::getUnitState() const {
    return unit_state;
}

int AbstractUnitProxy::connect() {
    return connection_adapter->connect();
}

void AbstractUnitProxy::poll(int32_t milliseconds_wait) {
    connection_adapter->poll(milliseconds_wait);
}

int AbstractUnitProxy::close() {
    return connection_adapter->close();
}

const ConnectionState& AbstractUnitProxy::getConnectionState() const {
    return connection_adapter->getConnectionState();
}

void AbstractUnitProxy::resetAuthorization() {
    unit_state = UnitStateUnknown;
}
void AbstractUnitProxy::manageAuthenticationRequest() {
    bool result = false;
    if(authorization_key.size()) {
        if((result = hasMoreMessage())) {
            //!check authentication state
            RemoteMessageUniquePtr result = getNextMessage();
            if(result->message->hasKey(AUTHORIZATION_KEY) ||
               result->message->isStringValue(AUTHORIZATION_KEY)) {
                const std::string remote_authentication_key = result->message->getStringValue(AUTHORIZATION_KEY);
                if(remote_authentication_key.compare(authorization_key) == 0) {
                    unit_state = UnitStateAuthenticated;
                } else {
                    unit_state = UnitStateNotAuthenticated;
                }
            }
        }
    } else {
        unit_state = UnitStateAuthenticated;
    }
}
