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

#ifndef __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_ExternalDriverUnitProxy_h
#define __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_ExternalDriverUnitProxy_h
#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                namespace raw_driver {
                    
                    class ExternalDriverHandlerWrapper;
                    //! Abstract base class for all unit proxy
                    class ExternalDriverUnitProxy:
                    public AbstractUnitProxy {
                        friend class ExternalDriverHandlerWrapper;
                    protected:
                        using AbstractUnitProxy::manageAuthenticationRequest;
                    public:
                        static const ProxyType proxy_type;
                        ExternalDriverUnitProxy(const std::string& _authorization_key,
                                           ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& connection_adapter);
                        virtual ~ExternalDriverUnitProxy();
                        
                        int sendAnswer(RemoteMessageUniquePtr& message,
                                       data::CDWUniquePtr& message_data);
                        int sendMessage(data::CDWUniquePtr& message_data);
                        using AbstractUnitProxy::sendMessage;
                        using AbstractUnitProxy::hasMoreMessage;
                        using AbstractUnitProxy::getNextMessage;
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__E639CBF_4E39_4FA4_B612_E6ED8C8410DE_ExternalDriverUnitProxy_h */
