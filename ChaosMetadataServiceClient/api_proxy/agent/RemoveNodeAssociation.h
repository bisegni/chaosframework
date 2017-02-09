/*
 *	RemoveNodeAssociation.h
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

#ifndef __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h
#define __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! remove the association information for a node that belong to an agent
                class RemoveNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(RemoveNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(RemoveNodeAssociation)
                public:
                    //! remove the association information for a node that belong to an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param associated_node_uid the nodeto remove from the agent
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           const std::string& associated_node_uid);;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h */
