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
#include <chaos_metadata_service_client/api_proxy/control_unit/SearchInstancesByUS.h>

using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

API_PROXY_CD_DEFINITION(SearchInstancesByUS,
                        "control_unit",
                        "searchInstancesByUS")


ApiProxyResult SearchInstancesByUS::execute(std::string unit_server_uid,
                                            std::vector<std::string> control_unit_implementation,
                                            uint32_t last_node_sequence_id,
                                            uint32_t result_page_length) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(chaos::NodeDefinitionKey::NODE_PARENT, unit_server_uid);
    if(control_unit_implementation.size()) {
        for(std::vector<std::string>::iterator it = control_unit_implementation.begin();
            it != control_unit_implementation.end();
            it++) {
            message->appendStringToArray(*it);
        }
        message->finalizeArrayForKey("control_unit_implementation");
    }
    message->addInt32Value("last_node_sequence_id", last_node_sequence_id);
    message->addInt32Value("result_page_length", result_page_length);
    return callApi(message);
}
