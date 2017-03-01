/*
 *	CheckAgentHostedProcess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_B9D7FBA3_13EC_485E_AC0A_2C87D20EC897_CheckAgentHostedProcess_h
#define __CHAOSFramework_B9D7FBA3_13EC_485E_AC0A_2C87D20EC897_CheckAgentHostedProcess_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! Start a batch process cheecking into the agent
                class CheckAgentHostedProcess:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CheckAgentHostedProcess)
                protected:
                    API_PROXY_CD_DECLARATION(CheckAgentHostedProcess)
                public:
                    //! Start a batch process cheecking into the agent
                    /*!
                     The metadata server load all associated node and ask to the agent for their
                     alive status
                     \param agent_uid the agent that need to be asked to check his asosciated node
                     */
                    ApiProxyResult execute(const std::string& agent_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B9D7FBA3_13EC_485E_AC0A_2C87D20EC897_CheckAgentHostedProcess_h */
