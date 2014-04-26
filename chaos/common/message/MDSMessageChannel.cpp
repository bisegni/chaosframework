/*
 *	MDSMessageChannel.cpp
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
#include "MDSMessageChannel.h"
#include <chaos/common/network/NetworkBroker.h>
using namespace chaos;
using namespace chaos::common::data;

#define CHECK_TIMEOUT_AND_RESULT_CODE(x,e) \
if(!x.get()) {\
e = ErrorCode::EC_TIMEOUT;\
} else if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {\
e = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
}

    //! Send heartbeat
/*!
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(string& identificationID) {
    CDataWrapper hbMessage;
    hbMessage.addStringValue(DatasetDefinitionkey::DEVICE_ID, identificationID);
    MessageChannel::sendMessage(nodeAddress->nodeID.c_str(), "heartbeat", &hbMessage);
}

    //! Send dataset to MDS
/*!
 Return a list of all device id that are active
 \param deviceDataset the CDatawrapper representi the device dataset infromation, the pointer is not disposed
 \param millisecToWait delay after wich the wait is interrupt
 */
int MDSMessageChannel::sendUnitDescription(CDataWrapper *deviceDataset, bool requestCheck, uint32_t millisecToWait) {
    CHAOS_ASSERT(deviceDataset)
    int err = ErrorCode::EC_NO_ERROR;
    string currentBrokerIpPort;
    getBroker()->getPublishedHostAndPort(currentBrokerIpPort);
    deviceDataset->addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS, currentBrokerIpPort.c_str());
    if(requestCheck){
        auto_ptr<CDataWrapper> deviceRegistrationCheck( MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "registerControlUnit", deviceDataset, millisecToWait));
        CHECK_TIMEOUT_AND_RESULT_CODE(deviceRegistrationCheck, err)
    } else {
        MessageChannel::sendMessage(nodeAddress->nodeID.c_str(), "registerControlUnit", deviceDataset);
    }
    
    return err;
}

    //! Get all active device id
/*!
 Return a list of all device id that are active
 \deviceID
 */
int MDSMessageChannel::getAllDeviceID(vector<string>&  deviceIDVec, uint32_t millisecToWait) {
        //send request and wait the response
    int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> resultAnswer( MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getAllActiveDevice", NULL, millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(resultAnswer, err)
    if(err == ErrorCode::EC_NO_ERROR && resultAnswer->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
        auto_ptr<CDataWrapper> allDeviceInfo(resultAnswer->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(allDeviceInfo->hasKey(DatasetDefinitionkey::DEVICE_ID)){
                //there is a result
            auto_ptr<CMultiTypeDataArrayWrapper> allDeviceInfoVec(allDeviceInfo->getVectorValue(DatasetDefinitionkey::DEVICE_ID));
            
            for (int idx = 0; idx < allDeviceInfoVec->size(); idx++) {
                deviceIDVec.push_back(allDeviceInfoVec->getStringElementAtIndex(idx));
            }
        }
    }
    return err;
}

    //! Get node address for identification id
/*!
 Return the node address for an identification id
 nodeNetworkAddress node address structure to be filled with identification id network info
 */
int MDSMessageChannel::getNetworkAddressForDevice(string& identificationID, CDeviceNetworkAddress** deviceNetworkAddress, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    if(!deviceNetworkAddress) return -1;
    auto_ptr<CDataWrapper> callData(new CDataWrapper());
    callData->addStringValue(DatasetDefinitionkey::DEVICE_ID, identificationID);
        //send request and wait the response
    auto_ptr<CDataWrapper> resultAnswer( MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getNodeNetworkAddress", callData.get(), millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(resultAnswer, err)
    if(err == ErrorCode::EC_NO_ERROR && resultAnswer->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
        auto_ptr<CDataWrapper> nodeNetworkInfromation(resultAnswer->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS) && nodeNetworkInfromation->hasKey(CUDefinitionKey::CS_CM_CU_INSTANCE)){
                //there is a result
            *deviceNetworkAddress = new CDeviceNetworkAddress();
            (*deviceNetworkAddress)->ipPort = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS);
            (*deviceNetworkAddress)->nodeID = nodeNetworkInfromation->getStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE);
            (*deviceNetworkAddress)->deviceID = identificationID;
        }
    }
    return err;
}


    //! Get curent dataset for device
/*!
 Return the node address for an identification id
 \param identificationID id for wich we need to get the network address information
 \param millisecToWait delay after wich the wait is interrupt
 \return if the infromation is found, a CDataWrapper for dataset desprition is returned
 */
int MDSMessageChannel::getLastDatasetForDevice(string& identificationID, CDataWrapper** deviceDefinition, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    if(!deviceDefinition) return -1;
    auto_ptr<CDataWrapper> callData(new CDataWrapper());
    callData->addStringValue(DatasetDefinitionkey::DEVICE_ID, identificationID);
        //send request and wait the response
    auto_ptr<CDataWrapper> deviceInitInformation(MessageChannel::sendRequest(nodeAddress->nodeID.c_str(), "getCurrentDataset", callData.get(), millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(deviceInitInformation, err)
    if(err == ErrorCode::EC_NO_ERROR) {
        if(deviceInitInformation->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
            *deviceDefinition = deviceInitInformation->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
        }
    }
    return err;
}
