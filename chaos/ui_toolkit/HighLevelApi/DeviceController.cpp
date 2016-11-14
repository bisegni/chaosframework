/*
 *	DeviceLiveDataFetcher.cpp
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

#include "DeviceController.h"
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::batch_command;

#define MSEC_WAIT_OPERATION 1000


//---------------------------------------------------------------------------------------------------
DeviceController::DeviceController(string& _deviceID):
device_id(_deviceID),
datasetDB(true) {
    
    const auto_ptr<chaos::common::data::CDataWrapper> d;
    mdsChannel = NULL;
    deviceChannel = NULL;
    ioLiveDataDriver = NULL;
    millisecToWait = MSEC_WAIT_OPERATION;
    channel_keys.push_back(device_id + DataPackPrefixID::OUTPUT_DATASET_POSTFIX);
    channel_keys.push_back(device_id + DataPackPrefixID::INPUT_DATASET_POSTFIX);
    channel_keys.push_back(device_id + DataPackPrefixID::CUSTOM_DATASET_POSTFIX);
    channel_keys.push_back(device_id + DataPackPrefixID::SYSTEM_DATASET_POSTFIX);
    channel_keys.push_back(device_id + DataPackPrefixID::HEALTH_DATASET_POSTFIX);
    channel_keys.push_back(device_id + DataPackPrefixID::ALARM_DATASET_POSTFIX);
    //  current_dataset.push_back(d);
    for(int cnt=0;cnt<channel_keys.size();cnt++)
        current_dataset.push_back(boost::shared_ptr<chaos::common::data::CDataWrapper>());
    
}

//---------------------------------------------------------------------------------------------------
DeviceController::~DeviceController() {
    LDBG_<<"["<<__PRETTY_FUNCTION__<<"] remove Device Controller:"<<device_id;
    stopTracking();
    
    if(mdsChannel){
        LLRpcApi::getInstance()->deleteMessageChannel(mdsChannel);
    }
    
    
    if(deviceChannel){
        LLRpcApi::getInstance()->deleteMessageChannel(deviceChannel);
        
    }
    
    if(ioLiveDataDriver){
        ioLiveDataDriver->deinit();
        delete(ioLiveDataDriver);
    }
}

//---------------------------------------------------------------------------------------------------
void DeviceController::setRequestTimeWaith(uint32_t newMillisecToWait){
    millisecToWait = newMillisecToWait;
}

//---------------------------------------------------------------------------------------------------
uint32_t DeviceController::getRequestTimeWaith(){
    return millisecToWait;
}

//---------------------------------------------------------------------------------------------------
void DeviceController::getDeviceId(string& dId) {
    dId = device_id;
}

//---------------------------------------------------------------------------------------------------
void DeviceController::updateChannel() throw(CException) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper *tmp_data_handler = NULL;
    CDeviceNetworkAddress *devAddress = NULL;
    //make the live driver
    if(!mdsChannel){
        mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
        if(!mdsChannel) throw CException(-1, "No MDS Channel created", "DeviceController::init");
    }
    
    err = mdsChannel->getLastDatasetForDevice(device_id, &tmp_data_handler, millisecToWait);
    if(err!=ErrorCode::EC_NO_ERROR || !tmp_data_handler) throw CException(-2, "No device dataset received", "DeviceController::updateChannel");
    
    auto_ptr<CDataWrapper> lastDeviceDefinition(tmp_data_handler);
    
    datasetDB.addAttributeToDataSetFromDataWrapper(*lastDeviceDefinition.get());
    
    err = mdsChannel->getNetworkAddressForDevice(device_id, &devAddress, millisecToWait);
    if(err!=ErrorCode::EC_NO_ERROR || !devAddress) throw CException(-3, "No Address found for device", "DeviceController::init");
    
    //update live data driver
    if(!ioLiveDataDriver) {
        ioLiveDataDriver = LLRpcApi::getInstance()->getDataProxyChannelNewInstance();
        if(ioLiveDataDriver) {
            ioLiveDataDriver->init(NULL);
            if(!mdsChannel->getDataDriverBestConfiguration(&tmp_data_handler, millisecToWait)){
                auto_ptr<CDataWrapper> best_available_da_ptr(tmp_data_handler);
                ioLiveDataDriver->updateConfiguration(best_available_da_ptr.get());
            }
        }
    }
    
    
    //allocate device channel the memory of the CDeviceNetworkAddress * is managed by channel
    if(!deviceChannel){
        deviceChannel = LLRpcApi::getInstance()->getNewDeviceMessageChannel(devAddress);
        if(!deviceChannel) throw CException(-4, "Invalid device channel created", "DeviceController::init");
    }else{
        deviceChannel->setNewAddress(devAddress);
    }
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setScheduleDelay(uint64_t microseconds) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->setScheduleDelay(microseconds, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName) {
    datasetDB.getDatasetAttributesName(attributesName);
}

//---------------------------------------------------------------------------------------------------
void DeviceController::getAttributeDescription(const string& attributesName, string& attributeDescription) {
    datasetDB.getAttributeDescription(attributesName, attributeDescription);
}

//---------------------------------------------------------------------------------------------------
void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType) {
    datasetDB.getDatasetAttributesName(directionType, attributesName);
}

//---------------------------------------------------------------------------------------------------
void DeviceController::getDeviceAttributeRangeValueInfo(const string& attributesName, chaos::common::data::RangeValueInfo& rangeInfo) {
    datasetDB.getAttributeRangeValueInfo(attributesName, rangeInfo);
}
//---------------------------------------------------------------------------------------------------
int DeviceController::getDeviceAttributeDirection(const string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {
    return datasetDB.getAttributeDirection(attributesName, directionType);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::getAttributeStrValue(string attribute_name, string& attribute_value) {
    int err = 0;
    CDataWrapper *  dataWrapper = getLiveCDataWrapperPtr();
    if(dataWrapper) {
        DataType::DataType attributeType;
        err = getDeviceAttributeType(attribute_name, attributeType);
        if(err == 0){
            switch (attributeType) {
                case DataType::TYPE_INT64:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getInt64Value(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_INT32:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getInt64Value(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_DOUBLE:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getDoubleValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_STRING:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getStringValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_BOOLEAN:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getBoolValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_BYTEARRAY: {
                    const char *buffer = NULL;
                    int size;
                    buffer = dataWrapper->getBinaryValue(attribute_name.c_str(), size);
                    if(buffer) attribute_value.assign(buffer, size);
                    break;
                }
                    
                default:
                    err = -1;
                    break;
            }
        }
        
    }
    return err;
}
//---------------------------------------------------------------------------------------------------
int DeviceController::getDeviceAttributeType(const string& attr, DataType::DataType& type) {
    int err = 0;
    if(attributeValueMap.count(attr)){
        type = attributeValueMap[attr].valueType;
    } else {
        err = -1;
    }
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::getType(std::string& control_unit_type) {
    int err = 0;
    if(cu_type.empty()) {
        CDataWrapper*tmp=fetchCurrentDatatasetFromDomain(DatasetDomainSystem);
        if(tmp && tmp->hasKey(DataPackSystemKey::DP_SYS_UNIT_TYPE)){
            std::string t=tmp->getCStringValue(DataPackSystemKey::DP_SYS_UNIT_TYPE);
            cu_type = t;
        } else {
            return -1;
        }
        
    }
    control_unit_type = cu_type;
    
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::initDevice() {
    CHAOS_ASSERT(mdsChannel && deviceChannel)
    int err = 0;
    CDataWrapper initConf;
    datasetDB.fillDataWrapperWithDataSetDescription(initConf);
    
    //initialize the devica with the metadataserver data
    err = deviceChannel->initDevice(&initConf, millisecToWait);
    //configure the live data with the same server where the device write
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::startDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->startDevice(millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::stopDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->stopDevice(millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::deinitDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->deinitDevice(millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::restoreDeviceToTag(const std::string& restore_tag) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->restoreDeviceToTag(restore_tag,
                                             millisecToWait);
}
int DeviceController::recoverDeviceFromError() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->recoverDeviceFromError(millisecToWait);
}
//---------------------------------------------------------------------------------------------------
uint64_t DeviceController::getState(CUStateKey::ControlUnitState& deviceState) {
    uint64_t ret=0;
    CDataWrapper*tmp=fetchCurrentDatatasetFromDomain(DatasetDomainHealth);
    deviceState=CUStateKey::UNDEFINED;
    if(tmp && tmp->hasKey(NodeHealtDefinitionKey::NODE_HEALT_STATUS)){
        std::string state=tmp->getCStringValue(NodeHealtDefinitionKey::NODE_HEALT_STATUS);
        if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_START) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STARTING))
            deviceState=CUStateKey::START;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOPING))
            deviceState= CUStateKey::STOP;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_INITING))
            deviceState= CUStateKey::INIT;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINITING)|| (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD))
            deviceState= CUStateKey::DEINIT;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR))
            deviceState= CUStateKey::RECOVERABLE_ERROR;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR))
            deviceState= CUStateKey::FATAL_ERROR;
        
        if(tmp->hasKey(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP)){
            ret = tmp->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP);
        }
    }
    return ret;
}

int DeviceController::getChannelsNum(){
    return channel_keys.size();
}
//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(string& attributeName, int32_t attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(const char *attributeName, int32_t attributeValue) {
    CDataWrapper attributeValuePack;
    attributeValuePack.addInt32Value(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(string& attributeName, double attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(const char *attributeName, double attributeValue) {
    CDataWrapper attributeValuePack;
    attributeValuePack.addDoubleValue(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeToValue(const char *attributeName, const char *attributeValue, bool noWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper attributeValuePack;
    RangeValueInfo range_info;
    
    //get type for the value
    if((err = datasetDB.getAttributeRangeValueInfo(attributeName, range_info)))
        return err;
    
    switch (range_info.valueType) {
        case DataType::TYPE_BOOLEAN: {
            bool boolValuePtr = lexical_cast<bool>(attributeValue);
            attributeValuePack.addBoolValue(attributeName, boolValuePtr);
            break;
        }
        case DataType::TYPE_STRING:{
            attributeValuePack.addStringValue(attributeName, attributeValue);
            break;
        }
        case DataType::TYPE_DOUBLE:{
            double doubleValuePtr = lexical_cast<double>(attributeValue);
            attributeValuePack.addDoubleValue(attributeName, doubleValuePtr);
            break;
        }
        case DataType::TYPE_INT32:{
            int32_t i32ValuePtr = lexical_cast<int32_t>(attributeValue);
            attributeValuePack.addInt32Value(attributeName, i32ValuePtr);
            break;
        }
        case DataType::TYPE_INT64:{
            int64_t i64ValuePtr = lexical_cast<int64_t>(attributeValue);
            attributeValuePack.addInt64Value(attributeName, i64ValuePtr);
            break;
        }
        case DataType::TYPE_BYTEARRAY:{
            //const char *byteArrayValuePtr = static_cast<const char*>(attributeValue);
            //attributeValuePack.addBinaryValue(attributeName, byteArrayValuePtr, bufferValuedDim);
            throw CException(1, "The byte array set is not managed", "DeviceController::setAttributeToValue");
            break;
        }
    }
    return deviceChannel->setAttributeValue(attributeValuePack, noWait, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeToValue(const char *attributeName, void *attributeValue, bool noWait, int32_t bufferValuedDim) {
    RangeValueInfo range_info;
    
    //get type for the value
    datasetDB.getAttributeRangeValueInfo(attributeName, range_info);
    
    //call default API
    return setAttributeToValue(attributeName, range_info.valueType, attributeValue, noWait, bufferValuedDim);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeToValue(const char *attributeName, DataType::DataType attributeType, void *attributeValue, bool noWait, int32_t bufferValuedDim) {
    CDataWrapper attributeValuePack;
    switch (attributeType) {
        case DataType::TYPE_BOOLEAN: {
            bool *boolValuePtr = static_cast<bool *>(attributeValue);
            attributeValuePack.addBoolValue(attributeName, *boolValuePtr);
            break;
        }
        case DataType::TYPE_STRING:{
            const char *strValuePtr = static_cast<const char *>(attributeValue);
            attributeValuePack.addStringValue(attributeName, strValuePtr);
            break;
        }
        case DataType::TYPE_DOUBLE:{
            double *doubleValuePtr = static_cast<double*>(attributeValue);
            attributeValuePack.addDoubleValue(attributeName, *doubleValuePtr);
            break;
        }
        case DataType::TYPE_INT32:{
            int32_t *i32ValuePtr = static_cast<int32_t*>(attributeValue);
            attributeValuePack.addInt32Value(attributeName, *i32ValuePtr);
            break;
        }
        case DataType::TYPE_INT64:{
            int64_t *i64ValuePtr = static_cast<int64_t*>(attributeValue);
            attributeValuePack.addInt64Value(attributeName, *i64ValuePtr);
            break;
        }
        case DataType::TYPE_BYTEARRAY:{
            const char *byteArrayValuePtr = static_cast<const char*>(attributeValue);
            attributeValuePack.addBinaryValue(attributeName, byteArrayValuePtr, bufferValuedDim);
            break;
        }
    }
    return deviceChannel->setAttributeValue(attributeValuePack, noWait, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::submitSlowControlCommand(string commandAlias,
                                               SubmissionRuleType::SubmissionRule submissionRule,
                                               uint32_t priority,
                                               uint64_t& command_id,
                                               uint32_t execution_channel,
                                               uint64_t scheduler_steps_delay,
                                               uint32_t submission_checker_steps_delay,
                                               CDataWrapper *slow_command_data) {
    CDataWrapper local_command_pack;
    CDataWrapper *result_data = NULL;
    int err = ErrorCode::EC_NO_ERROR;
    
    if(slow_command_data) {
        local_command_pack.appendAllElement(*slow_command_data);
    }
    
    // set the default slow command information
    local_command_pack.addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, commandAlias);
    local_command_pack.addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    local_command_pack.addInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32, (uint32_t) priority);
    
    if(execution_channel) local_command_pack.addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    if(scheduler_steps_delay) local_command_pack.addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, scheduler_steps_delay);
    if(submission_checker_steps_delay) local_command_pack.addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, submission_checker_steps_delay);
    
    //err = deviceChannel->setAttributeValue(local_command_pack, false, millisecToWait);
    local_command_pack.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_id);
    err = deviceChannel->sendCustomRequest(ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET, &local_command_pack, &result_data, millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR &&
       result_data &&
       result_data->hasKey(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
        //fill the command id
        command_id = result_data->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    } else {
        if(result_data)
            LERR_<<"missing key:"<<BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64<<" :"<<result_data->getJSONString();
    }
    //forward the request
    if(result_data) delete(result_data);
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::submitSlowControlCommand(string commandAlias,
                                               SubmissionRuleType::SubmissionRule submissionRule,
                                               uint64_t& command_id,
                                               uint32_t execution_channel,
                                               uint64_t scheduler_steps_delay,
                                               uint32_t submission_checker_steps_delay,
                                               CDataWrapper *slow_command_data) {
    CDataWrapper local_command_pack;
    CDataWrapper *result_data = NULL;
    int err = ErrorCode::EC_NO_ERROR;
    
    if(slow_command_data) {
        local_command_pack.appendAllElement(*slow_command_data);
    }
    // set the default slow command information
    local_command_pack.addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, commandAlias);
    local_command_pack.addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    
    if(execution_channel) local_command_pack.addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    if(scheduler_steps_delay) local_command_pack.addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, scheduler_steps_delay);
    if(submission_checker_steps_delay) local_command_pack.addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, (uint32_t) submission_checker_steps_delay);
    
    //forward the request
    local_command_pack.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, device_id);
    err = deviceChannel->sendCustomRequest(ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                           &local_command_pack,
                                           &result_data,
                                           millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR
       && result_data
       && result_data->hasKey(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
        //fill the command id
        command_id = result_data->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    }
    //delete result data
    if(result_data)delete(result_data);
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setSlowCommandFeatures(features::Features& features, bool lock_features, uint32_t execution_channel) {
    CDataWrapper local_command_pack;
    if(features.featuresFlag & features::FeaturesFlagTypes::FF_LOCK_USER_MOD) {
        local_command_pack.addBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
    }
    
    if(features.featuresFlag & features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY) {
        local_command_pack.addInt64Value(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64, features.featureSchedulerStepsDelay);
    }
    
    if(execution_channel) local_command_pack.addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES, &local_command_pack, NULL, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setSlowCommandLockOnFeatures(bool lock_features) {
    CDataWrapper local_command_pack;
    local_command_pack.addBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES, &local_command_pack, NULL, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::getCommandState(CommandState& command_state) {
    CDataWrapper local_command_pack;
    CDataWrapper *result_data = NULL;
    int err = ErrorCode::EC_NO_ERROR;
    local_command_pack.addInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_state.command_id);
    err = deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE, &local_command_pack, &result_data, millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR &&
       result_data) {
        //fill the command state
        command_state.last_event = static_cast<BatchCommandEventType::BatchCommandEventType>(result_data->getUInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_LAST_EVENT_UI32));
        if(command_state.last_event == BatchCommandEventType::EVT_FAULT) {
            command_state.fault_description.code = result_data->getUInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_CODE_UI32);
            command_state.fault_description.description = result_data->getStringValue(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DESC_STR);
            command_state.fault_description.domain = BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR;
        } else {
            command_state.fault_description.code = 0;
            command_state.fault_description.description.clear();
            command_state.fault_description.domain.clear();
        }
    }
    if(result_data) delete(result_data);
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::killCurrentCommand() {
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND, NULL, NULL, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::flushCommandStateHistory() {
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_FLUSH_COMMAND_HISTORY, NULL, NULL, millisecToWait);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::sendCustomRequest(const char * const action, common::data::CDataWrapper * const param, common::data::CDataWrapper**const result, bool async, bool queued) {
    return deviceChannel->sendCustomRequest(action, param, result, millisecToWait, async, queued);
}

//---------------------------------------------------------------------------------------------------
void DeviceController::sendCustomMessage(const char * const action, common::data::CDataWrapper * const param, bool queued) {
    deviceChannel->sendCustomMessage(action, param, !queued);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::checkRPCInformation(CDataWrapper **result_information,
                                          uint32_t timeout) {
    int err = -1;
    std::auto_ptr<MessageRequestFuture> result = deviceChannel->checkRPCInformation();
    if(result.get() == NULL) return -1;
    if(result->wait(timeout)) {
        err = result->getError();
        if(err == 0) {
            *result_information = result->detachResult();
        }
    }else{
        err = -2;
    }
    return err;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::echoTest(CDataWrapper * const echo_data,
                               CDataWrapper **echo_data_result,
                               uint32_t timeout) {
    int err = -1;
    std::auto_ptr<MessageRequestFuture> result = deviceChannel->echoTest(echo_data);
    if(result.get() == NULL) return err;
    if(result->wait(timeout)) {
        err = result->getError();
        if(err == 0) {
            *echo_data_result = result->detachResult();
        }
    }else{
        err = -2;
    }
    return err;
}

//---------------------------------------------------------------------------------------------------
std::auto_ptr<MessageRequestFuture> DeviceController::sendCustomRequestWithFuture(const std::string& action_name,
                                                                                  common::data::CDataWrapper *request_date) {
    return deviceChannel->sendCustomRequestWithFuture(action_name,
                                                      request_date);
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(string& attributeName, string& attributeValue) {
    return setAttributeValue(attributeName, attributeValue.c_str(),(uint32_t)attributeValue.size());
}

//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(string& attributeName, const char* attributeValue) {
    return setAttributeValue(attributeName, attributeValue,(uint32_t)strlen(attributeValue));
}

std::vector<chaos::common::data::RangeValueInfo> DeviceController::getDeviceValuesInfo(){
    std::vector<chaos::common::data::RangeValueInfo> ret;
    for(std::map<std::string,chaos::common::data::RangeValueInfo>::iterator i= attributeValueMap.begin();i!=attributeValueMap.end();i++)
        ret.push_back(i->second);
    
    return ret;
}
//---------------------------------------------------------------------------------------------------
int DeviceController::setAttributeValue(string& attributeName, const char* attributeValue, uint32_t size) {
    CDataWrapper attributeValuePack;
    const char *attrname=attributeName.c_str();
    
    if(attributeValueMap.find(attributeName) == attributeValueMap.end() )
        return ErrorCode::EC_ATTRIBUTE_NOT_FOUND;
    
    if(attributeValueMap[attributeName].dir==DataType::Output)
        return ErrorCode::EC_ATTRIBUTE_BAD_DIR;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_INT64:
            attributeValuePack.addInt64Value(attrname, boost::lexical_cast<int64_t>(attributeValue));
            return deviceChannel->setAttributeValue(attributeValuePack,millisecToWait);
            
        case DataType::TYPE_INT32:
            attributeValuePack.addInt32Value(attrname, boost::lexical_cast<int32_t>(attributeValue));
            return deviceChannel->setAttributeValue(attributeValuePack,millisecToWait);
            
        case DataType::TYPE_DOUBLE:
            attributeValuePack.addDoubleValue(attrname, boost::lexical_cast<double>(attributeValue));
            return deviceChannel->setAttributeValue(attributeValuePack,millisecToWait);
            
        case DataType::TYPE_BYTEARRAY:
            attributeValuePack.addBinaryValue(attrname,attributeValue,size);
            return deviceChannel->setAttributeValue(attributeValuePack,millisecToWait);
            
        case DataType::TYPE_STRING:
            attributeValuePack.addStringValue(attrname,attributeValue);
            return deviceChannel->setAttributeValue(attributeValuePack,millisecToWait);
        default:
            break;
    };
    return ErrorCode::EC_ATTRIBUTE_TYPE_NOT_SUPPORTED;
}

//---------------------------------------------------------------------------------------------------
void DeviceController::initializeAttributeIndexMap() {
    //	boost::mutex::scoped_lock lock(trackMutext);
    vector<string> attributeNames;
    RangeValueInfo attributerangeInfo;
    
    attributeValueMap.clear();
    
    
    //get all attribute name from db
    datasetDB.getDatasetAttributesName(DataType::Input, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        if(datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo)!=0){
            LERR_<<"CANNOT RETRIVE attr range info of:"<<*iter;
        }
        LDBG_<<"IN attr:"<<attributerangeInfo.name<<" type:"<<attributerangeInfo.valueType<<" bin type:"<<attributerangeInfo.binType;
        attributeValueMap.insert(make_pair(*iter, attributerangeInfo));
        
    }
    
    attributeNames.clear();
    datasetDB.getDatasetAttributesName(DataType::Output, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        
        if(datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo)!=0){
            LERR_<<"CANNOT RETRIVE attr range info of:"<<*iter;
            
        }
        LDBG_<<"OUT attr:"<<attributerangeInfo.name<<" type:"<<attributerangeInfo.valueType<<" bin type:"<<attributerangeInfo.binType;
        attributeValueMap.insert(make_pair(*iter, attributerangeInfo));
        
    }
    
}

//---------------------------------------------------------------------------------------------------
void DeviceController::allocateNewLiveBufferForAttributeAndType(string& attributeName,
                                                                DataType::DataSetAttributeIOAttribute attributeDirection,
                                                                DataType::DataType attrbiuteType) {
    //boost::mutex::scoped_lock  lock(trackMutext);
    if(attributeDirection == DataType::Output ||
       attributeDirection == DataType::Bidirectional ){
        
        switch (attributeValueMap[attributeName].valueType) {
                
                
            case DataType::TYPE_INT32:{
                SingleBufferCircularBuffer<int32_t> *newBuffer = new SingleBufferCircularBuffer<int32_t>(30);
                int32AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_INT64:{
                SingleBufferCircularBuffer<int64_t> *newBuffer = new SingleBufferCircularBuffer<int64_t>(30);
                int64AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_DOUBLE:{
                SingleBufferCircularBuffer<double_t> *newBuffer = new SingleBufferCircularBuffer<double_t>(30);
                doubleAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
            case DataType::TYPE_BYTEARRAY:{
                PointerBuffer *newBuffer = new PointerBuffer();
                pointerAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
        }
    } else if(attributeDirection == DataType::Input ||
              attributeDirection == DataType::Bidirectional ){
        
    }
    
}

//---------------------------------------------------------------------------------------------------
DataBuffer *DeviceController::getBufferForAttribute(string& attributeName) {
    boost::mutex::scoped_lock lock(trackMutext);
    DataBuffer * result = NULL;
    //allocate attribute traccking
    
    if(attributeValueMap.count(attributeName) == 0  ) return result;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_INT32:
            result = int32AttributeLiveBuffer[attributeName];
            break;
            
        case DataType::TYPE_INT64:
            result = int64AttributeLiveBuffer[attributeName];
            break;
            
        case DataType::TYPE_DOUBLE:
            result = doubleAttributeLiveBuffer[attributeName];
            break;
    }
    return result;
}

//---------------------------------------------------------------------------------------------------
PointerBuffer *DeviceController::getPtrBufferForAttribute(string& attributeName) {
    boost::mutex::scoped_lock lock(trackMutext);
    PointerBuffer * result = NULL;
    //allocate attribute traccking
    if(attributeValueMap.count(attributeName) == 0  ) return result;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_BYTEARRAY:
            result = pointerAttributeLiveBuffer[attributeName];
            break;
        default:
            break;
    }
    return result;
}

//---------------------------------------------------------------------------------------------------
DataBuffer *DeviceController::getPtrBufferForTimestamp(const int initialDimension) {
    return int64AttributeLiveBuffer.count(DataPackCommonKey::DPCK_TIMESTAMP)>0? int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]:NULL;
}

//---------------------------------------------------------------------------------------------------
void DeviceController::deinitializeAttributeIndexMap() {
    //	boost::mutex::scoped_lock lock(trackMutext);
    //dispose circula buffer
    for (std::map<string,  SingleBufferCircularBuffer<int32_t> *>::iterator iter = int32AttributeLiveBuffer.begin();
         iter != int32AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int32AttributeLiveBuffer.clear();
    
    for (std::map<string,  SingleBufferCircularBuffer<int64_t> *>::iterator iter = int64AttributeLiveBuffer.begin();
         iter != int64AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int64AttributeLiveBuffer.clear();
    
    for (std::map<string,  SingleBufferCircularBuffer<double_t> *>::iterator iter = doubleAttributeLiveBuffer.begin();
         iter != doubleAttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    doubleAttributeLiveBuffer.clear();
    
    for (std::map<string,  PointerBuffer*>::iterator iter = pointerAttributeLiveBuffer.begin();
         iter != pointerAttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    pointerAttributeLiveBuffer.clear();
}

//---------------------------------------------------------------------------------------------------
void DeviceController::addAttributeToTrack(string& attr) {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //add attribute name to list of tracking attribute
    trackingAttribute.push_back(attr);
    
    //allocate attribute traccking
    if(attributeValueMap.count(attr) == 0  ) return;
    
    //allcoate the buffer for the new attribute to track
    allocateNewLiveBufferForAttributeAndType(attr, attributeValueMap[attr].dir, attributeValueMap[attr].valueType);
}

//---------------------------------------------------------------------------------------------------
CDataWrapper * DeviceController::getLiveCDataWrapperPtr() {
    return current_dataset[DatasetDomainOutput].get();
}


//---------------------------------------------------------------------------------------------------
CDataWrapper * DeviceController::getCurrentDatasetForDomain(DatasetDomain domain) {
    if(domain<current_dataset.size()){
        return current_dataset[domain].get();
    }
    return NULL;
}

//---------------------------------------------------------------------------------------------------
chaos::common::data::CDataWrapper *  DeviceController::fetchCurrentDatatasetFromDomain(DatasetDomain domain) {
    CHAOS_ASSERT(ioLiveDataDriver)
    char *value = NULL;
    unsigned long value_len = 0;
    
    if(domain<current_dataset.size()){
        value = ioLiveDataDriver->retriveRawData(channel_keys[domain],(size_t*)&value_len);
        if(value){
            chaos::common::data::CDataWrapper *tmp = new CDataWrapper(value);
            current_dataset[domain].reset(tmp);
            free(value);
            return tmp;
        }
    }
    return NULL;
}

//---------------------------------------------------------------------------------------------------
int DeviceController::getTimeStamp(uint64_t& live){
    CDataWrapper * d = current_dataset[DatasetDomainOutput].get();
    live =0;
    if(d){
        live = d->getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
        return 0;
    }
    return -1;
}
//---------------------------------------------------------------------------------------------------
void DeviceController::setupTracking() {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //init live buffer
    initializeAttributeIndexMap();
    
    //initialize timestamp buffer
    SingleBufferCircularBuffer<int64_t> *newBuffer = new SingleBufferCircularBuffer<int64_t>(10);
    int64AttributeLiveBuffer.insert(make_pair(DataPackCommonKey::DPCK_TIMESTAMP, newBuffer));
}

//---------------------------------------------------------------------------------------------------
void DeviceController::stopTracking() {
    boost::mutex::scoped_lock lock(trackMutext);
    deinitializeAttributeIndexMap();
}

//---------------------------------------------------------------------------------------------------
void DeviceController::fetchCurrentDeviceValue() {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //! fetch the output odmain
    fetchCurrentDatatasetFromDomain(DatasetDomainOutput);
    
    if(trackingAttribute.size() == 0) return;
    CDataWrapper *tmpPtr = current_dataset[DatasetDomainOutput].get();
    
    //add timestamp value
    int64_t got_ts = tmpPtr->getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
    if(int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]->getLastValue() == got_ts) return;
    
    int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]->addValue(got_ts);
    
    //update buffer for tracked attribute
    for (std::vector<string>::iterator iter = trackingAttribute.begin();
         iter != trackingAttribute.end();
         iter++) {
        const char *key = (*iter).c_str();
        if(!tmpPtr->hasKey(key)) continue;
        
        switch (attributeValueMap[*iter].valueType) {
                
            case DataType::TYPE_INT32:
                int32AttributeLiveBuffer[*iter]->addValue(tmpPtr->getInt32Value(key));
                break;
                
            case DataType::TYPE_INT64:
                int64AttributeLiveBuffer[*iter]->addValue(tmpPtr->getInt64Value(key));
                break;
                
            case DataType::TYPE_DOUBLE:
                doubleAttributeLiveBuffer[*iter]->addValue(tmpPtr->getDoubleValue(key));
                break;
                
            case DataType::TYPE_BYTEARRAY:
                int32_t ptrLen = 0;
                const char * tmpPtrAttribute = tmpPtr->getBinaryValue(key, ptrLen);
                pointerAttributeLiveBuffer[*iter]->updateData(tmpPtrAttribute, ptrLen);
                break;
        }
    }
}

CDataWrapper *DeviceController::getCurrentData(){
    return current_dataset[DatasetDomainOutput].get();
}

//! get profile info
cu_prof_t DeviceController::getProfileInfo(){
    chaos::common::data::CDataWrapper *prof=  fetchCurrentDatatasetFromDomain(DatasetDomainHealth);
    cu_prof_t p;
    bzero(&p,sizeof(cu_prof_t));
    if(prof){
        p.push_rate=prof->getDoubleValue(ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE);
        p.sys_time =prof->getDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME);
        p.usr_time = prof->getDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME);
        p.upt_time = prof->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME);
        p.metric_time = prof->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP_LAST_METRIC);
    }
    return p;
}

//! get datapack between time itervall
void DeviceController::executeTimeIntervallQuery(DatasetDomain domain,
                                                 uint64_t start_ts,
                                                 uint64_t end_ts,
                                                 QueryCursor **query_cursor,
                                                 uint32_t page) {
    *query_cursor = ioLiveDataDriver->performQuery(channel_keys[domain],
                                                   start_ts,
                                                   end_ts,
                                                   page);
}

//! release a query
void DeviceController::releaseQuery(QueryCursor *query_cursor) {
    ioLiveDataDriver->releaseQuery(query_cursor);
}

int DeviceController::loadDatasetTypeFromSnapshotTag(const std::string& snapshot_tag,
                                                     DatasetDomain dataset_type,
                                                     chaos_data::CDataWrapper **cdatawrapper_handler) {
    return ioLiveDataDriver->loadDatasetTypeFromSnapshotTag(snapshot_tag,
                                                            device_id,
                                                            dataset_type,
                                                            cdatawrapper_handler);
}

int DeviceController::createNewSnapshot(const std::string& snapshot_tag,
                                        const std::vector<std::string>& other_snapped_device) {
    CHAOS_ASSERT(ioLiveDataDriver)
    std::vector<std::string> device_id_in_snap = other_snapped_device;
    device_id_in_snap.push_back(device_id);
    return ioLiveDataDriver->createNewSnapshot(snapshot_tag,
                                               device_id_in_snap);
}

int DeviceController::deleteSnapshot(const std::string& snapshot_tag) {
    CHAOS_ASSERT(ioLiveDataDriver)
    return ioLiveDataDriver->deleteSnapshot(snapshot_tag);
}

int DeviceController::getSnapshotList(ChaosStringVector& snapshot_list) {
    CHAOS_ASSERT(mdsChannel)
    return mdsChannel->searchSnapshotForNode(device_id,
                                             snapshot_list,
                                             millisecToWait);
}

int DeviceController::searchNode(const std::string& unique_id_filter,
                                 unsigned int node_type_filter,
                                 bool alive_only,
                                 unsigned int last_node_sequence_id,
                                 unsigned int page_length,
                                 ChaosStringVector& node_found) {
    
    return mdsChannel->searchNode(unique_id_filter,
                                  node_type_filter,
                                  alive_only,
                                  last_node_sequence_id,
                                  page_length,
                                  node_found,
                                  millisecToWait);
}

