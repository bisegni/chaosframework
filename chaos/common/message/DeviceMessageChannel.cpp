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

using namespace chaos;
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
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_INIT, initData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper deinitDeviceData;
    deinitDeviceData.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_DEINIT, &deinitDeviceData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_START, &startDeviceParam, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper stopDeviceData;
    stopDeviceData.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_STOP, &stopDeviceData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

//------------------------------------
int DeviceMessageChannel::getType(std::string& control_unit_type, uint32_t millisecToWait) {
	int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> info_result(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_CU_GET_INFO, NULL, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(info_result, err)
	if(err == ErrorCode::EC_NO_ERROR) {
        auto_ptr<CDataWrapper> info_pack(info_result->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(info_pack.get() && info_pack->hasKey(CUDefinitionKey::CS_CM_CU_TYPE)){
            control_unit_type = info_pack->getStringValue(CUDefinitionKey::CS_CM_CU_TYPE);
        }
    }
    return err;
}

//------------------------------------
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper deviceStateData;
    deviceStateData.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> stateResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_GET_STATE, &deviceStateData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(stateResult, err)
    if(err == ErrorCode::EC_NO_ERROR) {
        auto_ptr<CDataWrapper> statePack(stateResult->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
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
    attributesValues.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
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
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceNetworkAddress->deviceID);
    startDeviceParam.addInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY, scheduledDealy);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "updateConfiguration", &startDeviceParam, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;

}

//------------------------------------
void DeviceMessageChannel::sendCustomMessage(const char * const actAlias, CDataWrapper* const requestData, bool queued) {
    MessageChannel::sendMessage(deviceNetworkAddress->nodeID.c_str(), actAlias, requestData, !queued);
}

//------------------------------------
int DeviceMessageChannel::sendCustomRequest(const char * const actAlias, CDataWrapper* const requestData, CDataWrapper** resultData, uint32_t millisecToWait, bool async, bool queued) {
    int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), actAlias, requestData, millisecToWait, async, !queued));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    if(err == ErrorCode::EC_NO_ERROR && resultData) {
        *resultData = initResult->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
    }
    return err;
}
