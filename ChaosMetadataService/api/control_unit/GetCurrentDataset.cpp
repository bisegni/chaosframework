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

#include "GetCurrentDataset.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_GCD_INFO INFO_LOG(GetCurrentDataset)
#define CU_GCD_DBG  DBG_LOG(GetCurrentDataset)
#define CU_GCD_ERR  ERR_LOG(GetCurrentDataset)

GetCurrentDataset::GetCurrentDataset():
AbstractApi("getCurrentDataset"){
    
}

GetCurrentDataset::~GetCurrentDataset() {
    
}

CDataWrapper *GetCurrentDataset::execute(CDataWrapper *api_data,
                                         bool& detach_data) throw(chaos::CException) {
    
    if(!api_data) {LOG_AND_TROW(CU_GCD_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_GCD_ERR, -2, "The ndk_unique_id key is mandatory");}
    
    int                 err         = 0;
    bool                presence    = false;
    CDataWrapper        *result     = NULL;
    const   std::string cu_uid      = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    
    //get default control unit node description
    if((err = cu_da->checkPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the control unit id:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("No control unit available with uid id:%1%") % cu_uid));
    }
    
    if((err = cu_da->checkDatasetPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("No dataset found for control unit with uid id:%1%") % cu_uid));
    }
    if((err = cu_da->getDataset(cu_uid, &result))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    }
    return result;
}
