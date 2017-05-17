/*
 *	GetSnapshotDatasetForNode.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/12/2016 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/service/GetSnapshotDatasetForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetSnapshotDatasetForNode,
                        "service",
                        "getSnapshotDatasetForNode")
/*!
 
 */
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
    
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> snapshot_list(api_result.getVectorValue("dataset_list"));
    for(int idx = 0;
        idx < snapshot_list->size();
        idx++) {
        ChaosUniquePtr<CDataWrapper> snapshot_dataset_element(snapshot_list->getCDataWrapperElementAtIndex(idx));
        const std::string dataset_name = snapshot_dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME);
        CDWShrdPtr saved_dataset(snapshot_dataset_element->getCSDataValue("dataset_value"));
        
        dataset_in_snapshot.push_back(PairStrCDWShrdPtr(dataset_name,
                                                        saved_dataset));
    }
}
