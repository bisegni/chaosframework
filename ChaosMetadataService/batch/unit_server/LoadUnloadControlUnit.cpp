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


using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::unit_server;

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
MDSBatchCommand() {
    //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
    //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}
LoadUnloadControlUnit::~LoadUnloadControlUnit() {
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
    
    //set the send command phase
    request = createRequest(us_address,
                            UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                            (load?UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT:UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT));
}

// inherited method
void LoadUnloadControlUnit::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void LoadUnloadControlUnit::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT:{
            CDataWrapper message;
            message.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
            if(load){message.addStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE, cu_type);}
            sendRequest(*request,
                        &message);
            
            break;
        }
        case MESSAGE_PHASE_SENT:{
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:{
            BC_END_RUNNIG_PROPERTY
            break;
        }
    }
    
}

// inherited method
bool LoadUnloadControlUnit::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}