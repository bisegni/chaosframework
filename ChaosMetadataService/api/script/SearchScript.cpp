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

#include "SearchScript.h"

#define INFO INFO_LOG(SearchScript)
#define DBG  DBG_LOG(SearchScript)
#define ERR  ERR_LOG(SearchScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(SearchScript, "searchScript")
CDWUniquePtr SearchScript::execute(CDWUniquePtr api_data) {
    int err = 0;
    CDWUniquePtr found_page_element(new CDataWrapper());

    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    const std::string search_string = CDW_GET_VALUE_WITH_DEFAULT(api_data, "search_string", getStringValue, "");
    const uint64_t last_sequence_id = CDW_GET_VALUE_WITH_DEFAULT(api_data, "last_sequence_id", getUInt64Value, 0);
    const uint32_t page_lenght = CDW_GET_VALUE_WITH_DEFAULT(api_data, "page_lenght", getUInt32Value, 30);
    
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)
    
    //call dataaccesso for insert new script and get the sequence value
    if((err = s_da->searchScript(found_page_element,
                                 search_string,
                                 last_sequence_id,
                                 page_lenght))) {
        LOG_AND_TROW(ERR, err, "Error searching script");
    }
    //we don't have had error so we will have fill the list wrapper of the result page
    return found_page_element;
}
