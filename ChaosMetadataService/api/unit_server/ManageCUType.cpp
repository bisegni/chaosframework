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

#include "ManageCUType.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;
using namespace chaos::metadata_service::persistence::data_access;

#define US_ACT_INFO INFO_LOG(ManageCUType)
#define US_ACT_DBG  DBG_LOG(ManageCUType)
#define US_ACT_ERR  ERR_LOG(ManageCUType)

ManageCUType::ManageCUType():
AbstractApi("manageCUType"){
    
}

ManageCUType::~ManageCUType() {
    
}

CDataWrapper *ManageCUType::execute(CDataWrapper *api_data,
                             bool& detach_data) throw(chaos::CException) {
    
    CHECK_CDW_THROW_AND_LOG(api_data, US_ACT_ERR, -1, "No parameter has been set!")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, US_ACT_ERR, -2, "No ndk_uid is mandatory!")
    CHECK_KEY_THROW_AND_LOG(api_data, UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS, US_ACT_ERR, -3, "No usndk_hosted_cu_class is mandatory!")
    CHECK_KEY_THROW_AND_LOG(api_data, "operation", US_ACT_ERR, -4, "No 'operation' has been set!")
    
    int err = 0;
    bool presence = false;
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -5)
    //get the parameter
    const std::string us_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if((err = us_da->checkPresence(us_uid, presence))) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -6, "Error fetch the presence for the uid:%1%", % us_uid);
    }
    
    if(!presence) {
        LOG_AND_TROW_FORMATTED(US_ACT_ERR, -7, "The unit server '%1%' has not been found", % us_uid);
    }
    
    const std::string new_cu_type = api_data->getStringValue(UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS);
    const int32_t operation = api_data->getInt32Value("operation");
    //we can proceed
    switch (operation) {
        case 0:
            if((err = us_da->addCUType(us_uid, new_cu_type))) {
                LOG_AND_TROW_FORMATTED(US_ACT_ERR, -8, "Error adding cu type: %1% to unit server: %2%", %us_uid%new_cu_type);
            }
            break;
        case 1:
            if((err = us_da->removeCUType(us_uid, new_cu_type))) {
                LOG_AND_TROW_FORMATTED(US_ACT_ERR, -9, "Error removing cu type: %1% to unit server: %2%", %us_uid%new_cu_type);
            }
            break;
        default:
            LOG_AND_TROW_FORMATTED(US_ACT_ERR, -10, "Invalid oepration:%1% for ManageCUType api[%2%:%3%]", %operation%us_uid%new_cu_type);
            break;
    }

    return NULL;
}