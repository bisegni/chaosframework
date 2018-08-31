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

#include "RemoveScript.h"
#include <chaos/common/global.h>

#define INFO INFO_LOG(SaveScript)
#define DBG  DBG_LOG(SaveScript)
#define ERR  ERR_LOG(SaveScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

RemoveScript::RemoveScript():
AbstractApi("removeScript"){
}

RemoveScript::~RemoveScript() {
}

chaos::common::data::CDataWrapper *RemoveScript::execute(CDWUniquePtr) {
    int err = 0;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    //get scrip description
    ScriptBaseDescriptionSDWrapper script_bs_dw(api_data);
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)
    
    //call dataaccesso for insert new script and get the sequence value
    if((err = s_da->deleteScript(script_bs_dw().unique_id,
                                 script_bs_dw().name))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error updating script %1%[%2%]", %script_bs_dw.dataWrapped().name%script_bs_dw().unique_id));
    }
    return NULL;
}
