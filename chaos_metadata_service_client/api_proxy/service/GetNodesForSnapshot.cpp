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

#include <chaos_metadata_service_client/api_proxy/service/GetNodesForSnapshot.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetNodesForSnapshot,
                        "service",
                        "getNodesForSnapshot")

ApiProxyResult GetNodesForSnapshot::execute(const std::string& snapshot_name) {
    CDWUNiquePtr message(new CDataWrapper());
    message->addStringValue("snapshot_name", snapshot_name);
    return callApi(message);
}

ChaosUniquePtr<GetNodesForSnapshotHelper> GetNodesForSnapshot::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetNodesForSnapshotHelper>(new GetNodesForSnapshotHelper(api_result));
}


GetNodesForSnapshotHelper::GetNodesForSnapshotHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("node_in_snapshot")) return;
    if(!api_result->isVectorValue("node_in_snapshot")) return;
    
    CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = api_result->getVectorValue("node_in_snapshot");
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
