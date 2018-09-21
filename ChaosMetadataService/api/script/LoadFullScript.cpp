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

#include "LoadFullScript.h"

#define INFO INFO_LOG(NewScript)
#define DBG  DBG_LOG(NewScript)
#define ERR  ERR_LOG(NewScript)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(LoadFullScript, "loadFullScript")

CDWUniquePtr LoadFullScript::execute(CDWUniquePtr api_data) {
    int err = 0;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    //get scrip description
    ScriptSDWrapper script_sdw(api_data.get());
    ScriptBaseDescriptionSDWrapper script_description_sdw(api_data.get());
    
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)
    
    //call data access for insert new script and get the sequence value
    if((err = s_da->loadScript(script_description_sdw.dataWrapped().unique_id,
                               script_description_sdw.dataWrapped().name,
                               script_sdw.dataWrapped(),
                               true))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error loading script %1%",%script_description_sdw.dataWrapped().name));
    }
    //return the script base description
    return script_sdw.serialize();
}
