//
//  MDSMessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MDSMessageChannel.h"
#include <chaos/common/message/MessageBroker.h>
using namespace chaos;

//! Send heartbeat
/*! 
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(string& identificationID) {
    CDataWrapper hbMessage;
    hbMessage.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    MessageChannel::sendMessage(nodeAddress->nodeID.c_str(), "heartbeat", &hbMessage);
}

    //! Send dataset to MDS
/*! 
 Return a list of all device id that are active
 \param deviceDataset the CDatawrapper representi the device dataset infromation, th epointer is not disposed
 \param millisecToWait delay after wich the wait is interrupt
 */
int MDSMessageChannel::sendControlUnitDescription(CDataWrapper *deviceDataset, uint32_t millisecToWait) {
    CHAOS_ASSERT(deviceDataset)
    string currentBrokerIpPort;
    getBroker()->getPublishedHostAndPort(currentBrokerIpPort);
    deviceDataset->addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS, currentBrokerIpPort.c_str());
    
    MessageChannel::sendMessage(nodeAddress->nodeID.c_str(), "registerControlUnit", deviceDataset);
    return 0;
}

//! Get all active device id
/*! 
 Return a list of all device id that are active
 \deviceID
 */
int MDSMessageChannel::getAllDeviceID(vector<string>&  deviceIDVec, uint32_t millisecToWait) {
    //send request and wait the response
    auto_ptr<CDataWrapper> allDevicesResponse( MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getAllActiveDevice", NULL, millisecToWait));
    
    if(allDevicesResponse.get() && allDevicesResponse->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)){
        //there is a result
        auto_ptr<CMultiTypeDataArrayWrapper> deviceIDresult(allDevicesResponse->getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID));
        
        for (int idx = 0; idx < deviceIDresult->size(); idx++) {
            deviceIDVec.push_back(deviceIDresult->getStringElementAtIndex(idx));
        }
    }
    return allDevicesResponse.get()==NULL;
}

//! Get node address for identification id
/*! 
 Return the node address for an identification id
 nodeNetworkAddress node address structure to be filled with identification id network info
 */
CDeviceNetworkAddress *MDSMessageChannel::getNetworkAddressForDevice(string& identificationID, uint32_t millisecToWait) {
    CDataWrapper *callData = new CDataWrapper();
    CDeviceNetworkAddress *deviceNetworkAddress = NULL;
    callData->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    //send request and wait the response
    auto_ptr<CDataWrapper> nodeNetworkInfromation( MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getNodeNetworkAddress", callData, millisecToWait));
    
    if(nodeNetworkInfromation.get() && nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS) && nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE)){
        //there is a result
        deviceNetworkAddress = new CDeviceNetworkAddress();
        deviceNetworkAddress->ipPort = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS);
        deviceNetworkAddress->nodeID = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE);
        deviceNetworkAddress->deviceID = identificationID;
    }
    return deviceNetworkAddress;
}


//! Get curent dataset for device
/*! 
 Return the node address for an identification id
 \param identificationID id for wich we need to get the network address information
 \param millisecToWait delay after wich the wait is interrupt
 \return if the infromation is found, a CDataWrapper for dataset desprition is returned
 */
CDataWrapper* MDSMessageChannel::getLastDatasetForDevice(string& identificationID, uint32_t millisecToWait) {
    CDataWrapper *callData = new CDataWrapper();
    callData->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    //send request and wait the response
    CDataWrapper *deviceInitInformation = MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getCurrentDataset", callData, millisecToWait);
    return deviceInitInformation;
}
