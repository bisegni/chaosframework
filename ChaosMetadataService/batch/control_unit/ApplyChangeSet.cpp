/*
 *	ApplyChangeSet.cpp
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

#include "ApplyChangeSet.h"


using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_ACS_BC_INFO INFO_LOG(ApplyChangeSet)
#define CU_ACS_BC_DBG  DBG_LOG(ApplyChangeSet)
#define CU_ACS_BC_ERR  ERR_LOG(ApplyChangeSet)

DEFINE_MDS_COMAMND_ALIAS(ApplyChangeSet)


ApplyChangeSet::ApplyChangeSet():
MDSBatchCommand(),
retry_number(0),
message_channel(NULL),
cu_id(),
cu_rpc_addr(),
cu_rpc_dom(),
phase(IDSTPhase_SEND_MESSAGE){
    //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
    //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
    
}

ApplyChangeSet::~ApplyChangeSet() {
    if(message_channel) {
        CU_ACS_BC_INFO << "Release message channel";
        releaseChannel(message_channel);
    }
}

// inherited method
void ApplyChangeSet::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    cu_rpc_addr = data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    cu_rpc_dom = data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
    
    message_channel = getNewMessageChannel();
    if(!message_channel) {
        CU_ACS_BC_ERR << "No Message channel";
        throw chaos::CException(-5, "No Message channel", __PRETTY_FUNCTION__);
    }
    //set the phase to send message
    phase = IDSTPhase_SEND_MESSAGE;
    message = data;
}

// inherited method
void ApplyChangeSet::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void ApplyChangeSet::ccHandler() {
    MDSBatchCommand::ccHandler();
    retry_number++;
    switch(phase) {
        case IDSTPhase_SEND_MESSAGE: {
        
            //send message for action
            request_future = message_channel->sendRequestWithFuture(cu_rpc_addr,
                                                                    cu_rpc_dom,
                                                                    ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                                                    message);
            
            CU_ACS_BC_DBG << "Call 'setDatasetAttribute' action of to control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
            phase = IDSTPhase_WAIT_ANSWER;
        }
        case IDSTPhase_WAIT_ANSWER: {
            CU_ACS_BC_DBG << "Waith for 'setDatasetAttribute' action ack from the control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
            if(!request_future.get()) {
                CU_ACS_BC_ERR << "request with no future";
                throw chaos::CException(-1, "request with no future", __PRETTY_FUNCTION__);
            }
            if(request_future->wait(1000)) {
                //we have hd answer
                if(request_future->getError()) {
                    CU_ACS_BC_ERR << "We have had answer with error"
                    << request_future->getError() << " \n and message "
                    << request_future->getErrorMessage() << "\n on domain "
                    << request_future->getErrorDomain();
                } else {
                    CU_ACS_BC_DBG << "Request send with success";
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
bool ApplyChangeSet::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    CU_ACS_BC_DBG << " Time out reached for "<< ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET <<" action ack from the control unit:" << cu_id << " on rpc[" << cu_rpc_addr <<":"<<cu_rpc_dom<<"]";
    return result;
}