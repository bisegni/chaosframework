/*
 *	SetSnapshotDatasetsForNode.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 26/04/2017 INFN, National Institute of Nuclear Physics
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

#include "SetSnapshotDatasetsForNode.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(SetSnapshotDatasetsForNode)
#define DBG  DBG_LOG(SetSnapshotDatasetsForNode)
#define ERR  ERR_LOG(SetSnapshotDatasetsForNode)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

SetSnapshotDatasetsForNode::SetSnapshotDatasetsForNode():
AbstractApi("setSnapshotDatasetsForNode"){}

SetSnapshotDatasetsForNode::~SetSnapshotDatasetsForNode() {}

chaos::common::data::CDataWrapper *SetSnapshotDatasetsForNode::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_MANDATORY_KEY(api_data, "snapshot_name", ERR, -2);
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("snapshot_name"), ERR, -3, "The snapshot name needs to be a string");
    CHECK_MANDATORY_KEY(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -4);
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -5, "The node unique id needs to be a string");
    CHECK_MANDATORY_KEY(api_data, "dataset", ERR, -6);
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isVectorValue("dataset"), ERR, -7, "The dataset needs to be a vector");
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -8);
    
    int err = 0;
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    std::auto_ptr<CMultiTypeDataArrayWrapper> dataset_vec_ptr(api_data->getVectorValue("dataset"));
    
    for(int idx = 0;
        idx < dataset_vec_ptr->size();
        idx++) {
        std::auto_ptr<CDataWrapper> dataset_info_ptr(dataset_vec_ptr->getCDataWrapperElementAtIndex(idx));
        
        if(dataset_info_ptr->hasKey("dataset_key") == false ||
           dataset_info_ptr->hasKey("dataset_value") == false) {
            //
            continue;
        }
        
        const std::string dataset_key = dataset_info_ptr->getStringValue("dataset_key");
        const std::auto_ptr<CDataWrapper> dataset_value(dataset_info_ptr->getCSDataValue("dataset_value"));
        
        //call api for set the value
        if((err = s_da->setDatasetInSnapshotForNode(node_uid, snapshot_name, dataset_key, *dataset_value))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error %1% setting the dataset %2% for node %3% in snapshot %4% ",%err%dataset_key%node_uid%snapshot_name));
        }
    }
    return NULL;
}
