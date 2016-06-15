/*
 *	AddScriptInstance.cpp
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

#include "ManageScriptInstance.h"

#include <chaos_service_common/data/data.h>

#define INFO INFO_LOG(NewScript)
#define DBG  DBG_LOG(NewScript)
#define ERR  ERR_LOG(NewScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::data::general;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

ManageScriptInstance::ManageScriptInstance():
AbstractApi("ManageScriptInstance"){
}

ManageScriptInstance::~ManageScriptInstance() {
}

chaos::common::data::CDataWrapper *ManageScriptInstance::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    //check for mandatory attributes
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "script_name", ERR, -2, "The script name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "instance_name", ERR, -3, "The instance name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "create", ERR, -4, "The create key is mandatory")
    const std::string script_name =  CDW_GET_VALUE_WITH_DEFAULT(api_data, "script_name", getStringValue,"");
    const std::string instance_name = CDW_GET_VALUE_WITH_DEFAULT(api_data, "instance_name", getStringValue, "");
    const bool create = CDW_GET_VALUE_WITH_DEFAULT(api_data, "create", getBoolValue, false);
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)
    
    if(create) {
        if((err = s_da->addScriptInstance(script_name,
                                          instance_name))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating instance %1% for script %2%",%instance_name%script_name));
        }
    } else {
        if((err = s_da->removeScriptInstance(script_name,
                                             instance_name))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error removing instance %1% for script %2%",%instance_name%script_name));
        }
    }
    return NULL;
}
