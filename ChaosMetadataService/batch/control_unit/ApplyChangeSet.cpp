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