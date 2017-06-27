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

#include "SearchInstancesForScript.h"

#include <chaos_service_common/data/data.h>

#define INFO INFO_LOG(NewScript)
#define DBG  DBG_LOG(NewScript)
#define ERR  ERR_LOG(NewScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

SearchInstancesForScript::SearchInstancesForScript():
AbstractApi("searchInstancesForScript"){
}

SearchInstancesForScript::~SearchInstancesForScript() {
}

CDataWrapper *SearchInstancesForScript::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    //check for mandatory attributes
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "script_name", ERR, -2, "The script name is mandatory")
    const std::string script_name =  CDW_GET_VALUE_WITH_DEFAULT(api_data, "script_name", getStringValue,"");
    const std::string search_string = CDW_GET_VALUE_WITH_DEFAULT(api_data, "search_string", getStringValue, "");
    const uint64_t last_sequence_id = CDW_GET_VALUE_WITH_DEFAULT(api_data, "last_sequence_id", getUInt64Value, 0);
    const uint32_t page_lenght = CDW_GET_VALUE_WITH_DEFAULT(api_data, "page_lenght", getUInt32Value, 30);

    //get scrip description
    
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)

    
    //create sd wrapper for a std vector list with node description
    StdVectorSDWrapper<ScriptInstance, ScriptInstanceSDWrapper> ni_list_wrapper;
    
    if((err = s_da->searchScriptInstance(ni_list_wrapper.dataWrapped(),
                                         script_name,
                                         search_string,
                                         last_sequence_id,
                                         page_lenght))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error searching instance for script %1%",%script_name));
    }
    return ni_list_wrapper.serialize().release();
}
