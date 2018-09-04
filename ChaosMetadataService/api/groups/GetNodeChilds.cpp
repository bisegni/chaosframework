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

#include "GetNodeChilds.h"
#include <chaos/common/chaos_types.h>

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, NodeList)

using namespace chaos::metadata_service::api::groups;

#define G_AN_INFO INFO_LOG(GetNodeChilds)
#define G_AN_DBG  DBG_LOG(GetNodeChilds)
#define G_AN_ERR  ERR_LOG(GetNodeChilds)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::groups;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(GetNodeChilds, "getNodeChilds")

CDWUniquePtr GetNodeChilds::execute(CDWUniquePtr api_data) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, G_AN_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "group_domain", G_AN_ERR, -2, "The group_domain key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue("group_domain"), G_AN_ERR, -3, "The group_domain needs to be a string");
    
    GET_DATA_ACCESS(TreeGroupDataAccess, tg_da, -6);
    NodeList node_list;
    const std::string group_domain = api_data->getStringValue("group_domain");
    if(api_data->hasKey("node_path")) {
        const std::string node_path = api_data->getStringValue("node_path");
        if((err = tg_da->getNodeChildFromPath(group_domain, node_path, node_list))){
            LOG_AND_TROW_FORMATTED(G_AN_ERR, -6, "Error returning node list for path %1% in domain %2% with error code %3%", %node_path%group_domain%err);
        }
    } else {
        if((err = tg_da->getNodeRootFromDomain(group_domain, node_list))){
            LOG_AND_TROW_FORMATTED(G_AN_ERR, -6, "Error returning root node list for domain %1% with error code %2%", %group_domain%err);
        }
    }
    //compose output
    CreateNewDataWrapper(result,);
    if(node_list.size()) {
        for(NodeListIterator it = node_list.begin();
            it != node_list.end();
            it++) {
            result->appendStringToArray(*it);
        }
    }
    result->finalizeArrayForKey("node_child_list");
    return result;
}
