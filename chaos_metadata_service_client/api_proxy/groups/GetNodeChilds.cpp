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

#include <chaos_metadata_service_client/api_proxy/groups/GetNodeChilds.h>

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
    CDWUniquePtr pack(new CDataWrapper());
    if(node_path.size()){pack->addStringValue("node_path", node_path);}
    pack->addStringValue("group_domain", node_domain);
    return callApi(pack);
}

ChaosUniquePtr<GetNodeChildsHelper> GetNodeChilds::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetNodeChildsHelper>(new GetNodeChildsHelper(api_result));
}


GetNodeChildsHelper::GetNodeChildsHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("node_child_list")) return;
    
    CMultiTypeDataArrayWrapperSPtr vec = api_result->getVectorValue("node_child_list");
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
