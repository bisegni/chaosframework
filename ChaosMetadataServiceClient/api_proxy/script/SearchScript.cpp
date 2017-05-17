/*
 *	SearchScript.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/script/SearchScript.h>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(SearchScript, "script", "searchScript")

ApiProxyResult SearchScript::execute(const std::string& search_string,
                                     const uint64_t start_sequence_id,
                                     const uint32_t page_lenght) {
    ChaosUniquePtr<CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue("search_string", search_string);
    api_data->addInt64Value("last_sequence_id", start_sequence_id);
    api_data->addInt32Value("page_lenght", page_lenght);
    return callApi(api_data.release());
}
