/*
 *	SaveNodeAssociation.h
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

#ifndef __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h
#define __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! save the association information fora not into an agent
                class SaveNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SaveNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(SaveNodeAssociation)
                public:
                    //! Save a node association into an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param node_association the node assocaition description
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           chaos::service_common::data::agent::AgentAssociation& node_association);
                    
                    //! Save multiple node association into an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param multiple_node_association a vector of node assocaition description
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           chaos::service_common::data::agent::VectorAgentAssociation& multiple_node_association);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h */
