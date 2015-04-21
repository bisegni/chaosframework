//
//  RegistrationAckBatchCommand.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/04/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "RegistrationAckBatchCommand.h"

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
MDSBatchCommand(),
retry_number(0),
message_channel(NULL){
        //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
        //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);

}
RegistrationAckBatchCommand::~RegistrationAckBatchCommand() {
    if(message_channel) {
        CU_RACK_INFO << "Release message channel";
        releaseChannel(message_channel);
    }
}

    // inherited method
void RegistrationAckBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);

        //set cu id to the batch command datapack
    if(!data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-1, RegistrationAckBatchCommand_NO_UID, __PRETTY_FUNCTION__);
    if(!data->hasKey(NodeDefinitionKey::NODE_RPC_ADDR)) throw CException(-2, RegistrationAckBatchCommand_NO_RPC_ADDR, __PRETTY_FUNCTION__);
    if(!data->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) throw CException(-3, RegistrationAckBatchCommand_NO_RPC_DOM, __PRETTY_FUNCTION__);
    if(!data->hasKey(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)) throw CException(-4, RegistrationAckBatchCommand_NO_RESULT_FOUND, __PRETTY_FUNCTION__);

    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    si_addr = data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    reg_result = data->getInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT);

    message_channel = getNewMessageChannel();
    if(!message_channel) {
        CU_RACK_ERR << "No Message channel";
        throw chaos::CException(-5, "No Message channel", __PRETTY_FUNCTION__);
    }

    phase = RACK_SEND;
}

    // inherited method
void RegistrationAckBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(phase) {
        case RACK_SEND: {
            CU_RACK_DBG << "Send ack to control unit:" << cu_id << " on ip:" << si_addr;
            CDataWrapper message;
            message.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
            message.addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT, reg_result);
            request_future = message_channel->sendRequestWithFuture(si_addr,
                                                                    UnitServerNodeDomainAndActionLabel::RPC_DOMAIN,
                                                                    UnitServerNodeDomainAndActionLabel::ACTION_REGISTRATION_CONTROL_UNIT_ACK,
                                                                    &message);
            if(!request_future.get()) {
                CU_RACK_ERR << "request with no future";
                throw chaos::CException(-1, "request with no future", __PRETTY_FUNCTION__);
            } else {
                phase = RACK_ANSW_WAIT;
            }
            break;
        }

        default:
            break;
    }
}

    // inherited method
void RegistrationAckBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    retry_number++;

    switch(phase) {
        case RACK_SEND: {

            break;
        }

        case RACK_ANSW_WAIT: {
            if(request_future->wait(1000)) {
                    //we have hd answer
                if(request_future->getError()) {
                    CU_RACK_ERR << "We have had answer with error"
                    << request_future->getError() << " \n and message "
                    << request_future->getErrorMessage() << "\n on domain "
                    << request_future->getErrorDomain();
                } else {
                    CU_RACK_DBG << "Request send with success";
                }
                BC_END_RUNNIG_PROPERTY
            } else {
                    //we need to retry
                if((retry_number % 3) != 0) {
                    CU_RACK_DBG << "Retry counter";
                } else {
                    CU_RACK_DBG << "No more retry";
                    BC_END_RUNNIG_PROPERTY
                }
            }
            break;
        }
    }
    BC_END_RUNNIG_PROPERTY;
}

    // inherited method
bool RegistrationAckBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}