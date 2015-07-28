/*
 *	ResetAllApi.cpp
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

#include "ResetAllApi.h"

#define S_RA_INFO INFO_LOG(ResetAllApi)
#define S_RA_DBG  DBG_LOG(ResetAllApi)
#define S_RA_ERR  ERR_LOG(ResetAllApi)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::healt;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< boost::shared_ptr<CDataWrapper> > ResultVector;

ResetAllApi::ResetAllApi():
AbstractApi("resetAll"){
    
}

ResetAllApi::~ResetAllApi() {
    
}

chaos::common::data::CDataWrapper *ResetAllApi::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -1);
    if((err = u_da->resetAllData())) {
        LOG_AND_TROW(S_RA_ERR, err, "Error resetting all data");
    }
    return NULL;
}