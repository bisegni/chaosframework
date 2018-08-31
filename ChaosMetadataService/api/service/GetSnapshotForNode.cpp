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

#include "GetSnapshotForNode.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(GetAllSnapshot)
#define DBG  DBG_LOG(GetAllSnapshot)
#define ERR  ERR_LOG(GetAllSnapshot)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CD(GetSnapshotForNode,getSnapshotForNode)

CDWUniquePtr GetSnapshotForNode::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The node unique id key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, "The node unique id needs to be a string");
    
    int err = 0;
    ChaosStringVector snapshot_for_node;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
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
    return result;
}
