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
#include <chaos/common/cconstants.h>

using namespace chaos;

/*! \name Check Request Result Macro
 These macros are used to check the result of a syncronous request operation. 
 */
/*! \{ */

/*! Check for delay error or application error */
#define CHECK_TIMEOUT_AND_RESULT_CODE(x,e) \
if(!x.get()) {\
e = ErrorCode::EC_TIMEOUT;\
} else if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {\
e = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
}
/*! \} */

    //! base constructor
/*!
 The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
 ip:port:cu_node_address(instance):deviceID
 */
DeviceMessageChannel::DeviceMessageChannel(MessageBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress) : NetworkAddressMessageChannel(msgBroker, _deviceNetworkAddress){
        //take the reference for avoid continue cast fro CNetwork and CDevice
    deviceNetworkAddress = _deviceNetworkAddress;
}

void DeviceMessageChannel::setNewAddress(CDeviceNetworkAddress *_deviceAddress) {
    NetworkAddressMessageChannel::setNewAddress(_deviceAddress);
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
    deinitDeviceData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_DEINIT, &deinitDeviceData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

    //------------------------------------
int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_START, &startDeviceParam, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

    //------------------------------------
int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper stopDeviceData;
    stopDeviceData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_STOP, &stopDeviceData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

    //! Get device state
/*!
 Get the current state of the hardware
 \millisecToWait the number of millisecond for waith the answer
 */
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper deviceStateData;
    deviceStateData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
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
int DeviceMessageChannel::setAttributeValue(CDataWrapper& attributesValues, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
        //create the pack
    //CDataWrapper deviceAttributeValues;
        //setup with the deviceID code
    attributesValues.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    //deviceAttributeValues.addCSDataValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION, attributesValues);

    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "setDatasetAttribute", &attributesValues, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;
}

    //------------------------------------
int DeviceMessageChannel::setScheduleDelay(uint32_t scheduledDealy, uint32_t millisecToWait){
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    startDeviceParam.addInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY, scheduledDealy);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "updateConfiguration", &startDeviceParam, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return err;

}

/*! 
 \brief send a message to a custom action
 */
void DeviceMessageChannel::sendCustomMessage(const char * const actAlias, CDataWrapper* const requestData) {
    MessageChannel::sendMessage(deviceNetworkAddress->nodeID.c_str(), actAlias, requestData);
}

/*! 
 \brief send a request to a custom action
 */
int DeviceMessageChannel::sendCustomRequest(const char * const actAlias, CDataWrapper* const requestData, CDataWrapper**const resultData, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), actAlias, requestData, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    if(err == ErrorCode::EC_NO_ERROR) {
        *resultData = initResult->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
    }
    return err;
}
