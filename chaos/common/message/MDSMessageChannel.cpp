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

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;

#define DECODE_ERROR(x) \
if((last_error_code = x->getError())){\
last_error_message = x->getErrorMessage();\
last_error_domain = x->getErrorDomain();\
} else {\
last_error_message = "No Error";\
last_error_domain = "No Domain";\
}

MDSMessageChannel::MDSMessageChannel(NetworkBroker *network_broker,
                                     const std::vector<CNetworkAddress>& mds_node_address,
                                     MessageRequestDomainSHRDPtr _new_message_request_domain):
MultiAddressMessageChannel(network_broker,
                           mds_node_address,
                           _new_message_request_domain){}

//! return last sendxxx error code
int32_t MDSMessageChannel::getLastErrorCode() {
    return MultiAddressMessageChannel::getLastErrorCode();
}

//! return last sendxxx error message
const std::string& MDSMessageChannel::getLastErrorMessage() {
    return MultiAddressMessageChannel::getLastErrorMessage();
}

//! return last sendxxx error domain
const std::string& MDSMessageChannel::getLastErrorDomain() {
    return MultiAddressMessageChannel::getLastErrorDomain();
}


//! Send heartbeat
/*!
 Send the heartbeat for an identification ID. This can be an id for a device or an uitoolkit instance
 \param identificationID identification id of a device or a client
 */
void MDSMessageChannel::sendHeartBeatForDeviceID(const std::string& identification_id) {
    std::auto_ptr<CDataWrapper> hb_message(new CDataWrapper());
    hb_message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                ChaosSystemDomainAndActionLabel::MDS_CU_HEARTBEAT,
                hb_message.get());
}


//! Send unit server CU states to MDS
int MDSMessageChannel::sendUnitServerCUStates(CDataWrapper& deviceDataset,
                                              bool requestCheck,
                                              uint32_t millisec_to_wait) {
    int size_bson = 0;
    string currentBrokerIpPort;
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    std::auto_ptr<CDataWrapper> data(new CDataWrapper(deviceDataset.getBSONRawData(size_bson)));
    data->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    
    if(requestCheck){
        std::auto_ptr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                               ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                                                                                               data.release());
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                    data.get());
    }
    return last_error_code;
}


//! Send unit server description to MDS
int MDSMessageChannel::sendNodeRegistration(CDataWrapper& node_description,
                                            bool requestCheck,
                                            uint32_t millisec_to_wait) {
    int size_bson = 0;
    std::string currentBrokerIpPort;
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    std::auto_ptr<CDataWrapper> data(new CDataWrapper(node_description.getBSONRawData(size_bson)));
    data->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    
    //set our timestamp
    data->addInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP,
                        chaos::common::utility::TimingUtil::getTimeStamp());
    if(requestCheck){
        std::auto_ptr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                               MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                                                                                               data.release());
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                    data.get());
    }
    return last_error_code;
}

int MDSMessageChannel::sendNodeLoadCompletion(CDataWrapper& node_information,
                                              bool requestCheck,
                                              uint32_t millisec_to_wait) {
    int size_bson = 0;
    std::string currentBrokerIpPort;
    
    //get rpc receive port
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    std::auto_ptr<CDataWrapper> data(new CDataWrapper(node_information.getBSONRawData(size_bson)));
    data->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    
    //set our timestamp
    data->addInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP,
                        chaos::common::utility::TimingUtil::getTimeStamp());
    if(requestCheck){
        std::auto_ptr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                               MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION,
                                                                                               data.release());
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION,
                    data.get());
    }
    return last_error_code;
}

//! Get node address for identification id
/*!
 Return the node address for an identification id
 nodeNetworkAddress node address structure to be filled with identification id network info
 */
int MDSMessageChannel::getNetworkAddressForDevice(const std::string& identification_id,
                                                  CDeviceNetworkAddress** deviceNetworkAddress,
                                                  uint32_t millisec_to_wait) {
    if(!deviceNetworkAddress) return -1;
    auto_ptr<CDataWrapper> data(new CDataWrapper());
    data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    
    std::auto_ptr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                           "getNodeDescription",
                                                                                           data.release());
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((last_error_code  == ErrorCode::EC_NO_ERROR) &&
           request_future->getResult() &&
           request_future->getResult()->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) &&
           request_future->getResult()->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) {
            
            *deviceNetworkAddress = new CDeviceNetworkAddress();
            (*deviceNetworkAddress)->ip_port = request_future->getResult()->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
            (*deviceNetworkAddress)->node_id = request_future->getResult()->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
            (*deviceNetworkAddress)->device_id = identification_id;
        }
    }
    return last_error_code;
}


//! Get curent dataset for device
/*!
 Return the node address for an identification id
 \param identificationID id for wich we need to get the network address information
 \param millisec_to_wait delay after wich the wait is interrupt
 \return if the infromation is found, a CDataWrapper for dataset desprition is returned
 */
int MDSMessageChannel::getLastDatasetForDevice(const std::string& identification_id,
                                               CDataWrapper** deviceDefinition,
                                               uint32_t millisec_to_wait) {
    if(!deviceDefinition) return -1000;
    auto_ptr<CDataWrapper> data(new CDataWrapper());
    data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    
    std::auto_ptr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("control_unit",
                                                                                           "getFullDescription",
                                                                                           data.release());
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((last_error_code == ErrorCode::EC_NO_ERROR) &&
           request_future->getResult()) {
            *deviceDefinition = request_future->detachResult();
        }
    }
    return last_error_code;
}

//! return the configuration for the data driver
/*!
 Return the besta available data service at the monent within the configuraiton for data driver
 */
int MDSMessageChannel::getDataDriverBestConfiguration(CDataWrapper** deviceDefinition, uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    if(!deviceDefinition) return -1000;
    //send request and wait the response
    std::auto_ptr<MultiAddressMessageRequestFuture> future = sendRequestWithFuture(DataServiceNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                   "getBestEndpoints",
                                                                                   NULL);
    future->setTimeout(millisec_to_wait);
    if(future->wait()) {
        if((err = future->getError()) == ErrorCode::EC_NO_ERROR) {
            *deviceDefinition = future->detachResult();
        }
    } else {
        err = -1001;
    }
    return err;
}

std::auto_ptr<MultiAddressMessageRequestFuture> MDSMessageChannel::sendRequestWithFuture(const std::string& action_domain,
                                                                                         const std::string& action_name,
                                                                                         chaos::common::data::CDataWrapper *request_pack,
                                                                                         int32_t request_timeout) {
    return sendRequestWithFuture(action_domain,
                                 action_name,
                                 request_pack,
                                 request_timeout);
}

int MDSMessageChannel::sendMessage(const std::string& action_domain,
                                   const std::string& action_name,
                                   chaos::common::data::CDataWrapper *request_pack) {
    return sendMessage(action_domain,
                       action_name,
                       request_pack);
}

int MDSMessageChannel::callMethod(const std::string& action_domain,
                                  const std::string& action_name) {
    return sendMessage(action_domain,
                       action_name,
                       NULL);
}

