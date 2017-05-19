/*
 *	SetVariable.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/12/2016 INFN, National Institute of Nuclear Physics
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

#include "SetVariable.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(SetVariable)
#define DBG  DBG_LOG(SetVariable)
#define ERR  ERR_LOG(SetVariable)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

SetVariable::SetVariable():
AbstractApi("setVariable"){}

SetVariable::~SetVariable() {}

chaos::common::data::CDataWrapper *SetVariable::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "variable_name", ERR, -2, "The variable_name key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("variable_name"), ERR, -3, "The variable_name needs to be an object");
    CHECK_KEY_THROW_AND_LOG(api_data, "variable_value", ERR, -4, "The variable_value key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isCDataWrapperValue("variable_value"), ERR, -5, "The variable_value key needs to be an object");
    
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -6);
    
    int err = 0;
    const std::string variable_name = api_data->getStringValue("variable_name");
    ChaosUniquePtr<chaos::common::data::CDataWrapper> variable_value(api_data->getCSDataValue("variable_value"));

    
    if((err = u_da->setVariable(variable_name,
                                *variable_value))){
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error setting variable %1% with error %2%", %variable_name%err));
    }
    return NULL;
}
