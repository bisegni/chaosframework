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
