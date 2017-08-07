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
            /*!
             Templated interface that give rule for the the instantiation of a class with one param constructor.
             */
            template <typename R, typename p1 >
            class ObjectInstancerP1 {
            public:
                virtual ~ObjectInstancerP1(){};
                virtual R* getInstance(p1 *_p1) = 0;
            };

            /*!
             Templated interface that give rule for the the instantiation of a class with two param constructor.
             */
            template <typename R, typename p1, typename p2 >
            class ObjectInstancerP2 {
            public:
                virtual ~ObjectInstancerP2(){};
                virtual R* getInstance(p1 _p1, p2 _p2) = 0;
            };


            /*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1>
            class TypedObjectInstancerP1:
            public ObjectInstancerP1<R, p1> {
            public:
                R* getInstance(p1 *_p1) {
                    return new T(_p1);
                }
            };

            /*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1, typename p2 >
            class TypedObjectInstancerP2:
            public ObjectInstancerP2<R, p1, p2> {
            public:
                R* getInstance(p1 _p1, p2 _p2) {
                    return new T(_p1, _p2);
                }
            };

            template<typename T>
            struct UnitConnection {
                const ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter> protocol_adapter;
                const ChaosSharedPtr<T> unit_proxy;

                UnitConnection(const ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter>& _protocol_adapter,
                               const ChaosSharedPtr<T> _unit_proxy):
                protocol_adapter(_protocol_adapter),
                unit_proxy(_unit_proxy){}
            };

            //! Entry point for a connection to chaos external unit server
            class ConnectionManager {
                friend class chaos::micro_unit_toolkit::ChaosMicroUnitToolkit;

                typedef ChaosSharedPtr< ObjectInstancerP1<unit_proxy::AbstractUnitProxy, protocol_adapter::AbstractProtocolAdapter> > UnitProxyInstancer;
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
                                                                                      protocol_adapter::AbstractProtocolAdapter>())));
                }

                template<typename UnitProxyClass>
                ChaosUniquePtr< UnitConnection<UnitProxyClass> > getNewUnitConnection(const ProtocolType protocol_type,
                                                                                      const std::string& protocol_endpoint,
                                                                                      const std::string& protocol_option) {
                    ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter> protocol_adater = getProtocolAdapter(protocol_type,
                                                                                                                   protocol_endpoint,
                                                                                                                   protocol_option);
                    ChaosSharedPtr<UnitProxyClass> unit_proxy(static_cast<UnitProxyClass*>(getUnitProxy(UnitProxyClass::proxy_type, protocol_adater.get()).release()));
                    ChaosUniquePtr< UnitConnection<UnitProxyClass> > uc(new UnitConnection<UnitProxyClass>(protocol_adater,
                                                                                                           unit_proxy));
                    return uc;
                }



                ChaosSharedPtr<protocol_adapter::AbstractProtocolAdapter> getProtocolAdapter(ProtocolType type,
                                                                                             const std::string& endpoint,
                                                                                             const std::string& protocol_option);

                ChaosUniquePtr<unit_proxy::AbstractUnitProxy> getUnitProxy(ProxyType type,
                                                                           protocol_adapter::AbstractProtocolAdapter *protocol_adapter);
            };
        }
    }
}

#endif /* __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h */
