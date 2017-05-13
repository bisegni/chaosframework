/*
 *	AgentProcessController.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 13/02/2017 INFN, National Institute of Nuclear Physics
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

#include "AgentProcessController.h"

#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::agent;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::agent;

#define INFO INFO_LOG(AgentProcessController)
#define DBG  DBG_LOG(AgentProcessController)
#define ERR  ERR_LOG(AgentProcessController)

DEFINE_MDS_COMAMND_ALIAS(AgentProcessController)

AgentProcessController::AgentProcessController():
MDSBatchCommand(),
process_op(NodeAssociationOperationUndefined){}

AgentProcessController::~AgentProcessController() {}

// inherited method
void AgentProcessController::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    CHECK_CDW_THROW_AND_LOG(data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The unique id of unit server is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, "node_operation", ERR, -2, "The process_oepration is a mandatory key")
    
    int err = 0;
    std::string agent_host;
    process_op = (NodeAssociationOperation)data->getInt32Value("node_operation");
    const std::string node_to_launch = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //get the agent that host the node whre execute the ooperation
    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->getAgentForNode(node_to_launch, agent_host))) {
        LOG_AND_TROW(ERR, -3, CHAOS_FORMAT("Error finding the agent for the node %1% with error %2%", %node_to_launch%err));
    } else if (agent_host.size() == 0) {
        LOG_AND_TROW(ERR, -4, CHAOS_FORMAT("No agent found for node %1%", %node_to_launch));
    }
    
    //! fetch the agent information
    CDataWrapper *tmp_ptr;
    std::unique_ptr<CDataWrapper> node_information;
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(agent_host, &tmp_ptr))) {
        LOG_AND_TROW(ERR, err, "Error fetching node decription")
    }
    node_information.reset(tmp_ptr);
    AgentAssociationSDWrapper assoc_sd_wrapper;
    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->loadNodeAssociationForAgent(agent_host, node_to_launch, assoc_sd_wrapper()))) {
        LOG_AND_TROW(ERR, -4, CHAOS_FORMAT("Error loading the association for node %1% on agent %2% with error %3%", %node_to_launch%agent_host%err));
    }
    message_data = assoc_sd_wrapper.serialize();
    switch(process_op) {
        case NodeAssociationOperationLaunch: {
            request = createRequest(node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE);
            break;
        }
        case NodeAssociationOperationStop:
            request = createRequest(node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE);
            break;
        case NodeAssociationOperationKill:
            //set kill attribute
            message_data->addBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL, true);
            request = createRequest(node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE);
            break;
        case NodeAssociationOperationRestart:
            request = createRequest(node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE);
            break;
            
        default: {
            LOG_AND_TROW(ERR, -5, CHAOS_FORMAT("Invalid operation[%1%] to execute on process %2% through the agent %3%", %process_op%node_to_launch%agent_host));
        }
    }
}

// inherited method
void AgentProcessController::acquireHandler() {
    
}

// inherited method
void AgentProcessController::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        message_data.get());
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
bool AgentProcessController::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
