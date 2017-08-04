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
#include <chaos_micro_unit_toolkit/connection/unit_proxy/RawDriverUnitProxy.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

ConnectionManager::ConnectionManager(const std::string& endpoint,
                                     const ProtocolType protocol_type) {
    registerUnitProxy<RawDriverUnitProxy>();
    registerProtocolAdapter<http::HTTPProtocolAdapter>();
}

ConnectionManager::~ConnectionManager() {
    
}

ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter> ConnectionManager::getProtocolAdapter(ProtocolType type,
                                                                                                const std::string& endpoint,
                                                                                                const std::string& protocol_option) {
    if(map_protocol.count(type) == 0){ return ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter>();}
    return ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter>(map_protocol[type]->getInstance(endpoint,
                                                                                                     protocol_option));
}

ChaosUniquePtr<unit_proxy::AbstractUnitProxy> ConnectionManager::getUnitProxy(ProxyType type,
                                                                              protocol_adapter::AbstractProtocolAdapter& protocol_adapter){
    if(map_proxy.count(type) == 0){ return ChaosUniquePtr<unit_proxy::AbstractUnitProxy>();}
    return ChaosUniquePtr<unit_proxy::AbstractUnitProxy>(map_proxy[type]->getInstance(protocol_adapter));
}
