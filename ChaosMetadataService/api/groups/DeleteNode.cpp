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

#include "DeleteNode.h"

using namespace chaos::metadata_service::api::groups;

#define G_DN_INFO INFO_LOG(DeleteNode)
#define G_DN_DBG  DBG_LOG(DeleteNode)
#define G_DN_ERR  ERR_LOG(DeleteNode)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::groups;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CD(DeleteNode,deleteNode)

CDWUniquePtr DeleteNode::execute(CDWUniquePtr api_data) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, G_DN_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "node_name", G_DN_ERR, -2, "The node_name key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue("node_name"), G_DN_ERR, -3, "The node_name needs to be a string");
    
    CHECK_KEY_THROW_AND_LOG(api_data, "node_domain", G_DN_ERR, -4, "The node_domain key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue("node_domain"), G_DN_ERR, -5, "The node_domain needs to be a string");
    
    CHECK_KEY_THROW_AND_LOG(api_data, "node_parent_path", G_DN_ERR, -6, "The node_parent_path key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue("node_parent_path"), G_DN_ERR, -7, "The node_parent_path needs to be a string");
    
    const std::string node_domain = api_data->getStringValue("node_domain");
    const std::string node_name = api_data->getStringValue("node_name");
    const std::string node_parent_path = api_data->getStringValue("node_parent_path");
    
    GET_DATA_ACCESS(TreeGroupDataAccess, tg_da, -8);
    
    if((err = tg_da->deleteNodeGroupToDomain(node_domain,
                                             node_name,
                                             node_parent_path))) {
        LOG_AND_TROW_FORMATTED(G_DN_ERR, err, "Error deleting node %1%/%2% in the domain %3%", %node_parent_path%node_name%node_domain);
    }
    return CDWUniquePtr();
}
