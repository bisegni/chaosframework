/*
 *	LoadAgentDescription.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 13/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_C694B943_5C76_4F78_A505_E57E6EACD39D_LoadAgentDescription_h
#define __CHAOSFramework_C694B943_5C76_4F78_A505_E57E6EACD39D_LoadAgentDescription_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! remove the association information for a node that belong to an agent
                class LoadAgentDescription:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LoadAgentDescription)
                protected:
                    API_PROXY_CD_DECLARATION(LoadAgentDescription)
                public:
                    //! Start a node usign an agent
                    /*!
                     The mds automatically found the agent that contains the node and send request to that agent to start it
                     \param node_uid the node unique id to start
                     */
                    ApiProxyResult execute(const std::string& node_uid,
                                           const bool load_related_data = false);
                    static void deserialize(chaos::common::data::CDataWrapper *api_result,
                                            chaos::service_common::data::agent::AgentInstance& agent_instance);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C694B943_5C76_4F78_A505_E57E6EACD39D_LoadAgentDescription_h */
