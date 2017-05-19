/*
 *	LoadInstanceOnUnitServer.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/06/16 INFN, National Institute of Nuclear Physics
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

#include "LoadInstanceOnUnitServer.h"

#include "../control_unit/IDSTControlUnitBatchCommand.h"
#include "../../common/CUCommonUtility.h"


using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch::script;

#define INFO INFO_LOG(LoadInstanceOnUnitServer)
#define DBG  DBG_LOG(LoadInstanceOnUnitServer)
#define ERR  ERR_LOG(LoadInstanceOnUnitServer)

DEFINE_MDS_COMAMND_ALIAS(LoadInstanceOnUnitServer)

LoadInstanceOnUnitServer::LoadInstanceOnUnitServer():
MDSBatchCommand(),
last_sequence_id(0){}

LoadInstanceOnUnitServer::~LoadInstanceOnUnitServer() {}

// inherited method
void LoadInstanceOnUnitServer::setHandler(CDataWrapper *data) {
    int err = 0;
    MDSBatchCommand::setHandler(data);
    CDataWrapper *tmp_ptr = NULL;
    CHECK_CDW_THROW_AND_LOG(data, ERR, -1, "No parameter found");
    
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_PARENT, ERR, -2, "The node parent key for unit server is mandatory");
    CHAOS_LASSERT_EXCEPTION(data->isStringValue(chaos::NodeDefinitionKey::NODE_PARENT), ERR, -3, CHAOS_FORMAT("%1% key need to be a string with the unit server uid", %chaos::NodeDefinitionKey::NODE_PARENT));
    
    CHECK_KEY_THROW_AND_LOG(data, ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST, ERR, -4, "The execution pool list is mandatory");
    CHAOS_LASSERT_EXCEPTION(data->isVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST), ERR, -5, "ep_pool_list key need to be a vector with the execution pool list");
    
    unit_server = data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> array(data->getVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST));
    for(int idx = 0;
        idx < array->size();
        idx++) {
        epool_list.push_back(array->getStringElementAtIndex(idx));
    }
    
    err = getDataAccess<mds_data_access::UnitServerDataAccess>()->getDescription(unit_server, &tmp_ptr);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> unit_server_description(tmp_ptr);
    if(err){
        LOG_AND_TROW(ERR, err, "Error fetching unit server description");
    }
    
    if(unit_server_description.get() &&
       unit_server_description->hasKey(NodeDefinitionKey::NODE_RPC_ADDR)) {
        //set the send command phase
        unit_server_rpc_addr = unit_server_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
        search_script_phase = SearchScriptPhaseLoadScriptPage;
        BC_NORMAL_RUNNING_PROPERTY
    } else {
        DBG << "we have no execution pool so we terminate the command here";
        BC_END_RUNNING_PROPERTY
    }
    
    //for debug
    setFeatures(::chaos::common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)1000000000);
}

// inherited method
void LoadInstanceOnUnitServer::acquireHandler() {
    int err = 0;
    
    switch(search_script_phase) {
        case SearchScriptPhaseLoadScriptPage:
            current_script_idx = 0;
            current_script_page.clear();
            //load next page
            //get the script that are present into the pools
            if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->getScriptForExecutionPoolPathList(epool_list,
                                                                                                            current_script_page,
                                                                                                            last_sequence_id,
                                                                                                            10))) {
                LOG_AND_TROW(ERR, err, "Error fetching script page");
            }
            
            //set the last sequnce id since now
            if(current_script_page.size()) {
                last_sequence_id = current_script_page[current_script_page.size()-1].unique_id;
                
                //change to the load instance per script phase
                search_script_phase = SearchScriptPhaseLoadInstancePage;
            } else {
                //we have no had script so we finisch here
                BC_END_RUNNING_PROPERTY;
                break;
            }
            
            
        case SearchScriptPhaseLoadInstancePage:
            //load instance for script
            current_instance_idx = 0;
            current_instance_page.clear();
            if(current_script_idx < current_script_page.size()) {
                if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->getUnscheduledInstanceForJob(current_script_page[current_script_idx],
                                                                                                           current_instance_page))) {
                    LOG_AND_TROW(ERR, err, "Error fetching script page");
                }
                //set the last sequnce id since now
                if(current_instance_page.size()) {
                    //change to the load instance per script phase
                    search_script_phase = SearchScriptPhaseConsumeInstance;
                    instance_work_phase = InstanceWorkPhasePrepare;
                } else {
                    //we terminate here and we are doing another loop on same phase for check instance on the next script
                    //go to next script
                    current_script_idx++;
                    break;
                }
            }else {
                //we need to load another script page
                search_script_phase = SearchScriptPhaseLoadScriptPage;
                break;
            }
            
            
        case SearchScriptPhaseConsumeInstance:
            //load instance for script
            if(current_instance_idx < current_instance_page.size()) {
                //we can work on current
            } else {
                //try to load more instances
                search_script_phase = SearchScriptPhaseLoadInstancePage;
            }
            break;
    }
}

// inherited method
void LoadInstanceOnUnitServer::ccHandler() {
    switch(search_script_phase) {
        case SearchScriptPhaseLoadScriptPage:
            break;
            
            
        case SearchScriptPhaseLoadInstancePage:
            break;
            
            
        case SearchScriptPhaseConsumeInstance: {
            switch(instance_work_phase) {
                case InstanceWorkPhasePrepare: {
                    int err = 0;
                    bool reserved = false;
                    //reserve the instance
                    if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->reserveInstanceForScheduling(reserved,
                                                                                                               current_instance_page[current_instance_idx],
                                                                                                               unit_server,
                                                                                                               30000))){
                        LOG_AND_TROW(ERR, err, CHAOS_FORMAT("Error reserving %1% instance",%current_instance_page[current_instance_idx]));
                    } else {
                        if(reserved == false) {
                            //go to the next instance
                            current_instance_idx++;
                            search_script_phase = SearchScriptPhaseConsumeInstance;
                            break;
                            
                        } else {
                            //we have had success reserving the instance
                            if(prepareScriptInstance(current_script_page[current_script_idx],
                                                     current_instance_page[current_instance_idx])) {
                                //we need to the laod phase
                                instance_work_phase = InstanceWorkPhaseLoadOnServer;
                            } else {
                                //we need to work on another instance
                                current_instance_idx++;
                                search_script_phase = SearchScriptPhaseConsumeInstance;
                                break;
                            }
                        }
                    }
                    
                }
                    
                case InstanceWorkPhaseLoadOnServer:
                    if(loadScriptInstance()){
                        //load operation has terminated
                        //we need to work on another instance
                        current_instance_idx++;
                        search_script_phase = SearchScriptPhaseConsumeInstance;
                    } else {
                        //load operation need more timing
                    }
                    break;
            }
            
            break;
        }
    }
}

// inherited method
bool LoadInstanceOnUnitServer::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}

bool LoadInstanceOnUnitServer::prepareScriptInstance(const chaos::service_common::data::script::ScriptBaseDescription& current_script_description,
                                                     const std::string& current_instance_name) {
    int err = 0;
    
    //first time we need to copy the script dataset into the instance
    if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->copyScriptDatasetAndContentToInstance(current_script_description,
                                                                                                        current_instance_name))){
        ERR << "We have had error copying script dataset to instance";
        return true;
    }
    
    //script is ever considered that it is in autoload
    load_datapack = CUCommonUtility::prepareRequestPackForLoadControlUnit(current_instance_name,
                                                                          getDataAccess<mds_data_access::ControlUnitDataAccess>());
    if(load_datapack.get() == NULL){
        ERR << "Error creating autoload datapack for:"<<current_instance_name;
        return false;
    }
    //prepare auto init and autostart message into autoload pack
    CUCommonUtility::prepareAutoInitAndStartInAutoLoadControlUnit(current_instance_name,
                                                                  getDataAccess<mds_data_access::NodeDataAccess>(),
                                                                  getDataAccess<mds_data_access::ControlUnitDataAccess>(),
                                                                  getDataAccess<mds_data_access::DataServiceDataAccess>(),
                                                                  load_datapack.get());
    
    INFO << "Autoload control unit " << current_instance_name;
    request = createRequest(unit_server_rpc_addr,
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT);
    
    return true;
}

bool LoadInstanceOnUnitServer::loadScriptInstance() {
    bool result = false;
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:{
            sendMessage(*request,
                        load_datapack.get());
            result = true;
            break;
        }
            
        case MESSAGE_PHASE_SENT:{
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:{
            result = true;
            break;
        }
    }
    return result;
}
