/*
 *	ConnectionManager.h
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

#ifndef __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h
#define __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h

#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>

#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>

#include <string>
#include <map>

namespace chaos {
    namespace micro_unit_toolkit {
        //!forward decalration
        class ChaosMicroUnitToolkit;
        namespace connection {

            //! Entry point for a connection to chaos external unit server
            class ConnectionManager {
                friend class chaos::micro_unit_toolkit::ChaosMicroUnitToolkit;

                typedef ChaosSharedPtr< ObjectInstancerP1<unit_proxy::AbstractUnitProxy, ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>& > > UnitProxyInstancer;
                typedef std::map<ProxyType, UnitProxyInstancer > MapProxy;

                typedef ChaosSharedPtr< ObjectInstancerP2<protocol_adapter::AbstractProtocolAdapter, std::string, std::string> > ProtocolAdapterInstancer;
                typedef std::map<ProtocolType, ProtocolAdapterInstancer > MapProtocol;

                MapProxy    map_proxy;
                MapProtocol map_protocol;

                ConnectionManager();
                ~ConnectionManager();

                template<typename T>
                void registerProtocolAdapter() {
                    map_protocol.insert(std::make_pair(T::protocol_type, ProtocolAdapterInstancer(new TypedObjectInstancerP2<T,
                                                                                                  protocol_adapter::AbstractProtocolAdapter,
                                                                                                  std::string,
                                                                                                  std::string>())));
                }

                template<typename T>
                void registerUnitProxy() {
                    map_proxy.insert(std::make_pair(T::proxy_type, UnitProxyInstancer(new TypedObjectInstancerP1<T,
                                                                                      unit_proxy::AbstractUnitProxy,
                                                                                      ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>&>())));
                }


                ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter> getProtocolAdapter(ProtocolType type,
                                                                                             const std::string& endpoint,
                                                                                             const std::string& protocol_option);

                ChaosUniquePtr<unit_proxy::AbstractUnitProxy> getUnitProxy(ProxyType type,
                                                                           ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter> &protocol_adapter);
            };
        }
    }
}

#endif /* __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h */
