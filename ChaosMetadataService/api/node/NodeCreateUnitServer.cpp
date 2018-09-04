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
#include "NodeCreateUnitServer.h"

#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(NodeCreateUnitServer, "newNode");

CDWUniquePtr NodeCreateUnitServer::execute(CDWUniquePtr api_data) {
    bool presence = false;
    CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(USRA_ERR, -1, "Node unique id not found")
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        throw CException(-2, "Node type not found", __PRETTY_FUNCTION__);
    }
    
    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) != 0) {
        throw CException(-3, "Note type is not an Unit Server", __PRETTY_FUNCTION__);
    }
    
    //!get the unit server data access
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -4)
    if(us_da->checkPresence(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), presence)) {
        
    }
    return CDWUniquePtr(result);
}
