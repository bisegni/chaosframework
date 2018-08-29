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
    bool snap_presence = false;
    std::string working_job_unique_id;
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    CMultiTypeDataArrayWrapperSPtr dataset_vec_ptr(api_data->getVectorValue("dataset"));
    
    if(s_da->isSnapshotPresent(snapshot_name,
                               snap_presence)){
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error checking snapshot %1% presences",%snapshot_name));
    } if(snap_presence == false) {
        if((err = s_da->snapshotCreateNewWithName(snapshot_name,
                                                  working_job_unique_id))){
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error creating snapshot %1%",%snapshot_name));
        }
    }
    
    if((err = s_da->snapshotIncrementJobCounter(working_job_unique_id,
                                                snapshot_name,
                                                true))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error incrementing job counter on snapshot %1%",%snapshot_name));
    }
    try{
        for(int idx = 0;
            idx < dataset_vec_ptr->size();
            idx++) {
            ChaosUniquePtr<chaos::common::data::CDataWrapper> dataset_info_ptr(dataset_vec_ptr->getCDataWrapperElementAtIndex(idx));
            
            if(dataset_info_ptr->hasKey("dataset_key") == false ||
               dataset_info_ptr->hasKey("dataset_value") == false) {
                //
                continue;
            }
            
            const std::string dataset_key = dataset_info_ptr->getStringValue("dataset_key");
            const ChaosUniquePtr<chaos::common::data::CDataWrapper> dataset_value(dataset_info_ptr->getCSDataValue("dataset_value"));
            
            //call api for set the value

            if((err = s_da->setDatasetInSnapshotForNode(working_job_unique_id,
                                                        node_uid,
                                                        snapshot_name,
                                                        dataset_key,
                                                        *dataset_value))) {
                LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error setting the dataset \"%1%\" for node \"%2%\" in snapshot \"%3%\" ",%dataset_key%node_uid%snapshot_name));

            }
        }
    }catch(CException& ex) {
        if((err = s_da->snapshotIncrementJobCounter(working_job_unique_id,
                                                    snapshot_name,
                                                    false))) {
            LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error incrementing job counter on snapshot %1%",%snapshot_name));
        }
        throw;
    }
    
    return NULL;
}
