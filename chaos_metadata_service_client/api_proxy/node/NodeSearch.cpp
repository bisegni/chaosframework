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

#include <chaos_metadata_service_client/api_proxy/node/NodeSearch.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

API_PROXY_CD_DEFINITION(NodeSearch, "system", "nodeSearch")


ApiProxyResult NodeSearch::execute(const std::string& unique_id_filter,
                                   unsigned int node_type_filter,
                                   bool alive_only,
                                   unsigned int last_node_sequence_id,
                                   unsigned int page_length) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue("unique_id_filter", unique_id_filter);
    message->addInt32Value("node_type_filter", node_type_filter);
    message->addInt32Value("last_node_sequence_id", last_node_sequence_id);
    message->addBoolValue("alive_only", alive_only);
    message->addInt32Value("result_page_length", page_length);
    return callApi(message);
}
