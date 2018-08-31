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

#include "GetNodesForSnapshot.h"

#include <chaos/common/chaos_types.h>

#define S_GNFN_INFO INFO_LOG(GetAllSnapshot)
#define S_GNFN_DBG  DBG_LOG(GetAllSnapshot)
#define S_GNFN_ERR  ERR_LOG(GetAllSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, NodeList);

CHAOS_MDS_DEFINE_API_CD(GetNodesForSnapshot, getNodesForSnapshot)

CDWUniquePtr GetNodesForSnapshot::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, S_GNFN_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_GNFN_ERR, -2, "The snapshot_name key is mandatory");
    if(!api_data->isStringValue("snapshot_name")) {LOG_AND_TROW(S_GNFN_ERR, -3, "snapshot_name needs to be a string");}
    
    int err = 0;
    NodeList node_in_snapshot;
    CreateNewDataWrapper(result,);
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -1);
    
    if((err = s_da->getNodeInSnapshot(snapshot_name,
                                      node_in_snapshot))){
        LOG_AND_TROW(S_GNFN_ERR, -2, "Error getting the snpahsot list");
    }
    
    for(NodeListIterator it = node_in_snapshot.begin();
        it != node_in_snapshot.end();
        it++) {
        result->appendStringToArray(*it);
    }
    
    result->finalizeArrayForKey("node_in_snapshot");
    return result;
}
