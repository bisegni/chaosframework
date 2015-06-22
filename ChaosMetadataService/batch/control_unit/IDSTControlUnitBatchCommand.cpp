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
cu_id(),
action(ACTION_INIT) {
        //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
        //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);

}

IDSTControlUnitBatchCommand::~IDSTControlUnitBatchCommand() {
}

    // inherited method
void IDSTControlUnitBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);

        //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    action = static_cast<IDSTAction>(data->getInt32Value("action"));//0-init,1-start,2-ztop,3-deinit

    request = createRequest(data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR),
                            data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN),
                            "");
    message = data;
}

    // inherited method
void IDSTControlUnitBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

    // inherited method
void IDSTControlUnitBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            switch(action) {
                case ACTION_INIT:
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_INIT;
                    break;
                case ACTION_START:
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_START;
                    break;
                case ACTION_STOP:
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_STOP;
                    break;
                case ACTION_DEINIT:
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_DEINIT;
                    break;
            }

                //send message for action
            sendRequest(*request,
                        message);
            break;
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            BC_END_RUNNIG_PROPERTY
            break;
        }
    }
}
    // inherited method
bool IDSTControlUnitBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}