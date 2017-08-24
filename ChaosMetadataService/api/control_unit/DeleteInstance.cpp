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

#include "DeleteInstance.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_DI_INFO INFO_LOG(GetInstance)
#define CU_DI_DBG  DBG_LOG(GetInstance)
#define CU_DI_ERR  ERR_LOG(GetInstance)

DeleteInstance::DeleteInstance():
AbstractApi("deleteInstance"){

}

DeleteInstance::~DeleteInstance() {

}

CDataWrapper *DeleteInstance::execute(CDataWrapper *api_data,
                                      bool& detach_data) throw(chaos::CException) {
    int err = 0;
    if(!api_data) {LOG_AND_TROW(CU_DI_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_DI_ERR, -2, "The ndk_unique_id key (representing the control unit uid) is mandatory");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)) {LOG_AND_TROW(CU_DI_ERR, -3, "The ndk_parent key (representing the unit server uid) is mandatory");}

    const std::string cu_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string us_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);

    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    if((err = cu_da->deleteInstanceDescription(us_uid,
                                               cu_uid))){
        LOG_AND_TROW(CU_DI_ERR, err, boost::str(boost::format("Error removing the control unit instance description for cuid:%1% and usuid:%2%") % cu_uid % us_uid));
    }
    return NULL;
}