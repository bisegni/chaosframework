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
message_data(NULL),
phase(USAP_ACK_US){
    //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)100000);
    //set the timeout to 10 seconds
    //setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}

UnitServerAckCommand::~UnitServerAckCommand() {}

// inherited method
void UnitServerAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_CDW_THROW_AND_LOG(data, USAC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, USAC_ERR, -2, "The unique id of unit server is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_ADDR, USAC_ERR, -3, "The rpc address of unit server is mandatory")
    //CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_DOMAIN, USAC_ERR, -4, "The rpc domain of unit server is mandatory")
    
    unit_server_uid = data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    request = createRequest(data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_REG_ACK);
    message_data = data;
}

// inherited method
void UnitServerAckCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(phase) {
        case USAP_ACK_US:
            break;
        case USAP_CU_AUTOLOAD:
            break;
        case USAP_CU_FECTH_NEXT:
            break;
        case USAP_END:
            break;
    }
}

// inherited method
void UnitServerAckCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    int err = 0;
    switch(phase) {
        case USAP_ACK_US: {
            switch(request->phase) {
                case MESSAGE_PHASE_UNSENT:
                    sendRequest(*request,
                                message_data);
                    break;
                    
                case MESSAGE_PHASE_SENT:
                    manageRequestPhase(*request);
                    break;
                    
                case MESSAGE_PHASE_COMPLETED:{
                    //after terminate the control unit ack try to fetch cu autoload
                    phase = USAP_CU_FECTH_NEXT;
                    break;
                }
                    
                case MESSAGE_PHASE_TIMEOUT:
                    //terminate job
                    BC_END_RUNNIG_PROPERTY
                    break;
            }
            break;
        }
        case USAP_CU_FECTH_NEXT: {
            if(list_autoload_cu_current == list_autoload_cu.end()) {
                if(!(err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getControlUnitWithAutoFlag(unit_server_uid,
                                                                                                               chaos::metadata_service::persistence::AUTO_LOAD,
                                                                                                               0,
                                                                                                               list_autoload_cu))) {
                    
                    if(list_autoload_cu.size() == 0) {
                        //terminate job
                        BC_END_RUNNIG_PROPERTY
                    } else {
                        //we need to check if
                        list_autoload_cu_current = list_autoload_cu.begin();
                        phase = USAP_CU_AUTOLOAD;
                        
                        //terminate for testing
                        BC_END_RUNNIG_PROPERTY
                        
                        break;
                    }
                } else {
                    break;
                }
            } else {
                list_autoload_cu_current++;
                phase = USAP_CU_AUTOLOAD;
                
                //terminate for testing
                BC_END_RUNNIG_PROPERTY
                
                break;
            }
        }
            
        case USAP_CU_AUTOLOAD: {
            switch(request->phase) {
                case MESSAGE_PHASE_UNSENT:
                    sendRequest(*request,
                                message_data);
                    break;
                    
                case MESSAGE_PHASE_SENT:
                    manageRequestPhase(*request);
                    break;
                    
                case MESSAGE_PHASE_COMPLETED:{
                    //after terminate the control unit ack try to fetch cu autoload
                    phase = USAP_CU_FECTH_NEXT;
                    break;
                }
                    
                case MESSAGE_PHASE_TIMEOUT:
                    //terminate job
                    BC_END_RUNNIG_PROPERTY
                    break;
            }
            
            break;
        }
        
        case USAP_END: {
            break;
        }
    }
    
}

// inherited method
bool UnitServerAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}