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

#include <chaos_metadata_service_client/api_proxy/service/GetSnapshotForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetSnapshotForNode,
                        "service",
                        "getSnapshotForNode")

ApiProxyResult GetSnapshotForNode::execute(const std::string& snapshot_name) {
    CDWUNiquePtr message(new CDataWrapper());
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
    
    CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = api_result->getVectorValue("snapshot_for_node");
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
