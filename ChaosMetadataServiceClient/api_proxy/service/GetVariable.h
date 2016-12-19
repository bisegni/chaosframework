/*
 *	GetVariable.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h
#define __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                class GetVariable:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetVariable)
                protected:
                    API_PROXY_CD_DECLARATION(GetVariable)
                public:
                    
                    ApiProxyResult execute(const std::string& variable_name);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h */
