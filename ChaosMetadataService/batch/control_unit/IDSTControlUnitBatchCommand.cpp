/*
 *	IDSTControlUnitBatchCommand.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include "IDSTControlUnitBatchCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_IDST_BC_INFO INFO_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_DBG  DBG_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_ERR  ERR_LOG(IDSTControlUnitBatchCommand)

DEFINE_MDS_COMAMND_ALIAS(IDSTControlUnitBatchCommand)


IDSTControlUnitBatchCommand::IDSTControlUnitBatchCommand():
MDSBatchCommand(),
retry_number(0),
message_channel(NULL),
cu_id(),
cu_rpc_addr(),
cu_rpc_dom(),
action(ACTION_INIT) {
        //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
        //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);

}

IDSTControlUnitBatchCommand::~IDSTControlUnitBatchCommand() {
    if(message_channel) {
        CU_IDST_BC_INFO << "Release message channel";
        releaseChannel(message_channel);
    }
}

    // inherited method
void IDSTControlUnitBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);

        //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    cu_rpc_addr = data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    cu_rpc_dom = data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
    action = static_cast<IDSTAction>(data->getInt32Value("action"));//0-init,1-start,2-ztop,3-deinit

    message_channel = getNewMessageChannel();
    if(!message_channel) {
        CU_IDST_BC_ERR << "No Message channel";
        throw chaos::CException(-5, "No Message channel", __PRETTY_FUNCTION__);
    }
        //set the phase to send message
    phase = PHASE_SEND_MESSAGE;
    message = data;
}

    // inherited method
void IDSTControlUnitBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

    // inherited method
void IDSTControlUnitBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    retry_number++;
    switch(phase) {
        case PHASE_SEND_MESSAGE: {
            switch(action) {
                case ACTION_INIT:
                    action_to_call = NodeDomainAndActionRPC::ACTION_NODE_INIT;
                    break;
                case ACTION_START:
                    action_to_call = NodeDomainAndActionRPC::ACTION_NODE_START;
                    break;
                case ACTION_STOP:
                    action_to_call = NodeDomainAndActionRPC::ACTION_NODE_STOP;
                    break;
                case ACTION_DEINIT:
                    action_to_call = NodeDomainAndActionRPC::ACTION_NODE_DEINIT;
                    break;
            }

                //send message for action
            request_future = message_channel->sendRequestWithFuture(cu_rpc_addr,
                                                                    cu_rpc_dom,
                                                                    action_to_call,
                                                                    message);

            CU_IDST_BC_DBG << "Call "<< action_to_call << " action of to control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
            phase = PHASE_WAIT_ANSWER;
        }
        case PHASE_WAIT_ANSWER: {
            CU_IDST_BC_DBG << "Waith for "<< action_to_call << " action ack from the control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
            if(!request_future.get()) {
                CU_IDST_BC_ERR << "request with no future";
                throw chaos::CException(-1, "request with no future", __PRETTY_FUNCTION__);
            }
            if(request_future->wait(1000)) {
                    //we have hd answer
                if(request_future->getError()) {
                    CU_IDST_BC_ERR << "We have had answer with error"
                    << request_future->getError() << " \n and message "
                    << request_future->getErrorMessage() << "\n on domain "
                    << request_future->getErrorDomain();
                } else {
                    CU_IDST_BC_DBG << "Request send with success";
                }
                BC_END_RUNNIG_PROPERTY
            }
            break;
        }
    }
    if((retry_number % 3) == 0) {
        BC_END_RUNNIG_PROPERTY
    }
}
    // inherited method
bool IDSTControlUnitBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    CU_IDST_BC_DBG << " Time out reached for "<< action_to_call <<" action ack from the control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
    return result;
}