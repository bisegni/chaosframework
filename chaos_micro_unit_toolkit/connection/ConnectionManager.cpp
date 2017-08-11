/*
 *	ConnectionManagerManager.cpp
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

#include <chaos_micro_unit_toolkit/connection/ConnectionManager.h>

//protocols
#include <chaos_micro_unit_toolkit/connection/protocol_adapter/http/HTTPProtocolAdapter.h>

//unit
#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/RawDriverUnitProxy.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

ConnectionManager::ConnectionManager() {
    registerUnitProxy<raw_driver::RawDriverUnitProxy>();
    registerProtocolAdapter<http::HTTPProtocolAdapter>();
}

ConnectionManager::~ConnectionManager() {}

ChaosUniquePtr<AbstractProtocolAdapter> ConnectionManager::getProtocolAdapter(ProtocolType type,
                                                                              const std::string& endpoint,
                                                                              const std::string& protocol_option) {
    if(map_protocol.count(type) == 0){ return ChaosUniquePtr<AbstractProtocolAdapter>();}
    ProtocolAdapterInstancer is = map_protocol[type];
    return ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>(is->getInstance(endpoint,
                                                                                     protocol_option));
}

ChaosUniquePtr<AbstractUnitProxy> ConnectionManager::getUnitProxy(ProxyType type,
                                                                  ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>& protocol_adapter){
    if(map_proxy.count(type) == 0){ return ChaosUniquePtr<unit_proxy::AbstractUnitProxy>();}
    UnitProxyInstancer is = map_proxy[type];
    return ChaosUniquePtr<unit_proxy::AbstractUnitProxy>(is->getInstance(protocol_adapter));
}
