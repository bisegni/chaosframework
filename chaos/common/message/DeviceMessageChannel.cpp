/*	
 *	DeviceMessageChannel.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include "DeviceMessageChannel.h"
#include <chaos/common/chaos_constants.h>

using namespace chaos::common::message;
using namespace chaos::common::data;

//------------------------------------
DeviceMessageChannel::DeviceMessageChannel(NetworkBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress) : NodeMessageChannel(msgBroker, _deviceNetworkAddress){
        //take the reference for avoid continue cast fro CNetwork and CDevice
    deviceNetworkAddress = _deviceNetworkAddress;
}
//------------------------------------

void DeviceMessageChannel::setNewAddress(CDeviceNetworkAddress *_deviceAddress) {
    NodeMessageChannel::setNewAddress(_deviceAddress);
    deviceNetworkAddress = _deviceAddress;
}
//------------------------------------
int DeviceMessageChannel::initDevice(CDataWrapper *initData, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CHAOS_ASSERT(initData)
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
																  ChaosSystemDomainAndActionLabel::ACTION_CU_INIT,
																  initData,
																  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_DEINIT,
															  &message_data,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_START,
															  &message_data,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_STOP,
															  &message_data,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::restoreDeviceToTag(const std::string& restore_tag, uint32_t millisecToWait) {
	int err = ErrorCode::EC_NO_ERROR;
	CDataWrapper message_data;
	message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
	message_data.addStringValue(ChaosSystemDomainAndActionLabel::ACTION_CU_RESTORE_PARAM_TAG, restore_tag);
	auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_RESTORE,
															  &message_data,
															  millisecToWait));
	CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
	return err;
}

//------------------------------------
int DeviceMessageChannel::getType(std::string& control_unit_type, uint32_t millisecToWait) {
	int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_GET_INFO,
															  NULL,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
	if(err == ErrorCode::EC_NO_ERROR) {
        auto_ptr<CDataWrapper> info_pack(result->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(info_pack.get() && info_pack->hasKey(NodeDefinitionKey::NODE_TYPE)){
            control_unit_type = info_pack->getStringValue(NodeDefinitionKey::NODE_TYPE);
        }
    }
    return err;
}

//------------------------------------
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  ChaosSystemDomainAndActionLabel::ACTION_CU_GET_STATE,
															  &message_data,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    if(err == ErrorCode::EC_NO_ERROR) {
        auto_ptr<CDataWrapper> statePack(result->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(statePack.get() && statePack->hasKey(CUStateKey::CONTROL_UNIT_STATE)){
            deviceState = (CUStateKey::ControlUnitState)statePack->getInt32Value(CUStateKey::CONTROL_UNIT_STATE);
        }
    }
    return err;
}

//------------------------------------
int DeviceMessageChannel::setAttributeValue(CDataWrapper& attributesValues, bool noWait, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
        //create the pack
    //CDataWrapper deviceAttributeValues;
        //setup with the deviceID code
    attributesValues.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    //deviceAttributeValues.addCSDataValue(DatasetDefinitionkey::DESCRIPTION, attributesValues);
    if(noWait){
        MessageChannel::sendMessage(deviceNetworkAddress->nodeID.c_str(), "setDatasetAttribute", &attributesValues);
    } else {
        auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "setDatasetAttribute", &attributesValues, millisecToWait));
        CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::setScheduleDelay(uint64_t scheduledDealy, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_TYPE, deviceNetworkAddress->deviceID);
    message_data.addInt64Value(CUDefinitionKey::THREAD_SCHEDULE_DELAY, scheduledDealy);
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  "updateConfiguration",
															  &message_data,
															  millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return err;

}

//------------------------------------
void DeviceMessageChannel::sendCustomMessage(const char * const actAlias, CDataWrapper* const message_data, bool queued) {
    MessageChannel::sendMessage(deviceNetworkAddress->nodeID.c_str(),
								actAlias,
								message_data,
								!queued);
}

//------------------------------------
int DeviceMessageChannel::sendCustomRequest(const char * const actAlias, CDataWrapper* const message_data, CDataWrapper** resultData, uint32_t millisecToWait, bool async, bool queued) {
    int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(),
															  actAlias,
															  message_data,
															  millisecToWait,
															  async,
															  !queued));
    CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    if(err == ErrorCode::EC_NO_ERROR && resultData) {
        *resultData = result->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
    }
    return err;
}
