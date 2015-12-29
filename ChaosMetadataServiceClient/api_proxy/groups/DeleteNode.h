/*
 *	DeleteNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DeleteNode_h
#define __CHAOSFramework__DeleteNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace groups {;
                
                
                //! Delete node and all his child
                class DeleteNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(DeleteNode)
                protected:
                    API_PROXY_CD_DECLARATION(DeleteNode)
                public:
                    
                    ApiProxyResult execute(const std::string& node_domain,
                                           const std::string& node_name,
                                           const std::string& node_parent_path);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__DeleteNode_h */
