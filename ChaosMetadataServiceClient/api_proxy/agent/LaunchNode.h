/*
 *	LaunchNode.h
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

#ifndef __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_LaunchNode_h
#define __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_LaunchNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! remove the association information for a node that belong to an agent
                class LaunchNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LaunchNode)
                protected:
                    API_PROXY_CD_DECLARATION(LaunchNode)
                public:
                    //! Start a node usign an agent
                    /*!
                     The mds automatically found the agent that contains the node and send request to that agent to start it
                     \param node_uid the node unique id to start
                     */
                    ApiProxyResult execute(const std::string& node_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_LaunchNode_h */
