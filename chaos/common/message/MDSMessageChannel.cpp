/*
 *	MDSMessageChannel.cpp
 *	!CHAOS
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

#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/network/NetworkBroker.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::data;

/*
#define CHECK_TIMEOUT_AND_RESULT_CODE(x,e) \
if(!x.get()) {\
e = ErrorCode::EC_TIMEOUT;\
} else if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {\
e = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
}*/

    //! Send heartbeat
/*!
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(string& identificationID) {
    CDataWrapper hbMessage;
    hbMessage.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identificationID);
    sendMessage(nodeAddress->node_id,
                ChaosSystemDomainAndActionLabel::MDS_CU_HEARTBEAT,
                &hbMessage);
}


//! Send unit server CU states to MDS
int MDSMessageChannel::sendUnitServerCUStates(CDataWrapper& deviceDataset, bool requestCheck, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    string currentBrokerIpPort;
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    deviceDataset.addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    if(requestCheck){
        auto_ptr<CDataWrapper> cuStates( sendRequest(nodeAddress->node_id,
                                                     ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                                                     &deviceDataset,
                                                     millisecToWait));
        CHECK_TIMEOUT_AND_RESULT_CODE(cuStates, err)
    } else {
        sendMessage(nodeAddress->node_id,
                    ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                    &deviceDataset);
    }
    return err;
}


//! Send unit server description to MDS
int MDSMessageChannel::sendNodeRegistration(CDataWrapper& deviceDataset, bool requestCheck, uint32_t millisecToWait) {
    int err = ErrorCode::EC_NO_ERROR;
    string currentBrokerIpPort;
        //get rpc receive port
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    deviceDataset.addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);

        //set our timestamp
    deviceDataset.addInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP, chaos::common::utility::TimingUtil::getTimeStamp());
    
    if(requestCheck){
        auto_ptr<CDataWrapper> deviceRegistrationCheck(sendRequest(nodeAddress->node_id,
                                                                   MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                                                                   &deviceDataset,
                                                                   millisecToWait));
        CHECK_TIMEOUT_AND_RESULT_CODE(deviceRegistrationCheck, err)
    } else {
        sendMessage(nodeAddress->node_id,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                    &deviceDataset);
    }
    return err;
}

    //! Get all active device id
int MDSMessageChannel::getAllDeviceID(vector<string>&  deviceIDVec, uint32_t millisecToWait) {
        //send request and wait the response
    int err = ErrorCode::EC_NO_ERROR;
    auto_ptr<CDataWrapper> resultAnswer(sendRequest(nodeAddress->node_id,
                                                    ChaosSystemDomainAndActionLabel::MDS_GET_ALL_DEVICE,
                                                    NULL,
                                                    millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(resultAnswer, err)
    if(err == ErrorCode::EC_NO_ERROR && resultAnswer->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
        auto_ptr<CDataWrapper> allDeviceInfo(resultAnswer->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(allDeviceInfo->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)){
                //there is a result
            auto_ptr<CMultiTypeDataArrayWrapper> allDeviceInfoVec(allDeviceInfo->getVectorValue(NodeDefinitionKey::NODE_UNIQUE_ID));
            
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
    callData->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identificationID);
        //send request and wait the response
    auto_ptr<CDataWrapper> resultAnswer(sendRequest(nodeAddress->node_id,
                                                    ChaosSystemDomainAndActionLabel::MDS_GET_NODE_ADDRESS,
                                                    callData.get(),
                                                    millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(resultAnswer, err)
    if(err == ErrorCode::EC_NO_ERROR && resultAnswer->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
        auto_ptr<CDataWrapper> nodeNetworkInfromation(resultAnswer->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(nodeNetworkInfromation->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) && nodeNetworkInfromation->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)){
                //there is a result
            *deviceNetworkAddress = new CDeviceNetworkAddress();
            (*deviceNetworkAddress)->ip_port = nodeNetworkInfromation->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
            (*deviceNetworkAddress)->node_id = nodeNetworkInfromation->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
            (*deviceNetworkAddress)->device_id = identificationID;
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
    callData->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identificationID);
        //send request and wait the response
    auto_ptr<CDataWrapper> deviceInitInformation(sendRequest(nodeAddress->node_id,
                                                             "getCurrentDataset",
                                                             callData.get(),
                                                             millisecToWait));
    CHECK_TIMEOUT_AND_RESULT_CODE(deviceInitInformation, err) 
    if(err == ErrorCode::EC_NO_ERROR) {
        if(deviceInitInformation->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)){
            *deviceDefinition = deviceInitInformation->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
        }
    }
    return err;
}
