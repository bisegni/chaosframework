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

#include "GetSnapshotDatasetsForNode.h"

#include <chaos/common/chaos_types.h>

#define INFO INFO_LOG(GetSnapshotDatasetsForNode)
#define DBG  DBG_LOG(GetSnapshotDatasetsForNode)
#define ERR  ERR_LOG(GetSnapshotDatasetsForNode)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CD(GetSnapshotDatasetsForNode, getSnapshotDatasetForNode)

CDWUniquePtr GetSnapshotDatasetsForNode::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The node unique id key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, "The node unique id needs to be a string");
    CHECK_KEY_THROW_AND_LOG(api_data, "snapshot_name", ERR, -4, "The snpshot name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("snapshot_name"), ERR, -5, "The snapshot name needs to be a string");
    int err = 0;
    VectorStrCDWShrdPtr saved_dataset;
    CreateNewDataWrapper(result,);
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string snapshot_name = api_data->getStringValue("snapshot_name");
    
    GET_DATA_ACCESS(SnapshotDataAccess, s_da, -6);
    
    if((err = s_da->getDatasetInSnapshotForNode(node_uid,
                                                snapshot_name,
                                                saved_dataset))){
        LOG_AND_TROW(ERR, -5, CHAOS_FORMAT("Error retrieving saved dataset for node %1% and dataset %2%",%node_uid%snapshot_name));
    }
    
    for(VectorStrCDWShrdPtrIterator it = saved_dataset.begin();
        it != saved_dataset.end();
        it++) {
        CDataWrapper dataset_element;        
        dataset_element.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME, it->first);
        dataset_element.addCSDataValue("dataset_value", *it->second);
        result->appendCDataWrapperToArray(dataset_element);
    }

    result->finalizeArrayForKey("dataset_list");
    return result;
}
