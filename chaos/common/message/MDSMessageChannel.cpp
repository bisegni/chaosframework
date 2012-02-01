//
//  MDSMessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MDSMessageChannel.h"

using namespace chaos;

//! Send heartbeat
/*! 
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(string& identificationID) {
    CDataWrapper *hbMessage = new CDataWrapper();
    hbMessage->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    MDSMessageChannel::sendMessage(nodeAddress->nodeID.c_str(), "heartbeat", hbMessage);
}


//! Get all active device id
/*! 
 Return a list of all device id that are active
 \deviceID
 */
int MDSMessageChannel::getAllDeviceID(vector<string>&  deviceIDVec, unsigned int millisecToWait) {
    //send request and wait the response
    auto_ptr<CDataWrapper> allDevicesResponse( MDSMessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getAllActiveDevice", new CDataWrapper(), millisecToWait));
    
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
int MDSMessageChannel::getNetworkAddressForDevice(string& identificationID, CDeviceNetworkAddress& deviceNetworkAddress, unsigned int millisecToWait) {
    CDataWrapper *callData = new CDataWrapper();
    callData->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    //send request and wait the response
    auto_ptr<CDataWrapper> nodeNetworkInfromation( MDSMessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getNodeNetworkAddress", callData, millisecToWait));
    
    if(nodeNetworkInfromation.get() && nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS) && nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE)){
        //there is a result
        deviceNetworkAddress.ipPort = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS);
        deviceNetworkAddress.nodeID = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE);
        deviceNetworkAddress.deviceID = identificationID;
    }
    return nodeNetworkInfromation.get() == NULL;
}
