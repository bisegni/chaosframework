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

#include <ChaosMetadataServiceClient/api_proxy/service/GetNodesForSnapshot.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetNodesForSnapshot,
                        "service",
                        "getNodesForSnapshot")
/*!
 
 */
ApiProxyResult GetNodesForSnapshot::execute(const std::string& snapshot_name) {
    CDataWrapper *message = new CDataWrapper();
    message->addStringValue("snapshot_name", snapshot_name);
    return callApi(message);
}

std::unique_ptr<GetNodesForSnapshotHelper> GetNodesForSnapshot::getHelper(CDataWrapper *api_result) {
    return std::unique_ptr<GetNodesForSnapshotHelper>(new GetNodesForSnapshotHelper(api_result));
}


GetNodesForSnapshotHelper::GetNodesForSnapshotHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("node_in_snapshot")) return;
    if(!api_result->isVectorValue("node_in_snapshot")) return;
    
    std::unique_ptr<CMultiTypeDataArrayWrapper> snapshot_desc_list(api_result->getVectorValue("node_in_snapshot"));
    for(int idx = 0;
        idx < snapshot_desc_list->size();
        idx++) {
        const std::string node_uid = snapshot_desc_list->getStringElementAtIndex(idx);
        node_list.push_back(node_uid);
    }
}

GetNodesForSnapshotHelper::~GetNodesForSnapshotHelper() {}

size_t GetNodesForSnapshotHelper::getNodeListSize() {
    return node_list.size();
}
const NodesInSnapshotList& GetNodesForSnapshotHelper::getNodeList(){
    return node_list;
}
