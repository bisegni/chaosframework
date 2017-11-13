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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/RawDriverUnitProxy.h>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;

#define AUTHORIZATION_KEY           "authorization_key"
#define AUTHORIZATION_STATE         "authorization_state"
#define MESSAGE                     "msg"
#define REQUEST_IDENTIFICATION      "req_id"

const ProxyType RawDriverUnitProxy::proxy_type = ProxyTypeRawDriver;

RawDriverUnitProxy::RawDriverUnitProxy(ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& connection_adapter):
AbstractUnitProxy(connection_adapter){}

RawDriverUnitProxy::~RawDriverUnitProxy() {}

void RawDriverUnitProxy::authorization(const std::string& authorization_key) {
    authorization_state = AuthorizationStateRequested;
    data::DataPackUniquePtr message(new data::DataPack());
    message->addString(AUTHORIZATION_KEY, authorization_key);
    AbstractUnitProxy::sendMessage(message);
}

bool RawDriverUnitProxy::manageAutorizationPhase() {
    bool result = false;
    if((result = hasMoreMessage())) {
        //!check authentication state
        RemoteMessageUniquePtr result = getNextMessage();
        if(result->message->hasKey(AUTHORIZATION_STATE) ||
           result->message->isBool(AUTHORIZATION_STATE)) {
            authorization_state = (AuthorizationState)result->message->getBool(AUTHORIZATION_STATE);
        }
    }
    return result;
}

int RawDriverUnitProxy::sendMessage(DataPackUniquePtr& message_data) {
    DataPackUniquePtr message(new DataPack());
    message->addDataPack(MESSAGE, *message_data);
    return AbstractUnitProxy::sendMessage(message);
}

int RawDriverUnitProxy::sendAnswer(RemoteMessageUniquePtr& message,
                                  DataPackUniquePtr& message_data) {
    if(message->is_request == false) return - 1;
    DataPackUniquePtr answer(new DataPack());
    answer->addInt32(REQUEST_IDENTIFICATION, message->message_id);
    answer->addDataPack(MESSAGE, *message_data);
    return AbstractUnitProxy::sendMessage(answer);
}
