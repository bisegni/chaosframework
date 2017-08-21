/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__GetNodeChilds_h
#define __CHAOSFramework__GetNodeChilds_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

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
                    
                    static ChaosUniquePtr<GetNodeChildsHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetNodeChilds_h */
