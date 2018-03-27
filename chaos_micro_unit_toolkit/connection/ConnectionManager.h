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

#ifndef __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h
#define __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h

#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>

#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
#include <chaos_micro_unit_toolkit/connection/connection_adapter/AbstractConnectionAdapter.h>

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
                
                typedef ChaosSharedPtr< ObjectInstancerP2<unit_proxy::AbstractUnitProxy, const std::string&, ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& > > UnitProxyInstancer;
                typedef std::map<ProxyType, UnitProxyInstancer > MapProxy;
                
                typedef ChaosSharedPtr< ObjectInstancerP2<connection_adapter::AbstractConnectionAdapter, std::string, std::string> > ProtocolAdapterInstancer;
                typedef std::map<ConnectionType, ProtocolAdapterInstancer > MapProtocol;
                
                MapProxy    map_proxy;
                MapProtocol map_protocol;
                
                ConnectionManager();
                ~ConnectionManager();
                
                template<typename T>
                void registerProtocolAdapter() {
                    map_protocol.insert(std::make_pair(T::connection_type, ProtocolAdapterInstancer(new TypedObjectInstancerP2<T,
                                                                                                    connection_adapter::AbstractConnectionAdapter,
                                                                                                    std::string,
                                                                                                    std::string>())));
                }
                
                template<typename T>
                void registerUnitProxy() {
                    map_proxy.insert(std::make_pair(T::proxy_type, UnitProxyInstancer(new TypedObjectInstancerP2<T,
                                                                                      unit_proxy::AbstractUnitProxy,
                                                                                      const std::string&,
                                                                                      ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>&>())));
                }
                
                
                ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter> getProtocolAdapter(ConnectionType type,
                                                                                                 const std::string& endpoint,
                                                                                                 const std::string& protocol_option);
                
                ChaosUniquePtr<unit_proxy::AbstractUnitProxy> getUnitProxy(ProxyType type,
                                                                           const std::string& authorization_key,
                                                                           ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter> &connection_adapter);
            };
        }
    }
}
#endif /* __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h */
