/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;

#define MSG_INFO INFO_LOG(MDSMessageChannel)
#define MSG_DBG DBG_LOG(MDSMessageChannel)
#define MSG_ERR ERR_LOG(MDSMessageChannel)

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
    ChaosUniquePtr<chaos::common::data::CDataWrapper> hb_message(new CDataWrapper());
    hb_message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                ChaosSystemDomainAndActionLabel::MDS_CU_HEARTBEAT,
                MOVE(hb_message));
}

int MDSMessageChannel::sendEchoMessage(CDWUniquePtr data, CDWUniquePtr& result) {
    int err = 0;
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                            NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                                                                            MOVE(data));
    if(request_future->wait()) {
        result = MOVE(request_future->detachResult());
    } else {
        result.reset();
        err = request_future->getError();
    }
    return err;
}

int MDSMessageChannel::getBuildInfo(CDWUniquePtr& result) {
    int err = 0;
    CDWUniquePtr data;
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                            NodeDomainAndActionRPC::ACTION_GET_BUILD_INFO,
                                                                                            MOVE(data));
    if(request_future->wait()) {
        result = MOVE(request_future->detachResult());
    } else {
        result.reset();
        err = request_future->getError();
    }
    return err;
}

//! Send unit server CU states to MDS
int MDSMessageChannel::sendUnitServerCUStates(CDWUniquePtr device_dataset,
                                              bool requestCheck,
                                              uint32_t millisec_to_wait) {
    string currentBrokerIpPort;
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    device_dataset->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    
    if(requestCheck){
        ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                                ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                                                                                                MOVE(device_dataset));
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    ChaosSystemDomainAndActionLabel::UNIT_SERVER_STATES_ANSWER,
                    MOVE(device_dataset));
    }
    return last_error_code;
}


//! Send unit server description to MDS
int MDSMessageChannel::sendNodeRegistration(CDWUniquePtr node_description,
                                            bool requestCheck,
                                            uint32_t millisec_to_wait) {
    std::string currentBrokerIpPort;
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    node_description->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    node_description->addStringValue(NodeDefinitionKey::NODE_HOST_NAME, InetUtility::getHostname());
    //set our timestamp
    node_description->addInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP,
                        chaos::common::utility::TimingUtil::getTimeStamp());
    if(requestCheck){
        ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                                MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                                                                                                MOVE(node_description));
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE,
                    MOVE(node_description));
    }
    return last_error_code;
}

int MDSMessageChannel::sentNodeHealthStatus(CDWUniquePtr node_health_data,
                                            bool request_check,
                                            uint32_t millisec_to_wait) {
    std::string currentBrokerIpPort;
    
    if(request_check){
        ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                                MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_HEALTH_STATUS,
                                                                                                MOVE(node_health_data));
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION,
                    MOVE(node_health_data));
    }
    return last_error_code;
}

int MDSMessageChannel::sendNodeLoadCompletion(CDWUniquePtr node_information,
                                              bool requestCheck,
                                              uint32_t millisec_to_wait) {
    std::string currentBrokerIpPort;
    
    //get rpc receive port
    getRpcPublishedHostAndPort(currentBrokerIpPort);
    //ChaosUniquePtr<chaos::common::data::CDataWrapper> data(new CDataWrapper(node_information.getBSONRawData(size_bson)));
    node_information->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, currentBrokerIpPort);
    
    //set our timestamp
    node_information->addInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP,
                        chaos::common::utility::TimingUtil::getTimeStamp());
    if(requestCheck){
        ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                                MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION,
                                                                                                MOVE(node_information));
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
        } else {
            last_error_code = -1;
        }
    } else {
        sendMessage(NodeDomainAndActionRPC::RPC_DOMAIN,
                    MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION,
                    MOVE(node_information));
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
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data(new CDataWrapper());
    data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture(NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                            "getNodeDescription",
                                                                                            MOVE(data));
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
                                               CDWUniquePtr& device_definition,
                                               uint32_t millisec_to_wait) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data(new CDataWrapper());
    data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("control_unit",
                                                                                            "getFullDescription",
                                                                                            MOVE(data));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((last_error_code == ErrorCode::EC_NO_ERROR) &&
           request_future->getResult()) {
            device_definition = request_future->detachResult();
        }
    }
    return last_error_code;
}

//! retrieve a dataset related to a snapshot name of a given cu
int  MDSMessageChannel::loadSnapshotNodeDataset(const std::string& snapname,
                                                const std::string& node_uid,
                                                chaos::common::data::CDataWrapper& data_set,
                                                uint32_t millisec_to_wait){
    int err=0;
    std::map<uint64_t,std::string> mapsnap_res;
    if(searchSnapshot(snapname,mapsnap_res,millisec_to_wait)==0){
        ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
        message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
        message->addStringValue("snapshot_name", snapname);
        
        ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                                "getSnapshotDatasetForNode",
                                                                                                MOVE(message));
        request_future->setTimeout(millisec_to_wait);
        if(request_future->wait()) {
            DECODE_ERROR(request_future)
            err = request_future->getError();
            if(err==0){
                
                CMultiTypeDataArrayWrapperSPtr snapshot_list = request_future->getResult()->getVectorValue("dataset_list");
                if(snapshot_list->size()){
                    data_set.addStringValue("name",node_uid);
                    data_set.addInt64Value("timestamp",mapsnap_res.begin()->first);
                }
                for(int idx = 0;
                    idx < snapshot_list->size();
                    idx++) {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> snapshot_dataset_element(snapshot_list->getCDataWrapperElementAtIndex(idx));
                    
                    const std::string dataset_name = snapshot_dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME);
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> val(snapshot_dataset_element->getCSDataValue("dataset_value"));
                    if(val->hasKey(chaos::DataPackCommonKey::DPCK_DATASET_TYPE)){
                        std::string ret=datasetTypeToHuman(val->getUInt32Value(chaos::DataPackCommonKey::DPCK_DATASET_TYPE));
                        data_set.addCSDataValue(ret,*val);
                    }
                }
            }
        } else {
            err = -1;
        }
    }
    return err;
}
int MDSMessageChannel::getFullNodeDescription(const std::string& identification_id,
                                              CDWUniquePtr& device_definition,
                                              uint32_t millisec_to_wait){
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data(new CDataWrapper());
    data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, identification_id);
    data->addBoolValue("all",true);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("control_unit",
                                                                                            "getFullDescription",
                                                                                            MOVE(data));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((last_error_code == ErrorCode::EC_NO_ERROR) &&
           request_future->getResult()) {
            device_definition = request_future->detachResult();
        }
    }
    return last_error_code;
}

//! return the configuration for the data driver
/*!
 Return the besta available data service at the monent within the configuraiton for data driver
 */
int MDSMessageChannel::getDataDriverBestConfiguration(CDWUniquePtr& device_definition,
                                                      uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    //send request and wait the response
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = sendRequestWithFuture(DataServiceNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                    "getBestEndpoints");
    future->setTimeout(millisec_to_wait);
    if(future->wait()) {
        if((err = future->getError()) == ErrorCode::EC_NO_ERROR) {
            device_definition = future->detachResult();
            if(device_definition.get() == NULL) {throw chaos::CException(-1, "Empty result", __PRETTY_FUNCTION__);}
        }
    } else {
        err = -1001;
    }
    return err;
}

int MDSMessageChannel::createNewSnapshot(const std::string& snapshot_name,
                                         const ChaosStringVector& node_list,
                                         uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("snapshot_name", snapshot_name);
    
    for(ChaosStringVectorConstIterator it = node_list.begin(),
        end = node_list.end();
        it != end;
        it++) {
        message->appendStringToArray(*it);
    }
    message->finalizeArrayForKey("node_list");
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "createNewSnapshot",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        err = request_future->getError();
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::restoreSnapshot(const std::string& snapshot_name,
                                       uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("snapshot_name", snapshot_name);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "restoreSnapshot",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        err = request_future->getError();
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::deleteSnapshot(const std::string& snapshot_name,
                                      uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("snapshot_name", snapshot_name);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "deleteSnapshot",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        err = request_future->getError();
    } else {
        err = -1;
    }
    return err;
}
int MDSMessageChannel::searchSnapshot(const std::string& query_filter,
                                      std::map<uint64_t,std::string>& snapshot_found,
                                      uint32_t millisec_to_wait){
    int err = ErrorCode::EC_NO_ERROR;
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "getAllSnapshot");
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((err = request_future->getError()) == ErrorCode::EC_NO_ERROR) {
            
            if(!request_future->getResult() ||
               !request_future->getResult()->hasKey("snapshot_list_result") ||
               !request_future->getResult()->isVectorValue("snapshot_list_result")) return err;
            
            CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = request_future->getResult()->getVectorValue("snapshot_list_result");
            for(int idx = 0;
                idx < snapshot_desc_list->size();
                idx++) {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> tmp_desc(snapshot_desc_list->getCDataWrapperElementAtIndex(idx));
                
                if(tmp_desc->hasKey("snap_name")) {
                    if(!query_filter.empty()){
                        std::string cmp=tmp_desc->getStringValue("snap_name");
                        // TODO: implement filter in DB query
                        if(strstr(cmp.c_str(),query_filter.c_str())){
                            uint64_t tm=tmp_desc->getInt64Value("snap_ts");
                            snapshot_found[tm]=cmp;
                            
                        }
                    } else {
                        uint64_t tm=tmp_desc->getInt64Value("snap_ts");
                        snapshot_found[tm]=tmp_desc->getStringValue("snap_name");
                        
                    }
                }
            }
        }
    } else {
        err = -1;
    }
    return err;
    
}
int MDSMessageChannel::searchSnapshot(const std::string& query_filter,
                                      ChaosStringVector& snapshot_found,
                                      uint32_t millisec_to_wait) {
    std::map<uint64_t,std::string> found;
    int ret=searchSnapshot(query_filter,
                           found,millisec_to_wait);
    
    if(ret==0){
        for(std::map<uint64_t,std::string>::iterator i=found.begin();i!=found.end();i++){
            snapshot_found.push_back(i->second);
        }
    }
    return ret;
}


int MDSMessageChannel::searchNodeForSnapshot(const std::string& snapshot_name,
                                             ChaosStringVector& node_found,
                                             uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("snapshot_name", snapshot_name);
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "getNodesForSnapshot",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((err = request_future->getError()) == ErrorCode::EC_NO_ERROR) {
            
            if(request_future->getResult() &&
               request_future->getResult()->hasKey("node_in_snapshot") &&
               request_future->getResult()->isVectorValue("node_in_snapshot")) {
                //we have result
                CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = request_future->getResult()->getVectorValue("node_in_snapshot");
                for(int idx = 0;
                    idx < snapshot_desc_list->size();
                    idx++) {
                    const std::string node_uid = snapshot_desc_list->getStringElementAtIndex(idx);
                    
                    node_found.push_back(node_uid);
                }
            }
        }
    } else {
        err = -1;
    }
    return err;
}


int MDSMessageChannel::searchSnapshotForNode(const std::string& node_uid,
                                             ChaosStringVector& snapshot_found,
                                             uint32_t millisec_to_wait){
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "getSnapshotForNode",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((err = request_future->getError()) == ErrorCode::EC_NO_ERROR) {
            
            if(request_future->getResult() &&
               request_future->getResult()->hasKey("snapshot_for_node") &&
               request_future->getResult()->isVectorValue("snapshot_for_node")) {
                //we have result
                CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = request_future->getResult()->getVectorValue("snapshot_for_node");
                for(int idx = 0;
                    idx < snapshot_desc_list->size();
                    idx++) {
                    const std::string node_uid = snapshot_desc_list->getStringElementAtIndex(idx);
                    snapshot_found.push_back(node_uid);
                }
            }
        }
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::setVariable(const std::string& variable_name,
                                   chaos::common::data::CDataWrapper& variable_value,
                                   uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("variable_name",
                            variable_name);
    message->addCSDataValue("variable_value",
                            variable_value);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "setVariable",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        err = request_future->getError();
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::searchVariable(const std::string& variable_name,ChaosStringVector& variable_found,
                                      uint32_t millisec_to_wait){
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("variable_name",
                            variable_name);
    
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "searchVariable",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((err = request_future->getError()) == ErrorCode::EC_NO_ERROR) {
            
            if(request_future->getResult() &&
               request_future->getResult()->hasKey("varlist") &&
               request_future->getResult()->isVectorValue("varlist")) {
                //we have result
                CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = request_future->getResult()->getVectorValue("varlist");
                for(int idx = 0;
                    idx < snapshot_desc_list->size();
                    idx++) {
                    const std::string node_uid = snapshot_desc_list->getStringElementAtIndex(idx);
                    
                    variable_found.push_back(node_uid);
                }
            }
        }
    } else {
        err = -1;
    }
    return err;
}


int MDSMessageChannel::getVariable(const std::string& variable_name,
                                   chaos::common::data::CDWUniquePtr& variable_value,
                                   uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("variable_name",
                            variable_name);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "getVariable",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        err = request_future->getError();
        variable_value = request_future->detachResult();
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::removeVariable(const std::string& variable_name,
                                      uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("variable_name",
                            variable_name);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("service",
                                                                                            "removeVariable",
                                                                                           MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        err = request_future->getError();
    } else {
        err = -1;
    }
    return err;
}

int MDSMessageChannel::searchNode(const std::string& unique_id_filter,
                                  unsigned int node_type_filter,
                                  bool alive_only,
                                  unsigned int last_node_sequence_id,
                                  unsigned int page_length,
                                  ChaosStringVector& node_found,
                                  uint32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    message->addStringValue("unique_id_filter", unique_id_filter);
    message->addInt32Value("node_type_filter", node_type_filter);
    message->addInt32Value("last_node_sequence_id", last_node_sequence_id);
    message->addBoolValue("alive_only", alive_only);
    message->addInt32Value("result_page_length", page_length);
    ChaosUniquePtr<MultiAddressMessageRequestFuture> request_future = sendRequestWithFuture("system",
                                                                                            "nodeSearch",
                                                                                            MOVE(message));
    request_future->setTimeout(millisec_to_wait);
    if(request_future->wait()) {
        DECODE_ERROR(request_future)
        if((err = request_future->getError()) == ErrorCode::EC_NO_ERROR) {
            if(request_future->getResult() &&
               request_future->getResult()->hasKey("node_search_result_page") &&
               request_future->getResult()->isVectorValue("node_search_result_page")) {
                //we have result
                CMultiTypeDataArrayWrapperSPtr snapshot_desc_list = request_future->getResult()->getVectorValue("node_search_result_page");
                for(int idx = 0;
                    idx < snapshot_desc_list->size();
                    idx++) {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> element(snapshot_desc_list->getCDataWrapperElementAtIndex(idx));
                    if(element.get() &&
                       element->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
                        node_found.push_back(element->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
                    }
                }
            }
        }
    } else {
        err = -1;
    }
    return err;
}


ChaosUniquePtr<MultiAddressMessageRequestFuture> MDSMessageChannel::sendRequestWithFuture(const std::string& action_domain,
                                                                                          const std::string& action_name,
                                                                                          chaos::common::data::CDWUniquePtr request_pack,
                                                                                          int32_t request_timeout) {
    return MultiAddressMessageChannel::sendRequestWithFuture(action_domain,
                                                             action_name,
                                                             MOVE(request_pack),
                                                             request_timeout);
}

void MDSMessageChannel::sendMessage(const std::string& action_domain,
                                    const std::string& action_name,
                                    chaos::common::data::CDWUniquePtr request_pack) {
    return MultiAddressMessageChannel::sendMessage(action_domain,
                                                   action_name,
                                                   MOVE(request_pack));
}

void MDSMessageChannel::callMethod(const std::string& action_domain,
                                   const std::string& action_name) {
    return MultiAddressMessageChannel::sendMessage(action_domain,
                                                   action_name);
}

