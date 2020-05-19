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
#include "NodeDelete.h"

#define NS_INFO INFO_LOG(NodeDelete)
#define NS_DBG  DBG_LOG(NodeDelete)
#define NS_ERR  ERR_LOG(NodeDelete)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(NodeDelete, "nodeDelete");

CDWUniquePtr NodeDelete::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, NS_ERR, -1, "No parameter found")
    
    chaos::common::data::CDataWrapper *result=NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(NS_ERR, -1, "Node unique id not found: "+api_data->getJSONString())
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        LOG_AND_TROW(NS_ERR,-2, "Node type not found");
    }
    std::string node_uid=api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    std::string node_type=api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    
    NS_DBG<<" delete "<<node_uid<<"("<<node_type<<")";
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4);

    if (n_da->deleteNode(node_uid,node_type)){
            LOG_AND_TROW(NS_ERR, -5, "Cannot delete node: "+node_uid+" ["+node_type+"]");
    }

      
    return CDWUniquePtr();
}
