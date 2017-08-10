/*
 *	RawDriverHandlerWrapper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 10/08/2017 INFN, National Institute of Nuclear Physics
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
                        const AuthorizationState& auth_state;
                        bool authorized;
                    protected:
                        int unitEventLoop();
                        int manageRemoteMessage();
                    public:
                        RawDriverHandlerWrapper(UnitProxyHandler handler,
                                                void *user_data,
                                                const std::string& _authorization_key,
                                                ChaosUniquePtr<RawDriverUnitProxy>& _u_proxy);
                        ~RawDriverHandlerWrapper();
                    };
                    
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_RawDriverHandlerWrapper_h */
