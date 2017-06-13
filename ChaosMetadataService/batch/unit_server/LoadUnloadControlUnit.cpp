/*
 *	LoadUnloadControlUnit.cpp
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

#include "LoadUnloadControlUnit.h"

#include "../control_unit/IDSTControlUnitBatchCommand.h"
#include "../../common/CUCommonUtility.h"
#include <chaos_service_common/data/data.h>
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch::unit_server;
using namespace chaos::service_common::data::script;

#define BATHC_CU_LUL_INFO INFO_LOG(LoadUnloadControlUnit)
#define BATHC_CU_LUL_DBG  DBG_LOG(LoadUnloadControlUnit)
#define BATHC_CU_LUL_ERR  ERR_LOG(LoadUnloadControlUnit)

DEFINE_MDS_COMAMND_ALIAS(LoadUnloadControlUnit)
static const char * const LoadUnloadControlUnit_NO_DATA = "No data has been set";
static const char * const LoadUnloadControlUnit_NO_UNIQUE_ID = "No Unique id in data";
static const char * const LoadUnloadControlUnit_NO_CU_TYPE = "No control unit type in data";
static const char * const LoadUnloadControlUnit_NO_RPC_ADDRESS = "No unit server rpc address";
static const char * const LoadUnloadControlUnit_NO_LOAD_UNLOAD = "No load or unload has been specified";

LoadUnloadControlUnit::LoadUnloadControlUnit():
MDSBatchCommand() {}
LoadUnloadControlUnit::~LoadUnloadControlUnit() {}

int LoadUnloadControlUnit::prepareInstanceScriptForLoad(const std::string& instance_name) {
    int err = 0;
    bool found = false;
    ScriptBaseDescription sbd;
    
    //befor send if need to check if the current instsance is a script
    BATHC_CU_LUL_INFO << CHAOS_FORMAT("Send load data to %1%", %instance_name);
    if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getScriptAssociatedToControlUnitInstance(instance_name,
                                                                                                                found,
                                                                                                                sbd))){
        BATHC_CU_LUL_ERR << CHAOS_FORMAT("Error fetching the script associated to the control unit instance %1% with error %2%", %instance_name%err);
    } else if(found) {
        //in this case we need to copy the script data to the control unit instances
        if((err = getDataAccess<mds_data_access::ScriptDataAccess>()->copyScriptDatasetAndContentToInstance(sbd,
                                                                                                            instance_name))){
            BATHC_CU_LUL_ERR << CHAOS_FORMAT("Error copying the script data to the control unit instance %1% with error %2%", %instance_name%err);
        }
    }
    return err;
}

// inherited method
void LoadUnloadControlUnit::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    if(!data) {
        BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_DATA;
        throw chaos::CException(-1, LoadUnloadControlUnit_NO_DATA, __PRETTY_FUNCTION__);
    }
    
    if(!data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)){
        BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_UNIQUE_ID;
        throw chaos::CException(-2, LoadUnloadControlUnit_NO_UNIQUE_ID, __PRETTY_FUNCTION__);
    } else {
        cu_id = data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    }
    
    if(!data->hasKey("load")) {
        BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_LOAD_UNLOAD;
        throw chaos::CException(-5, LoadUnloadControlUnit_NO_LOAD_UNLOAD, __PRETTY_FUNCTION__);
        
    } else {
        load = data->getBoolValue("load");
    }
    
    if(load) {
        if(!data->hasKey(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE)){
            BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_CU_TYPE;
            throw chaos::CException(-3, LoadUnloadControlUnit_NO_CU_TYPE, __PRETTY_FUNCTION__);
        } else {
            cu_type = data->getStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE);
        }
    }
    
    if(!data->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_RPC_ADDRESS;
        throw chaos::CException(-4, LoadUnloadControlUnit_NO_RPC_ADDRESS, __PRETTY_FUNCTION__);
        
    } else {
        us_address = data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
    }
    //prepare load data pack to sento to control unit
    if(load){
        //in case ho script instance copy the script information
        prepareInstanceScriptForLoad(cu_id);
        load_unload_pack = CUCommonUtility::prepareRequestPackForLoadControlUnit(cu_id,
                                                                                 getDataAccess<mds_data_access::ControlUnitDataAccess>());
    } else {
        load_unload_pack.reset(new CDataWrapper());
        load_unload_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
    }
    //set the send command phase
    request = createRequest(us_address,
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            (load?UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT:UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT));
}

// inherited method
void LoadUnloadControlUnit::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:{
            sendMessage(*request,
                        load_unload_pack.get());
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
void LoadUnloadControlUnit::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:
            break;
            
        case MESSAGE_PHASE_SENT:{
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:{
            BC_END_RUNNING_PROPERTY
            break;
        }
    }
    
}

// inherited method
bool LoadUnloadControlUnit::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
