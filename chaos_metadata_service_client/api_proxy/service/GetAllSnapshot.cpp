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

#include <chaos_metadata_service_client/api_proxy/service/GetAllSnapshot.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetAllSnapshot,
                        "service",
                        "getAllSnapshot")

ApiProxyResult GetAllSnapshot::execute(const std::string& query_filter) {
    CDataWrapper *message = new CDataWrapper();
    message->addStringValue("snapshot_query_filter", query_filter);
    return callApi(message);
}

ChaosUniquePtr<GetAllSnapshotHelper> GetAllSnapshot::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetAllSnapshotHelper>(new GetAllSnapshotHelper(api_result));
}


GetAllSnapshotHelper::GetAllSnapshotHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("snapshot_list_result")) return;
    
    CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = api_result->getVectorValue("snapshot_list_result");
    for(int idx = 0;
        idx < snapshot_desc_list->size();
        idx++) {
        CDWUniquePtr tmp_desc(snapshot_desc_list->getCDataWrapperElementAtIndex(idx));
        
        SnapshotInformationPtr snapshot_information(new SnapshotInformation());
        if(tmp_desc->hasKey("snap_name")) {
            snapshot_information->name = tmp_desc->getStringValue("snap_name");
        }
        if(tmp_desc->hasKey("snap_ts")) {
            snapshot_information->timestamp = tmp_desc->getUInt64Value("snap_ts");
        }
        if(tmp_desc->hasKey("job_concurency")) {
            snapshot_information->work_concurency = tmp_desc->getUInt32Value("job_concurency");
        }

        snapshot_list.push_back(snapshot_information);
    }
}

GetAllSnapshotHelper::~GetAllSnapshotHelper() {
    
}

size_t GetAllSnapshotHelper::getSnapshotListSize() {
    return snapshot_list.size();
}
const SnapshotInfoList& GetAllSnapshotHelper::getSnapshotInfoList(){
    return snapshot_list;
}
