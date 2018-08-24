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

#include <chaos_metadata_service_client/api_proxy/script/ManageScriptInstance.h>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(ManageScriptInstance, "script", "manageScriptInstance")

ApiProxyResult ManageScriptInstance::execute(const uint64_t script_seq,
                                             const std::string& script_name,
                                             const std::string& instance_name,
                                             const bool create) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addInt64Value("script_seq", script_seq);
    api_data->addStringValue("script_name", script_name);
    api_data->addStringValue("instance_name", instance_name);
    api_data->addBoolValue("create", create);
    return callApi(api_data);
}

ApiProxyResult ManageScriptInstance::execute(const uint64_t script_seq,
                                             const std::string& script_name,
                                             const ChaosStringVector& instance_names,
                                             const bool create) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addInt64Value("script_seq", script_seq);
    api_data->addStringValue("script_name", script_name);
    for(ChaosStringVectorConstIterator it = instance_names.begin(),
        end = instance_names.end();
        it != end;
        it++) {
        api_data->appendStringToArray(*it);
    }
    api_data->finalizeArrayForKey("instance_name");
    api_data->addBoolValue("create", create);
    return callApi(api_data);
}
