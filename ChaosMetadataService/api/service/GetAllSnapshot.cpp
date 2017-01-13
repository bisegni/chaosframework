/*
 *	GetAllSnapshot.cpp
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

#include "GetAllSnapshot.h"

#define S_GAS_INFO INFO_LOG(GetAllSnapshot)
#define S_GAS_DBG  DBG_LOG(GetAllSnapshot)
#define S_GAS_ERR  ERR_LOG(GetAllSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

GetAllSnapshot::GetAllSnapshot():
AbstractApi("getAllSnapshot"){}

GetAllSnapshot::~GetAllSnapshot() {}

chaos::common::data::CDataWrapper *GetAllSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {

    
    int err = 0;
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    SnapshotList snapshots;

    std::string snapshot_query_result;
    
    if(api_data &&
       api_data->hasKey("snapshot_query_filter") &&
       api_data->isStringValue("snapshot_query_filter")) {
        snapshot_query_result = api_data->getStringValue("snapshot_query_filter");
    }
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -1);
    
    if((err = s_da->getAllSnapshot(snapshots))){
        LOG_AND_TROW(S_GAS_ERR, -2, "Error getting the snpahsot list");
    }
    
    for(SnapshotListIterator it = snapshots.begin();
        it != snapshots.end();
        it++) {
        result->appendCDataWrapperToArray(*(*it).get());
    }
    
    result->finalizeArrayForKey("snapshot_list_result");
    return result.release();
}
