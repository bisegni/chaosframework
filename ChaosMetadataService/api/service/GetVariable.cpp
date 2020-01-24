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

#include "GetVariable.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(GetVariable)
#define DBG  DBG_LOG(GetVariable)
#define ERR  ERR_LOG(GetVariable)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(GetVariable, "getVariable")

CDWUniquePtr GetVariable::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, VariableDefinitionKey::VARIABLE_NAME_KEY, ERR, -2, "The variable_name key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue(VariableDefinitionKey::VARIABLE_NAME_KEY), ERR, -3, "The variable_name needs to be an object");
    
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -4);
    
    int err = 0;
    const std::string variable_name = api_data->getStringValue(VariableDefinitionKey::VARIABLE_NAME_KEY);
    
    CDataWrapper *result = NULL;
    if((err = u_da->getVariable(variable_name, &result))){
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error retrieving the variable %1% with error %2%", %variable_name%err));
    }
    return CDWUniquePtr(result);
}
