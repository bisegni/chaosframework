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

#ifndef __CHAOSFramework__SearchInstancesByUS__
#define __CHAOSFramework__SearchInstancesByUS__

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

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
                    ApiProxyResult execute(std::string unit_server_uid,
                                           std::vector<std::string> control_unit_implementation = std::vector<std::string>(),
                                           uint32_t last_node_sequence_id = 0,
                                           uint32_t result_page_length = 100);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SearchInstancesByUS__) */
