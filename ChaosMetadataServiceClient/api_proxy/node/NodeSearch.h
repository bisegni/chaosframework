/*
 *	NodeSearch.h
 *	!CHOAS
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
#ifndef __CHAOSFramework__NodeSearch__
#define __CHAOSFramework__NodeSearch__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {

                class NodeSearch:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(NodeSearch)
                protected:
                        //! default constructor
                    API_PROXY_CD_DECLARATION(NodeSearch)

                public:

                    /*!
                     Return the description of the node
                     \param unique_id_filter is the string that will be tokenized and every token will 
                     work in or as the example: "one two tree" -> "%one%" or "%two%" or "%tree%"
                     */
                    ApiProxyResult execute(const std::string& unique_id_filter,
                                           unsigned int node_type_filter,
                                           unsigned int last_node_sequence_id = 0,
                                           unsigned int page_length = 30);
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__NodeSearch__) */
