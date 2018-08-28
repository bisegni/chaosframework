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

#include <chaos_metadata_service_client/api_proxy/script/SearchScript.h>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(SearchScript, "script", "searchScript")

ApiProxyResult SearchScript::execute(const std::string& search_string,
                                     const uint64_t start_sequence_id,
                                     const uint32_t page_lenght) {
    CDWUniquePtr api_data(new CDataWrapper());
    api_data->addStringValue("search_string", search_string);
    api_data->addInt64Value("last_sequence_id", start_sequence_id);
    api_data->addInt32Value("page_lenght", page_lenght);
    return callApi(api_data);
}
