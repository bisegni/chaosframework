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
static const char * const LoadUnloadControlUnit_NO_MESSAGE_CHANNEL = "No message channel has been found";

LoadUnloadControlUnit::LoadUnloadControlUnit():
MDSBatchCommand(),
retry_number(0),
message_channel(NULL),
phase(LUL_SEND_LOAD_COMMAND){
        //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
        //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}
LoadUnloadControlUnit::~LoadUnloadControlUnit() {
    if(message_channel) {
        BATHC_CU_LUL_INFO << "Release message channel";
        releaseChannel(message_channel);
    }
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



    message_channel = getNewMessageChannel();
    if(!message_channel) {
        BATHC_CU_LUL_ERR << LoadUnloadControlUnit_NO_MESSAGE_CHANNEL;
        throw chaos::CException(-6, LoadUnloadControlUnit_NO_MESSAGE_CHANNEL, __PRETTY_FUNCTION__);
    }

        //set the send command phase
    phase = load?LUL_SEND_LOAD_COMMAND:LUL_SEND_UNLOAD_COMMAND;
}

    // inherited method
void LoadUnloadControlUnit::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(phase) {
        case LUL_SEND_LOAD_COMMAND:{
            CDataWrapper message;
            message.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
            message.addStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE, cu_type);

            BATHC_CU_LUL_DBG << "Send command to load control unit '" << cu_id << "' of type '" << cu_type << "'";
            request_future = message_channel->sendRequestWithFuture(us_address,
                                                                    UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                    UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT,
                                                                    &message);
            if(!request_future.get()) {
                BATHC_CU_LUL_ERR << "request with no future";
                throw chaos::CException(-1, "request with no future", __PRETTY_FUNCTION__);
            } else {
                phase = LUL_WAIT_ANSWER;
            }
            break;
        }
        case LUL_SEND_UNLOAD_COMMAND:{
            CDataWrapper message;
            message.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);

            BATHC_CU_LUL_DBG << "Send command to unload control unit '" << cu_id << "' of type '" << cu_type << "'";
            request_future = message_channel->sendRequestWithFuture(us_address,
                                                                    UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                    UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT,
                                                                    &message);
            if(!request_future.get()) {
                BATHC_CU_LUL_ERR << "request with no future";
                throw chaos::CException(-1, "request with no future", __PRETTY_FUNCTION__);
            } else {
                phase = LUL_WAIT_ANSWER;
            }
            break;
        }
        default:
            break;
    }
}

    // inherited method
void LoadUnloadControlUnit::ccHandler() {
    MDSBatchCommand::ccHandler();
    BATHC_CU_LUL_INFO << "execute ccHandler";
    retry_number++;
    switch(phase) {
        case LUL_WAIT_ANSWER: {
            if(request_future->wait(1000)) {
                    //we have hd answer
                if(request_future->getError()) {
                    BATHC_CU_LUL_DBG << "We have had answer with error"
                    << request_future->getError() << " \n and message "
                    << request_future->getErrorMessage() << "\n on domain "
                    << request_future->getErrorDomain();
                } else {
                    BATHC_CU_LUL_DBG << "Request send with success";
                }
                BC_END_RUNNIG_PROPERTY
            } else {
                    //we need to retry
                if((retry_number % 3) != 0) {
                    BATHC_CU_LUL_DBG << "Retry counter";
                    phase = LUL_WAIT_ANSWER;
                } else {
                    BATHC_CU_LUL_DBG << "No more retry";
                    BC_END_RUNNIG_PROPERTY
                }
            }
            break;
        }

        default:
            break;
    }
}

    // inherited method
bool LoadUnloadControlUnit::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    BATHC_CU_LUL_DBG << "execute ccHandler with " << result;
    BC_END_RUNNIG_PROPERTY
    return result;
}