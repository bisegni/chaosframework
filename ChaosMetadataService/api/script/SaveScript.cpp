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

#include "SaveScript.h"
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

CHAOS_MDS_DEFINE_API_CLASS_CD(SaveScript, "saveScript")

CDWUniquePtr SaveScript::execute(CDWUniquePtr api_data) {
    int err = 0;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    
    bool import = CDW_GET_BOOL_WITH_DEFAULT(api_data, "import", false);
    
    //get scrip description
    ScriptSDWrapper script_dw(api_data.get());
    ScriptBaseDescriptionSDWrapper script_bs_result_dw;
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)

    //call dataaccesso for insert new script and get the sequence value
    if(import) {
      /*  if((err = s_da->insertNewScript(script_dw.dataWrapped()))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating new script %1%[%2%]",%script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
        }*/
      if((err = s_da->insertNewScript(api_data))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating new script %1%[%2%]",%script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
        }
    } else {
        if(script_dw.dataWrapped().script_description.unique_id == 0) {
       /*     if((err = s_da->insertNewScript(script_dw.dataWrapped()))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating new script %1%[%2%]",%script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
            }*/
	  if((err = s_da->insertNewScript(api_data))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating new script %1%[%2%]",%script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
            }
        } else {
           /* if((err = s_da->updateScript(script_dw.dataWrapped()))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error updating script %1%[%2%]", %script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
            }*/
	  if((err = s_da->updateScript(api_data))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error updating script %1%[%2%]", %script_dw.dataWrapped().script_description.name%script_dw.dataWrapped().script_description.unique_id));
            }
        }
    }
    
    //return the script base description
    script_bs_result_dw.dataWrapped() = script_dw.dataWrapped().script_description;
    return script_bs_result_dw.serialize();
}
