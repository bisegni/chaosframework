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

#include <chaos_metadata_service_client/api_proxy/script/UpdateBindType.h>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/structured/Lists.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(UpdateBindType, "script", "updateBindType")

ApiProxyResult UpdateBindType::execute(const ScriptBaseDescription& sbd,
                                       ScriptInstance& scr_inst) {
    ScriptBaseDescriptionSDWrapper swd(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(ScriptBaseDescription, const_cast<ScriptBaseDescription&>(sbd)));
    ScriptInstanceSDWrapper iwd(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(ScriptInstance, scr_inst));
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addCSDataValue("script_base_description", *swd.serialize());
    api_data->addCSDataValue("script_instance", *iwd.serialize());
    return callApi(api_data.release());
}


ApiProxyResult UpdateBindType::execute(const chaos::service_common::data::script::ScriptBaseDescription& sbd,
                                       std::vector<chaos::service_common::data::script::ScriptInstance>& scr_inst_vec) {
    ScriptBaseDescriptionSDWrapper swd(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(ScriptBaseDescription, const_cast<ScriptBaseDescription&>(sbd)));
    StdVectorSDWrapper<ScriptInstance, ScriptInstanceSDWrapper> ni_list_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(std::vector<ScriptInstance>, scr_inst_vec));
    ni_list_wrapper.serialization_key = "vector_script_instance";
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addCSDataValue("script_base_description", *swd.serialize());
    api_data->appendAllElement(*ni_list_wrapper.serialize());
    return callApi(api_data.release());
}
