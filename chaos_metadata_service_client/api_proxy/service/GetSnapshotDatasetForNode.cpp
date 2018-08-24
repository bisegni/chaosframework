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

#include <chaos_metadata_service_client/api_proxy/service/GetSnapshotDatasetForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetSnapshotDatasetForNode,
                        "service",
                        "getSnapshotDatasetForNode")

ApiProxyResult GetSnapshotDatasetForNode::execute(const std::string& snapshot_name,
                                                  const std::string& node_uid) {
    CDataWrapper *message = new CDataWrapper();
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    message->addStringValue("snapshot_name", snapshot_name);
    return callApi(message);
}

void GetSnapshotDatasetForNode::getAsMap(chaos::common::data::CDataWrapper& api_result,
                                         VectorStrCDWShrdPtr& dataset_in_snapshot) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result.hasKey("dataset_list")) return;
    if(!api_result.isVectorValue("dataset_list")) return;
    
    CMultiTypeDataArrayWrapperSPtr snapshot_list = api_result.getVectorValue("dataset_list");
    for(int idx = 0;
        idx < snapshot_list->size();
        idx++) {
        CDWUniquePtr snapshot_dataset_element(snapshot_list->getCDataWrapperElementAtIndex(idx));
        const std::string dataset_name = snapshot_dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME);
        CDWShrdPtr saved_dataset(snapshot_dataset_element->getCSDataValue("dataset_value"));
        
        dataset_in_snapshot.push_back(PairStrCDWShrdPtr(dataset_name,
                                                        saved_dataset));
    }
}
