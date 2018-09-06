/*
 * Copyright 2012, 05/02/2018 INFN
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
#include "SendRpcCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::node;

#define INFO INFO_LOG(SendRpcCommand)
#define DBG  DBG_LOG(SendRpcCommand)
#define ERR  ERR_LOG(SendRpcCommand)

DEFINE_MDS_COMAMND_ALIAS(SendRpcCommand)

SendRpcCommand::SendRpcCommand():
MDSBatchCommand() {}

SendRpcCommand::~SendRpcCommand() {}

// inherited method
void SendRpcCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    CDWUniquePtr node_description;
    
    CHECK_ASSERTION_THROW_AND_LOG(data!=NULL, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The attribute %1% is mandatory",%NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The attribute %1% need to be string",%NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CHECK_KEY_THROW_AND_LOG(data, RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, ERR, -6, CHAOS_FORMAT("The attribute %1% is mandatory",%RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN));
    CHAOS_LASSERT_EXCEPTION(data->isStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME), ERR, -7, CHAOS_FORMAT("The attribute %1% need to be string",%RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN));
    
    bool node_alive;
    CDWUniquePtr rpc_message;
    const std::string node_uid = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string rpc_action = data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
    
    if(data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE) &&
       data->isCDataWrapperValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)) {
        rpc_message=data->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
    }
    
    if(getDataAccess<mds_data_access::NodeDataAccess>()->isNodeAlive(node_uid, node_alive)) {
        INFO << CHAOS_FORMAT("The node %1% is offline so the rpc message will not be forwarded!", %node_uid);
        BC_END_RUNNING_PROPERTY
        
    } else {
        //node is olnie so we cann proceed
        if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(node_uid, node_description))) {
            LOG_AND_TROW_FORMATTED(ERR, err, "Error loading infomation for node '%1%'", %node_uid)
        } else if(node_description.get() == NULL) {
            LOG_AND_TROW_FORMATTED(ERR, -1, "No description found for node '%1%'", %node_uid)
        }
        
        const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
        const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
        request = createRequest(node_rpc_address,
                                node_rpc_domain,
                                rpc_action);
        sendMessage(*request,
                    MOVE(rpc_message));
    }
}

// inherited method
void SendRpcCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void SendRpcCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:
            break;
            
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            BC_END_RUNNING_PROPERTY
            break;
            
        default:
            break;
        }
    }
}

// inherited method
bool SendRpcCommand::timeoutHandler() {
    return MDSBatchCommand::timeoutHandler();
}
