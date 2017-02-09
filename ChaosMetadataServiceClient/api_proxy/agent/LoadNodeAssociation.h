/*
 *	LoadNodeAssociation.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 09/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h
#define __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! load the association information for a not belong to an agent
                class LoadNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LoadNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(LoadNodeAssociation)
                public:
                    //! load the association information for a not belong to an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param associated_node_uid the node associated to the agent
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           const std::string& associated_node_uid);
                    
                    static void deserialize(chaos::common::data::CDataWrapper *api_result,
                                            chaos::service_common::data::agent::AgentAssociation& node_association);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h */
