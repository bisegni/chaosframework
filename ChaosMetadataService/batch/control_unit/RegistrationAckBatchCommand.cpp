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

#include "RegistrationAckBatchCommand.h"

#include "../control_unit/IDSTControlUnitBatchCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_RACK_INFO INFO_LOG(RegistrationAckBatchCommand)
#define CU_RACK_DBG  DBG_LOG(RegistrationAckBatchCommand)
#define CU_RACK_ERR  ERR_LOG(RegistrationAckBatchCommand)

DEFINE_MDS_COMAMND_ALIAS(RegistrationAckBatchCommand)

static const char * const RegistrationAckBatchCommand_NO_UID = "No unique id found";
static const char * const RegistrationAckBatchCommand_NO_RPC_ADDR = "No rpc address id found";
static const char * const RegistrationAckBatchCommand_NO_RPC_DOM = "No rpc domain id found";
static const char * const RegistrationAckBatchCommand_NO_RESULT_FOUND = "No ack result found";

RegistrationAckBatchCommand::RegistrationAckBatchCommand():
MDSBatchCommand(){}
RegistrationAckBatchCommand::~RegistrationAckBatchCommand() {}

// inherited method
void RegistrationAckBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    //set cu id to the batch command datapack
    if(!data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-1, RegistrationAckBatchCommand_NO_UID, __PRETTY_FUNCTION__);
    if(!data->hasKey(NodeDefinitionKey::NODE_RPC_ADDR)) throw CException(-2, RegistrationAckBatchCommand_NO_RPC_ADDR, __PRETTY_FUNCTION__);
    if(!data->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) throw CException(-3, RegistrationAckBatchCommand_NO_RPC_DOM, __PRETTY_FUNCTION__);
    if(!data->hasKey(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)) throw CException(-4, RegistrationAckBatchCommand_NO_RESULT_FOUND, __PRETTY_FUNCTION__);
    
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    unit_server_addr = data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    reg_result = data->getInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT);
    request = createRequest(unit_server_addr,
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            UnitServerNodeDomainAndActionRPC::ACTION_REGISTRATION_CONTROL_UNIT_ACK);
}

// inherited method
void RegistrationAckBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            CU_RACK_DBG << "Send ack to control unit:" << cu_id << " on ip:" << unit_server_addr;
            CDataWrapper message;
            message.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
            message.addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT, reg_result);
            sendMessage(*request,
                        &message);
            BC_END_RUNNING_PROPERTY
        }
        default:
            break;
    }
}

// inherited method
void RegistrationAckBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    
    switch(request->phase) {
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
bool RegistrationAckBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}