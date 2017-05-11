/*
 *	SetManagementConfiguration.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/05/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AFFA027_1087_40A0_8EA4_95E22A477C86_SetManagementConfiguration_h
#define __CHAOSFramework__AFFA027_1087_40A0_8EA4_95E22A477C86_SetManagementConfiguration_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! Set the managment configuration for agent
                class SetManagementConfiguration:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SetManagementConfiguration)
                protected:
                    API_PROXY_CD_DECLARATION(SetManagementConfiguration)
                public:
                    ApiProxyResult execute(chaos::service_common::data::agent::AgentManagementSetting& agent_man_setting);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__AFFA027_1087_40A0_8EA4_95E22A477C86_SetManagementConfiguration_h */
