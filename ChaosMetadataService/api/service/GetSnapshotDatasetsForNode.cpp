/*
 *	GetSnapshotDatasetsForNode.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/09/2016 INFN, National Institute of Nuclear Physics
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

#include "GetSnapshotDatasetsForNode.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(GetSnapshotDatasetsForNode)
#define DBG  DBG_LOG(GetSnapshotDatasetsForNode)
#define ERR  ERR_LOG(GetSnapshotDatasetsForNode)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

GetSnapshotDatasetsForNode::GetSnapshotDatasetsForNode():
AbstractApi("getSettingForNodeSnapshot"){}

GetSnapshotDatasetsForNode::~GetSnapshotDatasetsForNode() {}

chaos::common::data::CDataWrapper *GetSnapshotDatasetsForNode::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The node unique id key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, "The node unique id needs to be a string");
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", ERR, -2, "The snpshot name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("snapshot_name"), ERR, -3, "The snapshot name needs to be a string");
    int err = 0;
    ChaosStringVector snapshot_for_node;
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -4);
    
    if((err = s_da->getSnapshotForNode(node_uid,
                                       snapshot_for_node))){
        LOG_AND_TROW(ERR, -5, "Error getting the node list");
    }
    
    for(ChaosStringVectorIterator it = snapshot_for_node.begin();
        it != snapshot_for_node.end();
        it++) {
        result->appendStringToArray(*it);
    }
    
    result->finalizeArrayForKey("snapshot_for_node");
    return result.release();
}
