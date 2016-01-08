/*	
 *	DeviceMessageChannel.cpp
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
#include "DeviceMessageChannel.h"
#include <chaos/common/chaos_constants.h>

using namespace chaos::common::message;
using namespace chaos::common::data;

//------------------------------------
DeviceMessageChannel::DeviceMessageChannel(NetworkBroker *msgBroker,
                                           CDeviceNetworkAddress *_deviceNetworkAddress):
NodeMessageChannel(msgBroker,
                   _deviceNetworkAddress),
deviceNetworkAddress(_deviceNetworkAddress){
}
//------------------------------------

void DeviceMessageChannel::setNewAddress(CDeviceNetworkAddress *_deviceAddress) {
    setNewAddress(_deviceAddress);
    deviceNetworkAddress = _deviceAddress;
}

int DeviceMessageChannel::recoverDeviceFromError(uint32_t millisecToWait){
     CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_RECOVER,
                                              &message_data,
                                              millisecToWait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::initDevice(CDataWrapper *initData, uint32_t millisecToWait) {
    //int err = ErrorCode::EC_NO_ERROR;
    CHAOS_ASSERT(initData)
    auto_ptr<CDataWrapper> initResult(sendRequest(deviceNetworkAddress->node_id,
                                                  NodeDomainAndActionRPC::ACTION_NODE_INIT,
                                                  initData,
                                                  millisecToWait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(initResult, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::deinitDevice(uint32_t millisecToWait) {
    //int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_DEINIT,
                                              &message_data,
                                              millisecToWait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::startDevice(uint32_t millisecToWait) {
    //int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_START,
                                              &message_data,
											  millisecToWait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::stopDevice(uint32_t millisecToWait) {
    //int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_STOP,
											  &message_data,
											  millisecToWait));
    //CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
    return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::restoreDeviceToTag(const std::string& restore_tag, uint32_t millisecToWait) {
	//int err = ErrorCode::EC_NO_ERROR;
	CDataWrapper message_data;
	message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
	message_data.addStringValue(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG, restore_tag);
	auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_NODE_RESTORE,
											  &message_data,
                                              millisecToWait));
	//CHECK_TIMEOUT_AND_RESULT_CODE(result, err)
	return getLastErrorCode();
}

//------------------------------------
int DeviceMessageChannel::getType(std::string& control_unit_type, uint32_t millisecToWait) {
	//
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_CU_GET_INFO,
                                              NULL,
                                              millisecToWait));
	if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        if(result.get() && result->hasKey(NodeDefinitionKey::NODE_TYPE)){
            control_unit_type = result->getStringValue(NodeDefinitionKey::NODE_TYPE);
        }
    }
    return getLastErrorCode();;
}

//------------------------------------
int DeviceMessageChannel::getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait) {
    CDataWrapper message_data;
    deviceState=CUStateKey::UNDEFINED;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
											  NodeDomainAndActionRPC::ACTION_NODE_GET_STATE,
											  &message_data,
											  millisecToWait));
    if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        if(result.get() && result->hasKey(CUStateKey::CONTROL_UNIT_STATE)){
            deviceState = (CUStateKey::ControlUnitState)result->getInt32Value(CUStateKey::CONTROL_UNIT_STATE);
        }
    }
    return getLastErrorCode();;
}

//------------------------------------
int DeviceMessageChannel::setAttributeValue(CDataWrapper& attributesValues, bool noWait, uint32_t millisecToWait) {
        //create the pack
    //CDataWrapper deviceAttributeValues;
        //setup with the device_id code
    attributesValues.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    //deviceAttributeValues.addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, attributesValues);
    if(noWait){
        sendMessage(deviceNetworkAddress->node_id, ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET, &attributesValues);
    } else {
        auto_ptr<CDataWrapper> initResult(sendRequest(deviceNetworkAddress->node_id,
                                                      ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                                      &attributesValues,
                                                      millisecToWait));
    }
    return getLastErrorCode();
}

    //------------------------------------
int DeviceMessageChannel::setScheduleDelay(uint64_t scheduledDealy,
                                           uint32_t millisecToWait) {
    CDataWrapper message_data;
    message_data.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceNetworkAddress->device_id);
    message_data.addInt64Value(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, scheduledDealy);
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              NodeDomainAndActionRPC::ACTION_UPDATE_PROPERTY,
                                              &message_data,
                                              millisecToWait));
    return getLastErrorCode();

}

//------------------------------------
void DeviceMessageChannel::sendCustomMessage(const std::string& action_name,
                                             CDataWrapper* const message_data,
                                             bool queued) {
    sendMessage(deviceNetworkAddress->node_id,
                action_name,
				message_data,
				!queued);
}

//------------------------------------
int DeviceMessageChannel::sendCustomRequest(const std::string& action_name,
                                            CDataWrapper* const message_data,
                                            CDataWrapper** result_data,
                                            uint32_t millisec_to_wait,
                                            bool async,
                                            bool queued) {
    auto_ptr<CDataWrapper> result(sendRequest(deviceNetworkAddress->node_id,
                                              action_name,
                                              message_data,
                                              millisec_to_wait,
                                              async,
                                              !queued));
    if(getLastErrorCode() == ErrorCode::EC_NO_ERROR) {
        *result_data = result.release();
    }
    return getLastErrorCode();
}

//------------------------------------
std::auto_ptr<MessageRequestFuture>  DeviceMessageChannel::sendCustomRequestWithFuture(const std::string& action_name,
                                                                                       common::data::CDataWrapper *request_data) {
    return sendRequestWithFuture(deviceNetworkAddress->node_id,
                                 action_name,
                                 request_data);
}
