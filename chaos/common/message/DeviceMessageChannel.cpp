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

using namespace chaos;

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
    int err = 0;
    CHAOS_ASSERT(initData)
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_INIT, initData, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    int err = 0;
    CDataWrapper deinitDeviceData;
    deinitDeviceData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_DEINIT, &deinitDeviceData, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = 0;
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_START, &startDeviceParam, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = 0;
    CDataWrapper stopDeviceData;
    stopDeviceData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_STOP, &stopDeviceData, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //! Get device state
/*!
 Get the current state of the hardware
 \millisecToWait the number of millisecond for waith the answer
 */
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait) {
    int err = 0;
    CDataWrapper stopDeviceData;
    stopDeviceData.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    auto_ptr<CDataWrapper> stateResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), ChaosSystemDomainAndActionLabel::ACTION_DEVICE_GET_STATE, &stopDeviceData, millisecToWait));
    if(stateResult.get() && stateResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = stateResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
        if(err == 0 && stateResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT)) {
            auto_ptr<CDataWrapper> statePack(stateResult->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT));
            if(statePack.get() && statePack->hasKey(CUStateKey::CONTROL_UNIT_STATE)){
                deviceState = (CUStateKey::ControlUnitState)statePack->getInt32Value(CUStateKey::CONTROL_UNIT_STATE);
            }
        }
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::setAttributeValue(CDataWrapper& attributesValues, uint32_t millisecToWait) {
    int err = 0;
        //create the pack
    CDataWrapper deviceAttributeValues;
        //setup with the deviceID code
    deviceAttributeValues.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    deviceAttributeValues.addCSDataValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION, attributesValues);

    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "setDatasetAttribute", &deviceAttributeValues, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //------------------------------------
int DeviceMessageChannel::setScheduleDelay(uint32_t scheduledDealy, uint32_t millisecToWait){
    int err = 0;
    CDataWrapper startDeviceParam;
    startDeviceParam.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceNetworkAddress->deviceID);
    startDeviceParam.addInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY, scheduledDealy);
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "updateConfiguration", &startDeviceParam, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;

}
