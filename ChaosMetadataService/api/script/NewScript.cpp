/*
 *	NewScript.cpp
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

#include "NewScript.h"

#define INFO INFO_LOG(NewScript)
#define DBG  DBG_LOG(NewScript)
#define ERR  ERR_LOG(NewScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

NewScript::NewScript():
AbstractApi("newScript"){
}

NewScript::~NewScript() {
}

chaos::common::data::CDataWrapper *NewScript::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    //get scrip description
    ScriptHelper script_helper(api_data);
    
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)

    //call dataaccesso for insert new script and get the sequence value
    if((err = s_da->insertNewScript(script_helper.container()))) {
        LOG_AND_TROW(ERR, err, "Error creating new Script");
    }
    //return the script base description
    return script_helper.sdh.serialize().release();
}