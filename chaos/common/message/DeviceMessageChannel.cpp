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
#include "DeviceMessageChannel.h"
#include <chaos/common/chaos_constants.h>

using namespace chaos::common::message;
using namespace chaos::common::utility;
using namespace chaos::common::data;

#define DMCINFO INFO_LOG(DeviceMessageChannel)
#define DMCDBG DBG_LOG(DeviceMessageChannel)
#define DMCERR ERR_LOG(DeviceMessageChannel)

#define CHECK_ONLINE_OR_RETURN(x)\
if(isOnline()!=OnlineStateOnline) return x;

//------------------------------------
DeviceMessageChannel::DeviceMessageChannel(NetworkBroker *msg_broker,
                                           CDeviceNetworkAddress *_device_network_address,
                                           bool _self_managed,
                                           MessageRequestDomainSHRDPtr _new_message_request_domain):
NodeMessageChannel(msg_broker,
                   _device_network_address,
                   _new_message_request_domain),
device_network_address(_device_network_address),
local_mds_channel(NULL),
online(OnlineStateUnknown),
self_managed(_self_managed),
auto_reconnection(_self_managed){}

/*!
 Initialization phase of the channel
 */
void DeviceMessageChannel::init() throw(CException) {
    DMCINFO<< "Try to allocate local mds channel";
    if(self_managed) {
        if((local_mds_channel = getBroker()->getMetadataserverMessageChannel(getMessageRequestDomain())) == NULL) {
            throw CException(-1, "Local metadata server channel not found", __PRETTY_FUNCTION__);
        }
        DMCINFO<< "Local MDS Channel allocated";
        
//        if(device_network_address->ip_port.size() == 0 ||
//           device_network_address->node_id.size() == 0) {
            //force to update the network id
        tryToReconnect();
//        }
    }
}

/*!
 Initialization phase of the channel
 */
void DeviceMessageChannel::deinit() throw(CException) {
    if(self_managed) {
        async_central::AsyncCentralManager::getInstance()->removeTimer(this);

        if(local_mds_channel) {
            DMCINFO<< "Dispose local mds channel";
            getBroker()->disposeMessageChannel(local_mds_channel);
            local_mds_channel = NULL;
        }
    }
}

#pragma health utility
void DeviceMessageChannel::setNewAddress(CDeviceNetworkAddress *new_device_network_address) {
    NodeMessageChannel::setNewAddress(new_device_network_address);
    device_network_address = new_device_network_address;
    DMCINFO << boost::str(boost::format("New netowrk address for device %1% has been set[%2%/%3%]")%device_network_address->device_id%device_network_address->node_id%device_network_address->ip_port);
}

void DeviceMessageChannel::timeout() {
    DMCINFO << boost::str(boost::format("Automatic reconnection for device %1% on [%2%/%3%]")%device_network_address->device_id%device_network_address->node_id%device_network_address->ip_port);
    tryToReconnect();
}

bool DeviceMessageChannel::udpateNetworkAddress(int32_t millisec_to_wait) {
    int err = ErrorCode::EC_NO_ERROR;
    if(local_mds_channel == NULL || self_managed == false) return false;
    DMCINFO << "Update netowrk address for device " <<device_network_address->device_id;
    CDeviceNetworkAddress *tmp_addr = NULL;
    //ask to mds the new network address of the node
    if((err = local_mds_channel->getNetworkAddressForDevice(device_network_address->device_id, &tmp_addr))){
        //we have had error on comunication
        DMCINFO << "Error updating network address for " << device_network_address->node_id;
        return false;
    } else if (tmp_addr == NULL) {
        DMCINFO << "No network address received for " << device_network_address->node_id;
        return false;
    }
    
    //we can proceed
    ChaosUniquePtr<CDeviceNetworkAddress> new_device_network_address(tmp_addr);
    
    CHAOS_ASSERT((new_device_network_address->device_id.compare(device_network_address->device_id) == 0));
    
    //check if something has changed
    bool node_id_changed = (new_device_network_address->node_id.compare(device_network_address->node_id) != 0);
    bool node_ip_changed = (new_device_network_address->ip_port.compare(device_network_address->ip_port) != 0);
    bool is_changed = node_id_changed || node_ip_changed;
    if(is_changed) {
        //release new address to this channel
        setNewAddress(new_device_network_address.release());
    }
    return is_changed;
}

void DeviceMessageChannel::addListener(DeviceMessageChannelListener *new_listener) {
    AbstractListenerContainer::addListener(new_listener);
    //fire the last state
    ChaosWriteLock wl(mutex_online_state);
    //fire listener
    AbstractListenerContainer::fire(0);
}

void DeviceMessageChannel::removeListener(DeviceMessageChannelListener *listener) {
    AbstractListenerContainer::removeListener(listener);
}

const std::string& DeviceMessageChannel::getDeviceID() const {
    return device_network_address->device_id;
}

void DeviceMessageChannel::setOnline(OnlineState new_online_state) {
    if(new_online_state == online) return;
    
    if(new_online_state == OnlineStateOffline) {
        DMCINFO << CHAOS_FORMAT("Device %1% is offline",%device_network_address->device_id);
        //enable auto reconnection if we need
        if(auto_reconnection) {async_central::AsyncCentralManager::getInstance()->addTimer(this, 1000, 1000);}
    } else {
        DMCINFO << CHAOS_FORMAT("Device %1% is respawned",%device_network_address->device_id);
        //disable auto reconnection
        async_central::AsyncCentralManager::getInstance()->removeTimer(this);
    }
    ChaosWriteLock wl(mutex_online_state);
    online = new_online_state;
    //fire listener
    AbstractListenerContainer::fire(0);
}

void DeviceMessageChannel::tryToReconnect() {
    //!in this case we need to check on mds if somethig is changed
    bool address_changed = udpateNetworkAddress(5000);
    if(address_changed == false) {
        setOnline(OnlineStateOffline);
    }
    if(isOnline() != OnlineStateOnline) {
        bool is_nice_endpoint = (device_network_address->node_id.size() > 0) && (device_network_address->ip_port.size() > 0);
        if(is_nice_endpoint == false) {
            setOnline(OnlineStateOffline);
            //at this time we can porcess for request if node is alive because the
            //endpoint information if not good
            return;
        }
        //try to check on device if it is online
        ChaosUniquePtr<MessageRequestFuture> check_rpc_for_dev = checkRPCInformation();
        if(check_rpc_for_dev.get() &&
           check_rpc_for_dev->wait(5000)) {
            if(check_rpc_for_dev->getResult() &&
               check_rpc_for_dev->getResult()->hasKey("alive")) {
                setOnline(check_rpc_for_dev->getResult()->getBoolValue("alive")?OnlineStateOnline:OnlineStateOffline);
            } else {
                setOnline(OnlineStateOffline);
            }
        } else {
            setOnline(OnlineStateOffline);
        }
    }
}

OnlineState DeviceMessageChannel::isOnline() {
    ChaosReadLock rl(mutex_online_state);
    return online;
}

void DeviceMessageChannel::setAutoReconnection(bool _auto_reconnection) {
    if((auto_reconnection = _auto_reconnection)){
        if(online == OnlineStateOnline ||
           online == OnlineStateUnknown) {async_central::AsyncCentralManager::getInstance()->addTimer(this, 1000, 1000);}
    } else {
        async_central::AsyncCentralManager::getInstance()->removeTimer(this);
    }
}

void DeviceMessageChannel::setSelfManaged(bool _self_managed) {
    if(self_managed == _self_managed) return;
    deinit();
    self_managed = _self_managed;
    init();
}

#pragma device methods
int DeviceMessageChannel::recoverDeviceFromError(int32_t millisec_to_wait){
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_RECOVER,
                                              &message_data,
                                              millisec_to_wait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::initDevice(CDataWrapper *initData, int32_t millisec_to_wait) {
    CHAOS_ASSERT(initData)
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> initResult(sendRequest(device_network_address->node_id,
                                                  NodeDomainAndActionRPC::ACTION_NODE_INIT,
                                                  initData,
                                                  millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}


int DeviceMessageChannel::initDeviceToDefaultSetting(int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper *tmp_cdw_ptr = NULL;
    if(local_mds_channel == NULL  || self_managed == false) return -100;
    if((err = local_mds_channel->getLastDatasetForDevice(device_network_address->device_id, &tmp_cdw_ptr))){
        //we have had error on comunication
        DMCERR << "Error getting device initialization parameter for " << device_network_address->node_id;
        return -101;
    }
    ChaosUniquePtr<chaos::common::data::CDataWrapper> device_init_setting(tmp_cdw_ptr);
    //we can proceed wi the initilization
    ChaosUniquePtr<chaos::common::data::CDataWrapper> initResult(sendRequest(device_network_address->node_id,
                                                  NodeDomainAndActionRPC::ACTION_NODE_INIT,
                                                  device_init_setting.get(),
                                                  millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::deinitDevice(int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_DEINIT,
                                              &message_data,
                                              millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::startDevice(int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_START,
                                              &message_data,
                                              millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::stopDevice(int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_STOP,
                                              &message_data,
                                              millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::restoreDeviceToTag(const std::string& restore_tag, int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    message_data.addStringValue(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG, restore_tag);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_RESTORE,
                                              &message_data,
                                              millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::getType(std::string& control_unit_type, int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_CU_GET_INFO,
                                              NULL,
                                              millisec_to_wait));
    if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        if(result.get() && result->hasKey(NodeDefinitionKey::NODE_TYPE)){
            control_unit_type = result->getStringValue(NodeDefinitionKey::NODE_TYPE);
        }
    }
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    deviceState=CUStateKey::UNDEFINED;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_GET_STATE,
                                              &message_data,
                                              millisec_to_wait));
    if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        if(result.get() && result->hasKey(CUStateKey::CONTROL_UNIT_STATE)){
            deviceState = (CUStateKey::ControlUnitState)result->getInt32Value(CUStateKey::CONTROL_UNIT_STATE);
        }
    }
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::setAttributeValue(CDataWrapper& attributesValues,
                                            bool noWait,
                                            int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    //create the pack
    attributesValues.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    if(noWait){
        sendMessage(device_network_address->node_id, ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET, &attributesValues);
    } else {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> initResult(sendRequest(device_network_address->node_id,
                                                      ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                                      &attributesValues,
                                                      millisec_to_wait));
    }
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::setScheduleDelay(uint64_t scheduledDealy,
                                           int32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_network_address->device_id);
    message_data.addInt64Value(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, scheduledDealy);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              NodeDomainAndActionRPC::ACTION_UPDATE_PROPERTY,
                                              &message_data,
                                              millisec_to_wait));
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
    
}

//------------------------------------
int DeviceMessageChannel::sendCustomMessage(const std::string& action_name,
                                            CDataWrapper* const message_data) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    sendMessage(device_network_address->node_id,
                action_name,
                message_data);
    return 0;
}

//------------------------------------
int DeviceMessageChannel::sendCustomRequest(const std::string& action_name,
                                            CDataWrapper* const message_data,
                                            CDataWrapper** result_data,
                                            uint32_t millisec_to_wait) {
    CHECK_ONLINE_OR_RETURN(ErrorRpcCoce::EC_RPC_CHANNEL_OFFLINE);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(sendRequest(device_network_address->node_id,
                                              action_name,
                                              message_data,
                                              millisec_to_wait));
    if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        *result_data = result.release();
    }
    //setOnline(!CHAOS_IS_RPC_SERVER_OFFLINE(getLastErrorCode()));
    return getLastErrorCode();
}

//------------------------------------
ChaosUniquePtr<MessageRequestFuture>  DeviceMessageChannel::sendCustomRequestWithFuture(const std::string& action_name,
                                                                                       common::data::CDataWrapper *request_data) {
    //CHECK_ONLINE_OR_RETURN(ChaosUniquePtr<MessageRequestFuture>());
    return sendRequestWithFuture(device_network_address->node_id,
                                 action_name,
                                 request_data);
}

//! Send a request for receive RPC information
ChaosUniquePtr<MessageRequestFuture> DeviceMessageChannel::checkRPCInformation() {
    //CHECK_ONLINE_OR_RETURN(ChaosUniquePtr<MessageRequestFuture>());
    return NodeMessageChannel::checkRPCInformation(device_network_address->node_id);
}

//! Send a request for an echo test
ChaosUniquePtr<MessageRequestFuture> DeviceMessageChannel::echoTest(chaos::common::data::CDataWrapper *echo_data) {
    //CHECK_ONLINE_OR_RETURN(ChaosUniquePtr<MessageRequestFuture>());
    return NodeMessageChannel::echoTest(echo_data);
}

void DeviceMessageChannel::requestPromisesHandler(const FuturePromiseData& response_data) {
    if(!response_data.get()) return;
    if(response_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE) &&
       isOnline() == OnlineStateOnline) {
        bool online = !CHAOS_IS_RPC_SERVER_OFFLINE(response_data->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE));
        setOnline((online?OnlineStateOnline:OnlineStateOffline));
    }
}

void DeviceMessageChannel::fireToListener(unsigned int fire_code,
                                          AbstractListener *listener_to_fire) {
    DeviceMessageChannelListener *channel_listener = dynamic_cast<DeviceMessageChannelListener*>(listener_to_fire);
    if(channel_listener){
        channel_listener->deviceAvailabilityChanged(device_network_address->device_id,
                                                    online);
    }
}
