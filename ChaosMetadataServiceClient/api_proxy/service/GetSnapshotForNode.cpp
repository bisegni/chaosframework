/*
 *	GetSnapshotForNode.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/service/GetSnapshotForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetSnapshotForNode,
                        "service",
                        "getSnapshotForNode")
/*!
 
 */
ApiProxyResult GetSnapshotForNode::execute(const std::string& snapshot_name) {
    CDataWrapper *message = new CDataWrapper();
    message->addStringValue("snapshot_name", snapshot_name);
    return callApi(message);
}

ChaosUniquePtr<GetSnapshotForNodeHelper> GetSnapshotForNode::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetSnapshotForNodeHelper>(new GetSnapshotForNodeHelper(api_result));
}


GetSnapshotForNodeHelper::GetSnapshotForNodeHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("snapshot_for_node")) return;
    if(!api_result->isVectorValue("snapshot_for_node")) return;
    
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> snapshot_desc_list(api_result->getVectorValue("snapshot_for_node"));
    for(int idx = 0;
        idx < snapshot_desc_list->size();
        idx++) {
        const std::string node_uid = snapshot_desc_list->getStringElementAtIndex(idx);
        shapshot_list.push_back(node_uid);
    }
}

GetSnapshotForNodeHelper::~GetSnapshotForNodeHelper() {}

size_t GetSnapshotForNodeHelper::getSnapshotListSize() {
    return shapshot_list.size();
}
const ChaosStringVector& GetSnapshotForNodeHelper::getSnapshotList(){
    return shapshot_list;
}
