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

#include <chaos_metadata_service_client/api_proxy/script/SaveScript.h>

using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(SaveScript, "script", "saveScript")

ApiProxyResult SaveScript::execute(const Script& script_to_insert,
                                   const bool& import) {
    ScriptSDWrapper sh(script_to_insert);
    ChaosUniquePtr<common::data::CDataWrapper> data_pack = sh.serialize();
    if(import){data_pack->addBoolValue("import", import);}
    return callApi(data_pack.release());
}
