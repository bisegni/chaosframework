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

#include "AgentLoggingNodeManagement.h"

#include "../../common/CUCommonUtility.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::service_common::data::agent;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::agent;

#define INFO INFO_LOG(AgentLoggingNodeManagement)
#define DBG  DBG_LOG(AgentLoggingNodeManagement)
#define ERR  ERR_LOG(AgentLoggingNodeManagement)

DEFINE_MDS_COMAMND_ALIAS(AgentLoggingNodeManagement)

AgentLoggingNodeManagement::AgentLoggingNodeManagement():
MDSBatchCommand(),
managed_node(){}

AgentLoggingNodeManagement::~AgentLoggingNodeManagement() {}

// inherited method
void AgentLoggingNodeManagement::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    CHECK_ASSERTION_THROW_AND_LOG((data!=NULL), ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    CHECK_KEY_THROW_AND_LOG(data, "NodeAssociationLoggingOperation", ERR, -3, CHAOS_FORMAT("The %1% key is mandatory",%"NodeAssociationLoggingOperation"));
    int err = 0;
    //agent that host the node
    std::string agent_uid;
    managed_node = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    NodeAssociationLoggingOperation logging_action = (NodeAssociationLoggingOperation)data->getInt32Value("NodeAssociationLoggingOperation");
    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->getAgentForNode(managed_node, agent_uid))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error fetching agent uid for node decription for %1%", %managed_node))
    }
    
    //! fetch the agent information
    CDataWrapper *tmp_ptr;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> agent_node_information;
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(agent_uid, &tmp_ptr))) {
        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error fetching agent decription for %1%", %agent_uid))
    }
    agent_node_information.reset(tmp_ptr);
    
    
    AgentAssociationSDWrapper node_assoc_sdw;
    if((err = getDataAccess<mds_data_access::AgentDataAccess>()->loadNodeAssociationForAgent(agent_uid,
                                                                                             managed_node, node_assoc_sdw()))) {
        LOG_AND_TROW(ERR, -3, CHAOS_FORMAT("Error loading the association for agent %1% and node %2% with error %3%", %agent_uid%managed_node%err));
    }
    
    //add node association description
    message_data.reset(new CDataWrapper());
    message_data->addCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, *node_assoc_sdw.serialize());
    switch(logging_action) {
        case NodeAssociationLoggingOperationEnable:
            request = createRequest(agent_node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::LogWorker::ACTION_START_LOGGING_ASSOCIATION);
            break;
        case NodeAssociationLoggingOperationDisable:
            request = createRequest(agent_node_information->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                    AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN,
                                    AgentNodeDomainAndActionRPC::LogWorker::ACTION_STOP_LOGGING_ASSOCIATION);
            break;
        default: {
            LOG_AND_TROW(ERR, -4, CHAOS_FORMAT("Invalid logging mode for %1%", %managed_node));
        }
    }
;
}

// inherited method
void AgentLoggingNodeManagement::acquireHandler() {
    
}

// inherited method
void AgentLoggingNodeManagement::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendRequest(*request,
                        MOVE(message_data));
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
        default:
            BC_END_RUNNING_PROPERTY;
            break;
    }
}

// inherited method
bool AgentLoggingNodeManagement::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
