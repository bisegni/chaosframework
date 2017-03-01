/*
 *	ListNodeForAgent.cpp
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

#include "ListNodeForAgent.h"

#include <chaos/common/data/structured/Lists.h>

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(ListNodeForAgent)
#define ERR  DBG_LOG(ListNodeForAgent)
#define DBG  ERR_LOG(ListNodeForAgent)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

using namespace chaos::service_common::data::agent;

ListNodeForAgent::ListNodeForAgent():
AbstractApi(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LIST_NODE){
}

ListNodeForAgent::~ListNodeForAgent() {
}

CDataWrapper *ListNodeForAgent::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The key %1% need to be a string", %NodeDefinitionKey::NODE_UNIQUE_ID));
    
    //we can rpocessd
    GET_DATA_ACCESS(AgentDataAccess, a_da, -4);
    
    int err = 0;
    VectorAgentAssociationStatusSDWrapper association_status_vec_sd_wrap;
    association_status_vec_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if((err = a_da->getNodeListStatusForAgent(agent_uid, association_status_vec_sd_wrap()))) {
        LOG_AND_TROW(ERR, -5, CHAOS_FORMAT("Error loading association status for agent %1%",%agent_uid));
    }
    return association_status_vec_sd_wrap.serialize().release();
}
