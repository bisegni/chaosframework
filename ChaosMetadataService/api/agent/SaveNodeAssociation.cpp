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

#include "SaveNodeAssociation.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(SaveNodeAssociation)
#define ERR  DBG_LOG(SaveNodeAssociation)
#define DBG  ERR_LOG(SaveNodeAssociation)

using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

SaveNodeAssociation::SaveNodeAssociation():
AbstractApi("saveNodeAssociation"){}

SaveNodeAssociation::~SaveNodeAssociation() {}

CDWUniquePtr SaveNodeAssociation::execute(CDWUniquePtr api_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The key %1% need to be a string", %NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CHECK_KEY_THROW_AND_LOG(api_data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERR, -4, CHAOS_FORMAT("The key %1% is mandatory", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    CHAOS_LASSERT_EXCEPTION((api_data->isCDataWrapperValue(AgentNodeDefinitionKey::NODE_ASSOCIATED) || api_data->isVectorValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)), ERR, -5, CHAOS_FORMAT("The key %1% need to be an object or a vector", %AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    GET_DATA_ACCESS(AgentDataAccess, a_da, -6);
    
    int err = 0;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    std::string current_agent;
    
    if(api_data->isCDataWrapperValue(AgentNodeDefinitionKey::NODE_ASSOCIATED)) {
        AgentAssociationSDWrapper assoc_sd_wrapper;
        ChaosUniquePtr<chaos::common::data::CDataWrapper> assoc_ser(api_data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
        assoc_sd_wrapper.deserialize(assoc_ser.get());
        //check if associated node is assocaited no antother agent
        if((err = a_da->getAgentForNode(assoc_sd_wrapper().associated_node_uid, current_agent))) {
            LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error finding the agent for the node %1% with error %2%", %assoc_sd_wrapper().associated_node_uid%err));
        } else if(current_agent.size() > 0 &&
                  current_agent.compare(agent_uid) != 0) {
            LOG_AND_TROW(ERR, -8, CHAOS_FORMAT("The node %1% is associated to the agent %2% and can't be assocaited to %3%", %assoc_sd_wrapper().associated_node_uid%current_agent%agent_uid));
        }
            
        if((err = a_da->saveNodeAssociationForAgent(agent_uid, assoc_sd_wrapper()))) {
            LOG_AND_TROW(ERR, -9, CHAOS_FORMAT("Error saving association for node %1% into agent %2% with error %3%", %assoc_sd_wrapper().associated_node_uid%agent_uid%err));
        }
    } else {
        VectorAgentAssociationSDWrapper associationList_sd_wrap;
        associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
        associationList_sd_wrap.deserialize(api_data);
        for(std::vector<AgentAssociation>::iterator it = associationList_sd_wrap().begin(),
            end = associationList_sd_wrap().end();
            it != end;
            it++) {
            current_agent.clear();
            //check if associated node is assocaited no antother agent
            if((err = a_da->getAgentForNode(it->associated_node_uid, current_agent))) {
                LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error finding the agent for the node %1% with error %2%", %it->associated_node_uid%err));
            } else if(current_agent.size() > 0 &&
                      current_agent.compare(agent_uid) != 0) {
                LOG_AND_TROW(ERR, -8, CHAOS_FORMAT("The node %1% is associated to the agent %2% and can't be assocaited to %3%", %it->associated_node_uid%current_agent%agent_uid));
            }
            if((err = a_da->saveNodeAssociationForAgent(agent_uid, *it))) {
                LOG_AND_TROW(ERR, -9, CHAOS_FORMAT("Error saving association for node %1% into agent %2% with error %3%", %it->associated_node_uid%agent_uid%err));
            }
        }
    }
    return NULL;
}
