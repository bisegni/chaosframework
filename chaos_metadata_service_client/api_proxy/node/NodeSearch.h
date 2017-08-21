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
#ifndef __CHAOSFramework__NodeSearch__
#define __CHAOSFramework__NodeSearch__

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

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
                                           bool alive_only = false,
                                           unsigned int last_node_sequence_id = 0,
                                           unsigned int page_length = 30);
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__NodeSearch__) */
