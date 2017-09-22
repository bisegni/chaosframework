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

#include "RemoveVariable.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(RemoveVariable)
#define DBG  DBG_LOG(RemoveVariable)
#define ERR  ERR_LOG(RemoveVariable)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

RemoveVariable::RemoveVariable():
AbstractApi("removeVariable"){}

RemoveVariable::~RemoveVariable() {}

chaos::common::data::CDataWrapper *RemoveVariable::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "variable_name", ERR, -2, "The variable_name key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("variable_name"), ERR, -3, "The variable_name needs to be an object");
    
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -4);
    
    int err = 0;
    const std::string variable_name = api_data->getStringValue("variable_name");
    
    
    if((err = u_da->deleteVariable(variable_name))){
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error removing variable %1% with error %2%", %variable_name%err));
    }
    return NULL;
}
