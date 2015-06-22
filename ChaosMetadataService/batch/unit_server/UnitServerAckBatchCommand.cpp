/*
 *	UnitServerAckCommand.cpp
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
#include "UnitServerAckBatchCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::unit_server;

#define USAC_INFO INFO_LOG(UnitServerAckCommand)
#define USAC_DBG  DBG_LOG(UnitServerAckCommand)
#define USAC_ERR  ERR_LOG(UnitServerAckCommand)

DEFINE_MDS_COMAMND_ALIAS(UnitServerAckCommand)

UnitServerAckCommand::UnitServerAckCommand():
MDSBatchCommand(),
message_data(NULL){
    //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
    //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}

UnitServerAckCommand::~UnitServerAckCommand() {}

// inherited method
void UnitServerAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    if(data  && data->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        request = createRequest(data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_REG_ACK);
        message_data = data;
    } else throw chaos::CException(-1, "No unit server address has been set", __PRETTY_FUNCTION__);
    
}

// inherited method
void UnitServerAckCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void UnitServerAckCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:
            sendRequest(*request,
                        message_data);
            break;
            
        case MESSAGE_PHASE_SENT:
            manageRequestPhase(*request);
            break;
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:{
            BC_END_RUNNIG_PROPERTY
            break;
        }
    }
}

// inherited method
bool UnitServerAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}