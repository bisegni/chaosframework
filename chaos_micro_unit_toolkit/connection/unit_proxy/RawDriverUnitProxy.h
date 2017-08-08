/*
 *	RawDriverUnitProxy.h
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

#ifndef __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h
#define __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h
#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {

                //! Abstract base class for all unit proxy
                class RawDriverUnitProxy:
                public AbstractUnitProxy {

                public:
                    static const ProxyType proxy_type;
                    RawDriverUnitProxy(protocol_adapter::AbstractProtocolAdapter& protocol_adapter);
                    virtual ~RawDriverUnitProxy();

                    void authorization(const std::string& authorization_key);
                    bool manageAutorizationPhase();
                    using AbstractUnitProxy::sendMessage;
                    using AbstractUnitProxy::hasMoreMessage;
                    using AbstractUnitProxy::getNextMessage;
                    using AbstractUnitProxy::sendAnswer;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h */
