/*
 *	SearchInstancesByUS.h
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

#ifndef __CHAOSFramework__SearchInstancesByUS__
#define __CHAOSFramework__SearchInstancesByUS__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                class SearchInstancesByUS:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SearchInstancesByUS)
                protected:
                        //! default constructor
                    API_PROXY_CD_DECLARATION(SearchInstancesByUS)

                public:
                        //the unit server that host the instances
                    std::string unit_server_uid;
                        //the implementation that whe want to be returned
                    std::vector<std::string> control_unit_implementation;

                    ApiProxyResult execute(uint32_t last_node_sequence_id = 0,
                                           uint32_t result_page_length = 100);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SearchInstancesByUS__) */
