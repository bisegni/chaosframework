/*
 *	LoadNodeAssociation.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/02/2017 INFN, National Institute of Nuclear Physics
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

#include "LoadNodeAssociation.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent;    

#define INFO INFO_LOG(LoadNodeAssociation)
#define ERR  DBG_LOG(LoadNodeAssociation)
#define DBG  ERR_LOG(LoadNodeAssociation)

using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

LoadNodeAssociation::LoadNodeAssociation():
AbstractApi("loadNodeAssociation"){
}

LoadNodeAssociation::~LoadNodeAssociation() {
}

CDataWrapper *LoadNodeAssociation::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The key %1% need to be a string", %NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CHECK_KEY_THROW_AND_LOG(api_data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERR, -4, CHAOS_FORMAT("The key %1% is mandatory", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED), ERR, -5, CHAOS_FORMAT("The key %1% need to be a string", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    //we can rpocessd
    GET_DATA_ACCESS(AgentDataAccess, a_da, -6);
    
    int err = 0;
    AgentAssociationSDWrapper assoc_sd_wrapper;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string node_associated = api_data->getStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED);
    if((err = a_da->loadNodeAssociationForAgent(agent_uid, node_associated, assoc_sd_wrapper()))) {
        LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error loading association for node %1% into agent %2% with error %3%", %node_associated%agent_uid%err));
    }
    return assoc_sd_wrapper.serialize().release();
}