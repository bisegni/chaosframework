/*
 *	GetAllSnapshot.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/service/GetAllSnapshot.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(GetAllSnapshot,
                        "service",
                        "getAllSnapshot")
/*!
 
 */
ApiProxyResult GetAllSnapshot::execute(const std::string& query_filter) {
    return callApi();
}

std::auto_ptr<GetAllSnapshotHelper> GetAllSnapshot::getHelper(ApiProxyResult _api_result) {
    return std::auto_ptr<GetAllSnapshotHelper>(new GetAllSnapshotHelper(_api_result));
}


GetAllSnapshotHelper::GetAllSnapshotHelper(ApiProxyResult _api_result):
ApiResultHelper(_api_result){
    //snapshot_list
}

GetAllSnapshotHelper::~GetAllSnapshotHelper() {
    
}

int GetAllSnapshotHelper::update() {
    int err = 0;
    if((err = ApiResultHelper::update()))return err;
    
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result->getResult() || !api_result->getResult()->hasKey("snapshot_list_result")) return err;
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> snapshot_desc_list(api_result->getResult()->getVectorValue("snapshot_list_result"));
    for(int idx = 0;
        idx < snapshot_desc_list->size();
        idx++) {
        std::auto_ptr<CDataWrapper> tmp_desc(snapshot_desc_list->getCDataWrapperElementAtIndex(idx));
        
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
    }
    return err;
}

size_t GetAllSnapshotHelper::getSnapshotListSize() {
    return snapshot_list.size();
}
const SnapshotInfoList& GetAllSnapshotHelper::getSnapshotInfoList(){
    return snapshot_list;
}
