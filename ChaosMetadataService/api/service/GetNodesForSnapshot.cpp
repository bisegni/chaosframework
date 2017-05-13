/*
 *	GetNodesForSnapshot.cpp
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

#include "GetNodesForSnapshot.h"

#include <chaos/common/chaos_types.h>

#define S_GNFN_INFO INFO_LOG(GetAllSnapshot)
#define S_GNFN_DBG  DBG_LOG(GetAllSnapshot)
#define S_GNFN_ERR  ERR_LOG(GetAllSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, NodeList);

GetNodesForSnapshot::GetNodesForSnapshot():
AbstractApi("getNodesForSnapshot"){}

GetNodesForSnapshot::~GetNodesForSnapshot() {}

chaos::common::data::CDataWrapper *GetNodesForSnapshot::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, S_GNFN_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", S_GNFN_ERR, -2, "The snapshot_name key is mandatory");
    if(!api_data->isStringValue("snapshot_name")) {LOG_AND_TROW(S_GNFN_ERR, -3, "snapshot_name needs to be a string");}
    
    int err = 0;
    NodeList node_in_snapshot;
    std::unique_ptr<CDataWrapper> result(new CDataWrapper());
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
    return result.release();
}
