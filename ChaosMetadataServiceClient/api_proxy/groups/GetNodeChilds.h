/*
 *	GetNodeChilds.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
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

#ifndef __CHAOSFramework__GetNodeChilds_h
#define __CHAOSFramework__GetNodeChilds_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace groups {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, NodeChildList);
                
                class GetNodeChildsHelper {
                    NodeChildList node_child_list;
                public:
                    GetNodeChildsHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetNodeChildsHelper();
                    size_t getNodeChildsListSize();
                    const NodeChildList& getNodeChildsList();
                };
                
                //! Get all child for a node
                class GetNodeChilds:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetNodeChilds)
                protected:
                    API_PROXY_CD_DECLARATION(GetNodeChilds)
                public:
                    //! return the child of a node identified byt domain and his path
                    /*!
                     \param node_domain is the domain of the node
                     \param node_path is the path that univocally identify the node within the domain
                     */
                    ApiProxyResult execute(const std::string& node_domain,
                                           const std::string& node_path = std::string());
                    
                    static std::auto_ptr<GetNodeChildsHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetNodeChilds_h */