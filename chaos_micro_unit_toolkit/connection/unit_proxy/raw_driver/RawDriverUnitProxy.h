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

#ifndef __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h
#define __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h
#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                namespace raw_driver {
                    //! Abstract base class for all unit proxy
                    class RawDriverUnitProxy:
                    public AbstractUnitProxy {
                        
                    public:
                        static const ProxyType proxy_type;
                        RawDriverUnitProxy(ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>& protocol_adapter);
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
}

#endif /* __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_RawDriverUnitProxy_h */
