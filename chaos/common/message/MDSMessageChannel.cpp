//
//  MDSMessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MDSMessageChannel.h"

using namespace chaos;

MDSMessageChannel::MDSMessageChannel(MessageBroker *msgBroker,const char * const mdsIpAddress) : MessageChannel(msgBroker, mdsIpAddress) {
    mdsAddress.ipPort = mdsIpAddress;
    mdsAddress.nodeID = "system";
}

MDSMessageChannel::~MDSMessageChannel() {
    
}

//! Send heartbeat
/*! 
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(string& identificationID) {
    CDataWrapper *hbMessage = new CDataWrapper();
    hbMessage->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, identificationID);
    MDSMessageChannel::sendMessage(mdsAddress.nodeID.c_str(), "heartbeat", hbMessage);
}


//! Get all active device id
/*! 
 Return a list of all device id that are active
 \deviceID
 */
void MDSMessageChannel::getAllDeviceID(vector<string>&  deviceIDVec, unsigned int millisecToWait) {
    //send request and wait the response
    auto_ptr<CDataWrapper> allDevicesResponse( MDSMessageChannel::sendRequest(mdsAddress.nodeID.c_str(), "getAllActiveDevice", new CDataWrapper(), millisecToWait));
    
    
    if(allDevicesResponse.get() && allDevicesResponse->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)){
        //there is a result
        auto_ptr<CMultiTypeDataArrayWrapper> deviceIDresult(allDevicesResponse->getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID));
        
        for (int idx = 0; idx < deviceIDresult->size(); idx++) {
            deviceIDVec.push_back(deviceIDresult->getStringElementAtIndex(idx));
        }
        
    }
}