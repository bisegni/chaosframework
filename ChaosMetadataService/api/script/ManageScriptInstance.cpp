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

#include "ManageScriptInstance.h"

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

CHAOS_MDS_DEFINE_API_CLASS_CD(ManageScriptInstance, "manageScriptInstance")

CDWUniquePtr ManageScriptInstance::execute(CDWUniquePtr api_data) {
    int err = 0;
    
    //check for mandatory attributes
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "script_seq", ERR, -2, "The script seq is mandatory");
    CHECK_KEY_THROW_AND_LOG(api_data, "script_name", ERR, -3, "The script name is mandatory");
    CHECK_KEY_THROW_AND_LOG(api_data, "instance_name", ERR, -4, "The instance name is mandatory");
    CHECK_KEY_THROW_AND_LOG(api_data, "create", ERR, -5, "The create key is mandatory");
    
    const uint64_t script_seq = CDW_GET_VALUE_WITH_DEFAULT(api_data, "script_seq", getInt64Value,0);
    const std::string script_name =  CDW_GET_VALUE_WITH_DEFAULT(api_data, "script_name", getStringValue,"");
    const bool create = CDW_GET_VALUE_WITH_DEFAULT(api_data, "create", getBoolValue, false);
    
    //fetch dataaccess for the script managment
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -2)
    ChaosStringVector str_list;
    
    //standardize the usage mwith array
    if(api_data->isStringValue("instance_name")) {
        str_list.push_back(api_data->getStringValue("instance_name"));
    } else if(api_data->isVectorValue("instance_name")) {
        CMultiTypeDataArrayWrapperSPtr array(api_data->getVectorValue("instance_name"));
        for(int idx = 0;
            idx < array->size();
            idx++) {
            str_list.push_back(array->getStringElementAtIndex(idx));
        }
    }
    
    //do operation on the arry list
    for(ChaosStringVectorIterator it = str_list.begin(),
        end = str_list.end();
        it != end;
        it++) {
        if(create) {
            if((err = s_da->addScriptInstance(script_seq,
                                              script_name,
                                              *it))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating instance %1% for script %2%",%*it%script_name));
            }
        } else {
            if((err = s_da->removeScriptInstance(script_seq,
                                                 script_name,
                                                 *it))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error removing instance %1% for script %2%",%*it%script_name));
            }
        }
    }
    
    return CDWUniquePtr();
}
