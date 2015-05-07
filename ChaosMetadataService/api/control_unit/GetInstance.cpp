/*
 *	GetInstance.cpp
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

#include "GetInstance.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_GI_INFO INFO_LOG(GetInstance)
#define CU_GI_DBG  DBG_LOG(GetInstance)
#define CU_GI_ERR  ERR_LOG(GetInstance)

GetInstance::GetInstance():
AbstractApi("getInstance"){

}

GetInstance::~GetInstance() {

}

CDataWrapper *GetInstance::execute(CDataWrapper *api_data,
                                   bool& detach_data) throw(chaos::CException) {

    if(!api_data) {LOG_AND_TROW(CU_GI_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_GI_ERR, -2, "The ndk_unique_id key (representing the control unit uid) is mandatory");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)) {LOG_AND_TROW(CU_GI_ERR, -3, "The ndk_parent key (representing the unit server uid) is mandatory");}

    int err = 0;
    CDataWrapper *result = NULL;
    const std::string cu_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string us_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    if((err = cu_da->getInstanceDescription(us_uid,
                                            cu_uid,
                                            &result))){
        LOG_AND_TROW(CU_GI_ERR, err, boost::str(boost::format("Error fetching the control unit instance description for cuid:%1% and usuid:%2%") % cu_uid % us_uid));
    }
    return result;
}