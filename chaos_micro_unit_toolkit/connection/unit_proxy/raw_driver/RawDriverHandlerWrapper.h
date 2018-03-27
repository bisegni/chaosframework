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

#ifndef __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_RawDriverHandlerWrapper_h
#define __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_RawDriverHandlerWrapper_h

#include <chaos_micro_unit_toolkit/connection/unit_proxy/UnitProxyHandlerWrapper.h>
#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/RawDriverUnitProxy.h>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                namespace raw_driver {
                    
                    class RawDriverHandlerWrapper:
                    public UnitProxyHandlerWrapper {
                        std::string authorization_key;
                        const UnitState& unit_state;
                    protected:
                        int unitEventLoop();
                        int manageRemoteMessage();
                    public:
                        RawDriverHandlerWrapper(UnitProxyHandler handler,
                                                void *user_data,
                                                const std::string& _authorization_key,
                                                ChaosUniquePtr<RawDriverUnitProxy>& _u_proxy);
                        ~RawDriverHandlerWrapper();
                        
                        //! send spontaneus message to the remote raw driver layer
                        int sendMessage(data::CDWUniquePtr& message_data);
                    };
                    
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_RawDriverHandlerWrapper_h */
