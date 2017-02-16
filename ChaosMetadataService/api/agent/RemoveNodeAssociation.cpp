/*
 *	RemoveNodeAssociation.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 09/02/2017 INFN, National Institute of Nuclear Physics
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

#include "RemoveNodeAssociation.h"
#include "../../batch/agent/AgentRemoveNodeSafety.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(RemoveNodeAssociation)
#define ERR  DBG_LOG(RemoveNodeAssociation)
#define DBG  ERR_LOG(RemoveNodeAssociation)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

RemoveNodeAssociation::RemoveNodeAssociation():
AbstractApi("removeNodeAssociation"){
}

RemoveNodeAssociation::~RemoveNodeAssociation() {
}

CDataWrapper *RemoveNodeAssociation::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The key %1% need to be a string", %NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CHECK_KEY_THROW_AND_LOG(api_data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERR, -4, CHAOS_FORMAT("The key %1% is mandatory", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    CHAOS_LASSERT_EXCEPTION((api_data->isStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED) ||
                             api_data->isVectorValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)), ERR, -5, CHAOS_FORMAT("The key %1% need to be a string", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    //we can rpocessd
    GET_DATA_ACCESS(AgentDataAccess, a_da, -6);
    int err = 0;
    detach_data = true;
    uint64_t cmd_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::agent::AgentRemoveNodeSafety),
                                                        api_data,
                                                        0,
                                                        1000);
    //    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //    if(api_data->isStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
    //        const std::string node_associated = api_data->getStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED);
    //        if((err = a_da->removeNodeAssociationForAgent(agent_uid, node_associated))) {
    //            LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error removing association for node %1% into agent %2% with error %3%", %node_associated%agent_uid%err));
    //        }
    //    } else {
    //        ChaosStringVectorSDWrapper assoc_vec_sd_wrap;
    //        assoc_vec_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    //        assoc_vec_sd_wrap.deserialize(api_data);
    //        for(ChaosStringVectorIterator it = assoc_vec_sd_wrap().begin(),
    //            end = assoc_vec_sd_wrap().end();
    //            it != end;
    //            it++) {
    //            if((err = a_da->removeNodeAssociationForAgent(agent_uid, *it))) {
    //                LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error removing association for node %1% into agent %2% with error %3%", %*it%agent_uid%err));
    //            }
    //        }
    //    }
    return NULL;
}
