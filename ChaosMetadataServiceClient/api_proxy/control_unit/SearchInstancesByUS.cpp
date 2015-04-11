/*
 *	SearchInstancesByUS.cpp
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
#include <ChaosMetadataServiceClient/api_proxy/control_unit/SearchInstancesByUS.h>

using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

API_PROXY_CD_DEFINITION(SearchInstancesByUS,
                        "control_unit",
                        "searchInstancesByUS")

/*!

 */
ApiProxyResult SearchInstancesByUS::execute(uint32_t last_node_sequence_id,
                                            uint32_t result_page_length) {
    chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
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