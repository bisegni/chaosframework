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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/ExternalDriverUnitProxy.h>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;

#define MESSAGE                     "msg"
#define REQUEST_IDENTIFICATION      "req_id"

const ProxyType ExternalDriverUnitProxy::proxy_type = ProxyTypeRawDriver;

ExternalDriverUnitProxy::ExternalDriverUnitProxy(const std::string& _authorization_key,
                                                 ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& connection_adapter):
AbstractUnitProxy(_authorization_key,
connection_adapter){}

ExternalDriverUnitProxy::~ExternalDriverUnitProxy() {}

int ExternalDriverUnitProxy::sendMessage(CDWUniquePtr& message_data) {
    CDWUniquePtr message(new DataPack());
    message->addCDWValue(MESSAGE, *message_data);
    return AbstractUnitProxy::sendMessage(message);
}

int ExternalDriverUnitProxy::sendAnswer(RemoteMessageUniquePtr& message,
                                        CDWUniquePtr& message_data) {
    
    if(message->is_request == false) return - 1;
    CDWUniquePtr answer(new DataPack());
    answer->addInt32Value(REQUEST_IDENTIFICATION, message->message_id);
    answer->addCDWValue(MESSAGE, *message_data);
    return AbstractUnitProxy::sendMessage(answer);
}
