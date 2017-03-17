/*
 *	AgentAckCommand.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#include "AgentAckCommand.h"

#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::agent;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::agent;

#define INFO INFO_LOG(AgentAckCommand)
#define DBG  DBG_LOG(AgentAckCommand)
#define ERR  ERR_LOG(AgentAckCommand)

DEFINE_MDS_COMAMND_ALIAS(AgentAckCommand)

AgentAckCommand::AgentAckCommand():
MDSBatchCommand(),
message_data(NULL){}

AgentAckCommand::~AgentAckCommand() {}

// inherited method
void AgentAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_CDW_THROW_AND_LOG(data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The unique id of unit server is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_ADDR, ERR, -3, "The rpc address of unit server is mandatory")
    
    int err = 0;
    AgentInstanceSDWrapper agent_instance_sd_wrapper;
    
    node_uid = data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);

    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->loadAgentDescription(node_uid,
                                                                                      true,
                                                                                      agent_instance_sd_wrapper()))) {
        LOG_AND_TROW(ERR, -4, CHAOS_FORMAT("Error loading the full agent description for node %1% error %2%", %node_uid%err));
    }
    
    data->addCSDataValue("agent_description", *agent_instance_sd_wrapper.serialize());
    request = createRequest(data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                            AgentNodeDomainAndActionRPC::RPC_DOMAIN_,
                            AgentNodeDomainAndActionRPC::ACTION_AGENT_REGISTRATION_ACK);
    message_data = data;
}

// inherited method
void AgentAckCommand::acquireHandler() {
    
}

// inherited method
void AgentAckCommand::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        message_data);
        }
            
        case MESSAGE_PHASE_SENT:
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
        default:
            BC_END_RUNNING_PROPERTY
            break;
    }
}

// inherited method
bool AgentAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
