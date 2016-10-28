/*
 *	SubmitBatchCommand.cpp
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

#include "SubmitBatchCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::node;

#define CU_SBC_INFO INFO_LOG(SubmitBatchCommand)
#define CU_SBC_DBG  DBG_LOG(SubmitBatchCommand)
#define CU_SBC_ERR  ERR_LOG(SubmitBatchCommand)

DEFINE_MDS_COMAMND_ALIAS(SubmitBatchCommand)

SubmitBatchCommand::SubmitBatchCommand():
MDSBatchCommand() {}

SubmitBatchCommand::~SubmitBatchCommand() {}

// inherited method
void SubmitBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    CHECK_CDW_THROW_AND_LOG(data, CU_SBC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, "submission_task", CU_SBC_ERR, -2, "The attribute node unique id is mandatory")
    CU_SBC_INFO << data->getJSONString();
    command_instance.reset(data->getCSDataValue("submission_task"));
    
    CHECK_KEY_THROW_AND_LOG(command_instance.get(), NodeDefinitionKey::NODE_UNIQUE_ID, CU_SBC_ERR, -2, "The attribute node unique id is mandatory")
    
    const std::string node_uid = command_instance->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //fetch data for sending message to node id
    
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(node_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(CU_SBC_ERR, err, "Error loading infomation for node '%1%'", %node_uid)
    } else if(!tmp_ptr) {
        LOG_AND_TROW_FORMATTED(CU_SBC_ERR, -1, "No description found for node '%1%'", %node_uid)
    }
    
    std::auto_ptr<CDataWrapper> node_description(tmp_ptr);
    const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
    request = createRequest(node_rpc_address,
                            node_rpc_domain,
                            ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET);
}

// inherited method
void SubmitBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        command_instance.get());
            BC_END_RUNNING_PROPERTY
            break;
        }
            
        case MESSAGE_PHASE_SENT:
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
            break;
        default:
            break;
    }
}

// inherited method
void SubmitBatchCommand::ccHandler() {
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
bool SubmitBatchCommand::timeoutHandler() {
    return MDSBatchCommand::timeoutHandler();
}