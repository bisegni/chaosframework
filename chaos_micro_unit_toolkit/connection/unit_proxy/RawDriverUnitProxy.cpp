/*
 *	RawDriverUnitProxy.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 04/08/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/RawDriverUnitProxy.h>

using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;

#define AUTHORIZATION_KEY        "authorization_key"
#define AUTHORIZATION_STATE      "authorization_state"
#define MESSAGE                 "message"
#define REQUEST_IDENTIFICATION  "request_id"

const ProxyType RawDriverUnitProxy::proxy_type = ProxyTypeRawDriver;

RawDriverUnitProxy::RawDriverUnitProxy(protocol_adapter::AbstractProtocolAdapter *protocol_adapter):
AbstractUnitProxy(protocol_adapter){}

RawDriverUnitProxy::~RawDriverUnitProxy() {}

void RawDriverUnitProxy::authorization(const std::string& authorization_key) {
    data::DataPackUniquePtr message(new data::DataPack());
    message->addString(AUTHORIZATION_KEY, authorization_key);
    sendMessage(message);
}
