/*
 *	SearchScript.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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
SearchScript::SearchScript():
AbstractApi("searchScript"){}

SearchScript::~SearchScript() {}

chaos::common::data::CDataWrapper *SearchScript::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    std::unique_ptr<CDataWrapper> result;
    
    ScriptBaseDescriptionListWrapper found_page_element;
   
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
    return found_page_element.serialize().release();
}
