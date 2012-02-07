//
//  DeviceMessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

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

int DeviceMessageChannel::initDevice(CDataWrapper *initData, uint32_t millisecToWait) {
    int err = 0;
    CHAOS_ASSERT(initData)
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "initDevice", initData, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "deinitDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "startDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    int err = 0;
    auto_ptr<CDataWrapper> initResult(MessageChannel::sendRequest(deviceNetworkAddress->nodeID.c_str(), "stopDevice", NULL, millisecToWait));
    if(initResult.get() && initResult->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)){
        err = initResult->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
    }
    return err;
}

    //! Send the CDatawrapper as device attribute values
/*!
 All the CDataWrapper is sent as pack for the device attributes values, no check is done
 \param attributesValues the container for the values of some dataset attributes
 */
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