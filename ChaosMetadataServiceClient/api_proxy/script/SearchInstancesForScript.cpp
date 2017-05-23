/*
 *	SearchInstancesForScript.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/06/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/script/SearchInstancesForScript.h>

#include <chaos/common/data/structured/Lists.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(SearchInstancesForScript, "script", "searchInstancesForScript")

ApiProxyResult SearchInstancesForScript::execute(const std::string& script_name,
                                                 const std::string& search_string,
                                                 const uint64_t start_sequence_id,
                                                 const uint32_t page_lenght) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue("script_name", script_name);
    api_data->addStringValue("search_string", search_string);
    api_data->addInt64Value("last_sequence_id", start_sequence_id);
    api_data->addInt32Value("page_lenght", page_lenght);
    return callApi(api_data.release());
}

void SearchInstancesForScript::deserialize(chaos::common::data::CDataWrapper& serialization,
                                           std::vector<ScriptInstance>& instances_found) {
    StdVectorSDWrapper<ScriptInstance, ScriptInstanceSDWrapper> ni_list_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(std::vector<ScriptInstance>, instances_found));
    instances_found.clear();
    ni_list_wrapper.deserialize(&serialization);
}
