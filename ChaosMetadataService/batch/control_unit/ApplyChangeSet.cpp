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
cu_id(){}

ApplyChangeSet::~ApplyChangeSet() {}

// inherited method
void ApplyChangeSet::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    //set the phase to send message
    request = createRequest(data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR),
                            data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN),
                            ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET);
    message = data;
}

// inherited method
void ApplyChangeSet::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            
            //send message for action
            sendMessage(*request,
                        message);
            BC_END_RUNNING_PROPERTY
            break;
        }
        case MESSAGE_PHASE_SENT:
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
            break;
    }
}

// inherited method
void ApplyChangeSet::ccHandler() {
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
        }
    }
}
// inherited method
bool ApplyChangeSet::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}