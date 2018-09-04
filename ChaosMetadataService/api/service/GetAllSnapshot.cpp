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

#include "GetAllSnapshot.h"

#define S_GAS_INFO INFO_LOG(GetAllSnapshot)
#define S_GAS_DBG  DBG_LOG(GetAllSnapshot)
#define S_GAS_ERR  ERR_LOG(GetAllSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(GetAllSnapshot, "getAllSnapshot")

CDWUniquePtr GetAllSnapshot::execute(CDWUniquePtr api_data) {
    int err = 0;
    SnapshotList snapshots;
    CreateNewDataWrapper(result, );
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
    return result;
}
