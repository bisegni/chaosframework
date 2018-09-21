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
MDSBatchCommand(){}

AgentAckCommand::~AgentAckCommand() {}

// inherited method
void AgentAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_ASSERTION_THROW_AND_LOG((data!= NULL), ERR, -1, "No parameter found")
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
    message_data.reset(new CDataWrapper(data->getBSONRawData()));
}

// inherited method
void AgentAckCommand::acquireHandler() {
    
}

// inherited method
void AgentAckCommand::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        MOVE(message_data));
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
