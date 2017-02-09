/*
 *	ListNodeForAgent.h
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

#ifndef __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h
#define __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! return all node associated to an agent
                class ListNodeForAgent:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(ListNodeForAgent)
                protected:
                    API_PROXY_CD_DECLARATION(ListNodeForAgent)
                public:
                    //! find all node associated to the agent
                    /*!
                     \param agent_uid the agent unique id
                     */
                    ApiProxyResult execute(const std::string& agent_uid);
                    
                    static void deserialize(chaos::common::data::CDataWrapper *api_result,
                                            ChaosStringVector& associated_nodes);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h */
