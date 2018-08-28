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
#include "UnitServerAckBatchCommand.h"

#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::unit_server;

using namespace chaos::service_common::data::script;

#define USAC_INFO INFO_LOG(UnitServerAckCommand)
#define USAC_DBG  DBG_LOG(UnitServerAckCommand)
#define USAC_ERR  ERR_LOG(UnitServerAckCommand)

DEFINE_MDS_COMAMND_ALIAS(UnitServerAckCommand)

UnitServerAckCommand::UnitServerAckCommand():
MDSBatchCommand(),
us_can_start(false),
phase(USAP_ACK_US){
    list_autoload_cu_current = list_autoload_cu.end();
}

UnitServerAckCommand::~UnitServerAckCommand() {}

// inherited method
void UnitServerAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    //override default schedule time for this command
    //setFeatures(chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000);
    if(data->hasKey(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)) {
        us_can_start = data->getInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT) == ErrorCode::EC_MDS_NODE_REGISTRATION_OK;
    }
    CHECK_CDW_THROW_AND_LOG(data, USAC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, USAC_ERR, -2, "The unique id of unit server is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_ADDR, USAC_ERR, -3, "The rpc address of unit server is mandatory")
    //CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_DOMAIN, USAC_ERR, -4, "The rpc domain of unit server is mandatory")
    
    unit_server_uid = data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    message_data.reset(new CDataWrapper(data->getBSONRawData()));
    destination_address = message_data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
    request = createRequest(destination_address,
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_REG_ACK);
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
                    sendMessage(*request,
                                MOVE(message_data));
                    break;
                case MESSAGE_PHASE_SENT:
                    manageRequestPhase(*request);
                    break;
                    
                case MESSAGE_PHASE_COMPLETED:{
                    //after terminate the us ack try to fetch cu autoload
                    if(us_can_start) {
                        phase = USAP_CU_FECTH_NEXT;
                    } else {
                        BC_END_RUNNING_PROPERTY;
                    }
                    break;
                }
                    
                case MESSAGE_PHASE_TIMEOUT:
                    //terminate job
                    BC_END_RUNNING_PROPERTY
                    break;
            }
            break;
        }
        case USAP_CU_FECTH_NEXT: {
            if(list_autoload_cu_current == list_autoload_cu.end()) {
                list_autoload_cu.clear();
                //get next autoload cu for unit server
                if(!(err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getControlUnitWithAutoFlag(unit_server_uid,
                                                                                                               chaos::metadata_service::persistence::AUTO_LOAD,
                                                                                                               last_worked_cu.seq,
                                                                                                               list_autoload_cu))) {
                    if(list_autoload_cu.size() == 0) {
                        //terminate job
                        BC_END_RUNNING_PROPERTY
                    } else {
                        //we need to check if
                        list_autoload_cu_current = list_autoload_cu.begin();
                        if((err = prepareInstance())) {
                            BC_END_RUNNING_PROPERTY
                        } else {
                            phase = USAP_CU_AUTOLOAD;
                        }
                        break;
                    }
                } else {
                    USAC_ERR << "Error fetching the autoload control unit with code:" << err;
                    BC_END_RUNNING_PROPERTY
                    break;
                }
            } else {
                if(++list_autoload_cu_current == list_autoload_cu.end()) {
                    //whe have reached the end of fetched cu so we need to fetch new one page
                    break;
                }else {
                    if((err = prepareInstance())) {
                        BC_END_RUNNING_PROPERTY
                    } else {
                        phase = USAP_CU_AUTOLOAD;
                    }
                }
                break;
            }
        }
            
        case USAP_CU_AUTOLOAD: {
            switch(request->phase) {
                case MESSAGE_PHASE_UNSENT: {
                    sendMessage(*request,
                                MOVE(autoload_pack));
                    break;
                }
                    
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
                    BC_END_RUNNING_PROPERTY
                    USAC_ERR << "Whe have had tomeout error on load a control unit, the job will terminate becaus ethe unit serve ca be down";
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

int UnitServerAckCommand::prepareInstance() {
    int err = 0;
    bool found = false;
    ScriptBaseDescription sbd;
    last_worked_cu = *list_autoload_cu_current;
    //befor send if need to check if the current instsance is a script
    USAC_INFO << CHAOS_FORMAT("Send load data to %1%", %last_worked_cu.node_uid);
    if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getScriptAssociatedToControlUnitInstance(last_worked_cu.node_uid,
                                                                                                                found,
                                                                                                                sbd))){
        USAC_ERR << CHAOS_FORMAT("Error fetching the script associated to the control unit instance %1% with error %2%", %last_worked_cu.node_uid%err);
    } else if(found) {
        //in this case we need to copy the script data to the control unit instances
        if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->copyScriptDatasetAndContentToInstance(sbd,
                                                                                                            last_worked_cu.node_uid))){
            USAC_ERR << CHAOS_FORMAT("Error copying the script data to the control unit instance %1% with error %2%", %last_worked_cu.node_uid%err);
        }
    }
    
    //prepare load data pack to sento to control unit
    autoload_pack = CUCommonUtility::prepareRequestPackForLoadControlUnit(last_worked_cu.node_uid,
                                                                          getDataAccess<mds_data_access::ControlUnitDataAccess>());
    if(autoload_pack.get() == NULL){
        err = -1;
        USAC_ERR << "Error creating autoload datapack for:"<<last_worked_cu.node_uid<<" with code:" << err;
    } else {
        USAC_INFO << "Autoload control unit " << last_worked_cu.node_uid;
        request = createRequest(destination_address,
                                UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT);
        //prepare auto init and autostart message into autoload pack
        try{
            CUCommonUtility::prepareAutoInitAndStartInAutoLoadControlUnit(last_worked_cu.node_uid,
                                                                          getDataAccess<mds_data_access::NodeDataAccess>(),
                                                                          getDataAccess<mds_data_access::ControlUnitDataAccess>(),
                                                                          getDataAccess<mds_data_access::DataServiceDataAccess>(),
                                                                          autoload_pack.get());
        } catch(CException& ex) {
            getDataAccess<persistence::data_access::LoggingDataAccess>()->logException(last_worked_cu.node_uid,
                                                                                       "[MDS]UnitServerAckCommand",
                                                                                       "error",
                                                                                       ex);
            err = -2;
        } catch(...) {
            getDataAccess<persistence::data_access::LoggingDataAccess>()->logError(last_worked_cu.node_uid,
                                                                                   "[MDS]UnitServerAckCommand",
                                                                                   "error",
                                                                                   -3,
                                                                                   "Undefined error",
                                                                                   __PRETTY_FUNCTION__);
        }
    }
    return err;
}
