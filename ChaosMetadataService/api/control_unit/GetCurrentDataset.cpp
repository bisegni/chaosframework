/*
 *	GetCurrentDataset.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
