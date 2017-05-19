/*
 *	GetNodeChilds.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/groups/GetNodeChilds.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::groups;

API_PROXY_CD_DEFINITION(GetNodeChilds,
                        "groups",
                        "getNodeChilds")
/*!
 
 */
ApiProxyResult GetNodeChilds::execute(const std::string& node_domain,
                                      const std::string& node_path) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    if(node_path.size()){pack->addStringValue("node_path", node_path);}
    pack->addStringValue("group_domain", node_domain);
    return callApi(pack.release());
}

ChaosUniquePtr<GetNodeChildsHelper> GetNodeChilds::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetNodeChildsHelper>(new GetNodeChildsHelper(api_result));
}


GetNodeChildsHelper::GetNodeChildsHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("node_child_list")) return;
    
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> vec(api_result->getVectorValue("node_child_list"));
    for(int idx = 0;
        idx < vec->size();
        idx++) {
        const std::string node_name = vec->getStringElementAtIndex(idx);
        node_child_list.push_back(node_name);
    }
}

GetNodeChildsHelper::~GetNodeChildsHelper() {}

size_t GetNodeChildsHelper::getNodeChildsListSize() {
    return node_child_list.size();
}

const NodeChildList& GetNodeChildsHelper::getNodeChildsList(){
    return node_child_list;
}
