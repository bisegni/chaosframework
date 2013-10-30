/*
 *	DeviceLiveDataFetcher.cpp
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

#include "DeviceController.h"
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/common/io/IOMemcachedDriver.h>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
namespace cccs = chaos::cu::control_manager::slow_command;
namespace cc_data = chaos::common::data;

#define MSEC_WAIT_OPERATION 1000

//timestamp string variable definition
string timestampAttributeNameStr = DataPackKey::CS_CSV_TIME_STAMP;

DeviceController::DeviceController(string& _deviceID):deviceID(_deviceID),datasetDB(true) {
    mdsChannel = NULL;
    deviceChannel = NULL;
    ioLiveDataDriver = NULL;
    millisecToWait = MSEC_WAIT_OPERATION;
    
}

DeviceController::~DeviceController() {
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

void DeviceController::setRequestTimeWaith(uint32_t newMillisecToWait){
    millisecToWait = newMillisecToWait;
}

uint32_t DeviceController::getRequestTimeWaith(){
    return millisecToWait;
}

void DeviceController::getDeviceId(string& dId) {
    dId.assign(deviceID);
}

void DeviceController::updateChannel() throw(CException) {
    int err = ErrorCode::EC_NO_ERROR;
    CDataWrapper *devDefHandler = NULL;
    CDeviceNetworkAddress *devAddress = NULL;
    //make the live driver
    if(!mdsChannel){
        mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
        if(!mdsChannel) throw CException(-1, "No MDS Channel created", "DeviceController::init");
    }
    
    err = mdsChannel->getLastDatasetForDevice(deviceID, &devDefHandler, millisecToWait);
    if(err!=ErrorCode::EC_NO_ERROR || !devDefHandler) throw CException(-2, "No device dataset received", "DeviceController::updateChannel");
    
    auto_ptr<CDataWrapper> lastDeviceDefinition(devDefHandler);
    
    datasetDB.addAttributeToDataSetFromDataWrapper(*lastDeviceDefinition.get());
    
    err = mdsChannel->getNetworkAddressForDevice(deviceID, &devAddress, millisecToWait);
    if(err!=ErrorCode::EC_NO_ERROR || !devAddress) throw CException(-3, "No Address found for device", "DeviceController::init");
    
    //update live data driver
    ioLiveDataDriver = new IOMemcachedDriver();
    if(ioLiveDataDriver) {
        ioLiveDataDriver->init();
        ioLiveDataDriver->updateConfiguration(lastDeviceDefinition.get());
    }
    
    //allocate device channel the memory of the CDeviceNetworkAddress * is managed by channel
    if(!deviceChannel){
        deviceChannel = LLRpcApi::getInstance()->getNewDeviceMessageChannel(devAddress);
        if(!deviceChannel) throw CException(-4, "Invalid device channel created", "DeviceController::init");
    }else{
        deviceChannel->setNewAddress(devAddress);
    }
}

int DeviceController::setScheduleDelay(uint64_t microseconds) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->setScheduleDelay(microseconds, millisecToWait);
}


void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName) {
    datasetDB.getDatasetAttributesName(attributesName);
}

void DeviceController::getAttributeDescription(string& attributesName, string& attributeDescription) {
    datasetDB.getAttributeDescription(attributesName, attributeDescription);
}

/*!
 Get all attribute name
 */
void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType) {
    datasetDB.getDatasetAttributesName(directionType, attributesName);
}

/*!
 Get range valu einfo for attrbiute name
 */
void DeviceController::getDeviceAttributeRangeValueInfo(string& attributesName, chaos::RangeValueInfo& rangeInfo) {
    datasetDB.getAttributeRangeValueInfo(attributesName, rangeInfo);
}

int DeviceController::getDeviceAttributeDirection(string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {
    return datasetDB.getAttributeDirection(attributesName, directionType);
}

/*!
 Get the direction of the attribute
 */
int DeviceController::getDeviceAttributeType(string& attributesName, DataType::DataType& type) {
    int err = 0;
    if(attributeTypeMap.count(attributesName)){
        type = attributeTypeMap[attributesName];
    } else {
        err = -1;
    }
    return err;
}

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


int DeviceController::startDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->startDevice(millisecToWait);
}

int DeviceController::stopDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->stopDevice(millisecToWait);
}

int DeviceController::deinitDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->deinitDevice(millisecToWait);
}

//!Get device state
/*!
 Return the current device state
 */
int DeviceController::getState(CUStateKey::ControlUnitState& deviceState) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->getState(deviceState, millisecToWait);
}


int DeviceController::setAttributeValue(string& attributeName, int32_t attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

int DeviceController::setAttributeValue(const char *attributeName, int32_t attributeValue) {
    CDataWrapper attributeValuePack;
    attributeValuePack.addInt32Value(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack, millisecToWait);
}

int DeviceController::setAttributeValue(string& attributeName, double attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

int DeviceController::setAttributeValue(const char *attributeName, double attributeValue) {
    CDataWrapper attributeValuePack;
    attributeValuePack.addDoubleValue(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack, millisecToWait);
}

int DeviceController::setAttributeToValue(const char *attributeName, const char *attributeValue, bool noWait) {
	int err = ErrorCode::EC_NO_ERROR;
	CDataWrapper attributeValuePack;
	cc_data::RangeValueInfo range_info;
	
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
        case DataType::TYPE_STRUCT:{
            CDataWrapper cdValue(attributeValue);
            attributeValuePack.appendAllElement(cdValue);
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

int DeviceController::setAttributeToValue(const char *attributeName, void *attributeValue, bool noWait, int32_t bufferValuedDim) {
	cc_data::RangeValueInfo range_info;

	//get type for the value
	datasetDB.getAttributeRangeValueInfo(attributeName, range_info);
	
	//call default API
	return setAttributeToValue(attributeName, range_info.valueType, attributeValue, noWait, bufferValuedDim);
}

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
        case DataType::TYPE_STRUCT:{
            CDataWrapper *cdValuePtr = static_cast<CDataWrapper*>(attributeValue);
            attributeValuePack.appendAllElement(*cdValuePtr);
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

int DeviceController::submitSlowControlCommand(string commandAlias,
											   cccs::SubmissionRuleType::SubmissionRule submissionRule,
											   uint32_t priority,
											   uint64_t& command_id,
											   uint32_t schedulerStepsDelay,
											   uint32_t submissionCheckerStepsDelay,
											   CDataWrapper *slowCommandData) {
    CDataWrapper localCommandPack;
	CDataWrapper *resultData = NULL;
	int err = ErrorCode::EC_NO_ERROR;
	
    if(slowCommandData) {
        localCommandPack.appendAllElement(*slowCommandData);
    }
    
    
    // set the default slow command information
    localCommandPack.addStringValue(cccs::SlowCommandSubmissionKey::COMMAND_ALIAS_STR, commandAlias);
    localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32, (uint32_t) priority);
    if(schedulerStepsDelay) localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI32, (uint32_t) schedulerStepsDelay);
    if(submissionCheckerStepsDelay) localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, (uint32_t) submissionCheckerStepsDelay);
	
	//err = deviceChannel->setAttributeValue(localCommandPack, false, millisecToWait);
	localCommandPack.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceID);
	err = deviceChannel->sendCustomRequest("setDatasetAttribute", &localCommandPack, &resultData, millisecToWait);
	if(err == ErrorCode::EC_NO_ERROR && resultData && resultData->hasKey(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
		//fill the command id
		command_id = resultData->getUInt64Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
		delete(resultData);
	}
    //forward the request
    return err;
}

int DeviceController::submitSlowControlCommand(string commandAlias,
											   cccs::SubmissionRuleType::SubmissionRule submissionRule,
											   uint64_t& command_id,
											   uint32_t schedulerStepsDelay,
											   uint32_t submissionCheckerStepsDelay,
											   CDataWrapper *slowCommandData) {
    CDataWrapper localCommandPack;
	CDataWrapper *resultData = NULL;
	int err = ErrorCode::EC_NO_ERROR;
	
    if(slowCommandData) {
        localCommandPack.appendAllElement(*slowCommandData);
    }
    // set the default slow command information
    localCommandPack.addStringValue(cccs::SlowCommandSubmissionKey::COMMAND_ALIAS_STR, commandAlias);
    localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    if(schedulerStepsDelay) localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI32, (uint32_t) schedulerStepsDelay);
    if(submissionCheckerStepsDelay) localCommandPack.addInt32Value(cccs::SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, (uint32_t) submissionCheckerStepsDelay);

    //forward the request
	localCommandPack.addStringValue(DatasetDefinitionkey::DEVICE_ID, deviceID);
	err = deviceChannel->sendCustomRequest("setDatasetAttribute", &localCommandPack, &resultData, millisecToWait);
	if(err == ErrorCode::EC_NO_ERROR && resultData && resultData->hasKey(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
		//fill the command id
		command_id = resultData->getUInt64Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
		delete(resultData);
	}
    //forward the request
    return err;
}

int DeviceController::setSlowCommandFeatures(cccs::features::Features& features, bool lock_features) {
	CDataWrapper localCommandPack;
	if(features.featuresFlag & cccs::features::FeaturesFlagTypes::FF_LOCK_USER_MOD) {
		localCommandPack.addBoolValue(cccs::SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
	}
	
	if(features.featuresFlag & cccs::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY) {
		localCommandPack.addInt32Value(cccs::SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI32, features.featureSchedulerStepsDelay);
	}
	
	return deviceChannel->sendCustomRequest(cccs::SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES, &localCommandPack, NULL, millisecToWait);
}

//! Set the lock on slow command features
int DeviceController::setSlowCommandLockOnFeatures(bool lock_features) {
	CDataWrapper localCommandPack;
	localCommandPack.addBoolValue(cccs::SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
	return deviceChannel->sendCustomRequest(cccs::SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES, &localCommandPack, NULL, millisecToWait);
}

//! Get the statistick for a command
int DeviceController::getCommandState(cccs::CommandState& command_state) {
	CDataWrapper localCommandPack;
	CDataWrapper *resultData = NULL;
	int err = ErrorCode::EC_NO_ERROR;
	localCommandPack.addInt64Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_state.command_id);
	err = deviceChannel->sendCustomRequest(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE, &localCommandPack, &resultData, millisecToWait);
	if(err == ErrorCode::EC_NO_ERROR && resultData) {
		//fill the command state
		command_state.last_event = static_cast<cccs::SlowCommandEventType::SlowCommandEventType>(resultData->getUInt32Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_LAST_EVENT_UI32));
		if(command_state.last_event == cccs::SlowCommandEventType::EVT_FAULT) {
			command_state.fault_description.code = resultData->getUInt32Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_CODE_UI32);
			command_state.fault_description.description = resultData->getStringValue(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DESC_STR);
			command_state.fault_description.domain = resultData->getStringValue(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR);
		} else {
			command_state.fault_description.code = 0;
			command_state.fault_description.description.clear();
			command_state.fault_description.domain.clear();
		}
		delete(resultData);
	}
	return err;
}

//! Kill the current executing command
int DeviceController::killCurrentCommand() {
	return deviceChannel->sendCustomRequest(cccs::SlowControlExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND, NULL, NULL, millisecToWait);
}

//! Flush command states history
int DeviceController::flushCommandStateHistory() {
	return deviceChannel->sendCustomRequest(cccs::SlowControlExecutorRpcActionKey::RPC_FLUSH_COMMAND_HISTORY, NULL, NULL, millisecToWait);
}

int DeviceController::setAttributeValue(string& attributeName, string& attributeValue) {
    return setAttributeValue(attributeName, attributeValue.c_str(),(uint32_t)attributeValue.size());
}

int DeviceController::setAttributeValue(string& attributeName, const char* attributeValue) {
    return setAttributeValue(attributeName, attributeValue,(uint32_t)strlen(attributeValue));
}

int DeviceController::setAttributeValue(string& attributeName, const char* attributeValue, uint32_t size) {
    CDataWrapper attributeValuePack;
    const char *attrname=attributeName.c_str();
    
    if(attributeTypeMap.find(attributeName) == attributeTypeMap.end() )
        return ErrorCode::EC_ATTRIBUTE_NOT_FOUND;
    
    if(attributeDirectionMap[attributeName]==DataType::Output)
        return ErrorCode::EC_ATTRIBUTE_BAD_DIR;
    
    switch (attributeTypeMap[attributeName]) {
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

/*!
 Initialize the map for the devices
 \param initiDevicedescription the reference to CDataWrapper that contain device initialization information
 */
void DeviceController::initializeAttributeIndexMap() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    vector<string> attributeNames;
    RangeValueInfo attributerangeInfo;
    
    attributeTypeMap.clear();
    attributeDirectionMap.clear();
    
    //get all attribute name from db
    datasetDB.getDatasetAttributesName(DataType::Input, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo);
        attributeDirectionMap.insert(make_pair(*iter, (DataType::DataSetAttributeIOAttribute)DataType::Input));
        attributeTypeMap.insert(make_pair(*iter, (DataType::DataType)attributerangeInfo.valueType));
    }
    
    attributeNames.clear();
    datasetDB.getDatasetAttributesName(DataType::Output, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo);
        attributeDirectionMap.insert(make_pair(*iter, (DataType::DataSetAttributeIOAttribute)DataType::Output));
        attributeTypeMap.insert(make_pair(*iter, (DataType::DataType)attributerangeInfo.valueType));
    }
    
}

//! allocata new circular buffer for attribute and type
/*
 
 */
void DeviceController::allocateNewLiveBufferForAttributeAndType(string& attributeName, DataType::DataSetAttributeIOAttribute attributeDirection, DataType::DataType attrbiuteType) {
    boost::recursive_mutex::scoped_lock  lock(trackMutext);
    if(attributeDirection == DataType::Output ||
       attributeDirection == DataType::Bidirectional ){
        
        switch (attributeTypeMap[attributeName]) {
                
            case DataType::TYPE_INT32:{
                chaos::SingleBufferCircularBuffer<int32_t> *newBuffer = new chaos::SingleBufferCircularBuffer<int32_t>(30);
                int32AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_INT64:{
                chaos::SingleBufferCircularBuffer<int64_t> *newBuffer = new chaos::SingleBufferCircularBuffer<int64_t>(30);
                int64AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_DOUBLE:{
                chaos::SingleBufferCircularBuffer<double_t> *newBuffer = new chaos::SingleBufferCircularBuffer<double_t>(30);
                doubleAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
            case DataType::TYPE_BYTEARRAY:{
                chaos::PointerBuffer *newBuffer = new chaos::PointerBuffer();
                pointerAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
        }
    } else if(attributeDirection == DataType::Input ||
              attributeDirection == DataType::Bidirectional ){
        
    }
    
}


chaos::DataBuffer *DeviceController::getBufferForAttribute(string& attributeName) {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    chaos::DataBuffer * result = NULL;
    //allocate attribute traccking
    if(attributeTypeMap.count(attributeName) == 0 || attributeDirectionMap.count(attributeName) == 0 ) return result;
    
    switch (attributeTypeMap[attributeName]) {
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

chaos::PointerBuffer *DeviceController::getPtrBufferForAttribute(string& attributeName) {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    chaos::PointerBuffer * result = NULL;
    //allocate attribute traccking
    if(attributeTypeMap.count(attributeName) == 0 || attributeDirectionMap.count(attributeName) == 0 ) return result;
    
    switch (attributeTypeMap[attributeName]) {
        case DataType::TYPE_BYTEARRAY:
            result = pointerAttributeLiveBuffer[attributeName];
            break;
    }
    return result;
}

chaos::DataBuffer *DeviceController::getPtrBufferForTimestamp(const int initialDimension) {
    return int64AttributeLiveBuffer.count(timestampAttributeNameStr)>0? int64AttributeLiveBuffer[timestampAttributeNameStr]:NULL;
}

//!DeInitialize the map for the devices
/*!
 Dispose all memory used for live data buffer
 */
void DeviceController::deinitializeAttributeIndexMap() {
    //boost::recursive_mutex::scoped_lock lock(trackMutext);
    //dispose circula buffer
    for (std::map<string,  chaos::SingleBufferCircularBuffer<int32_t> *>::iterator iter = int32AttributeLiveBuffer.begin();
         iter != int32AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int32AttributeLiveBuffer.clear();
    
    for (std::map<string,  chaos::SingleBufferCircularBuffer<int64_t> *>::iterator iter = int64AttributeLiveBuffer.begin();
         iter != int64AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int64AttributeLiveBuffer.clear();
    
    for (std::map<string,  chaos::SingleBufferCircularBuffer<double_t> *>::iterator iter = doubleAttributeLiveBuffer.begin();
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
//add attrbiute to track
/*!
 Add attribute to tracking
 */
void DeviceController::addAttributeToTrack(string& attrbiuteName) {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    
    //add attribute name to list of tracking attribute
    trackingAttribute.push_back(attrbiuteName);
    
    //allocate attribute traccking
    if(attributeTypeMap.count(attrbiuteName) == 0 || attributeDirectionMap.count(attrbiuteName) == 0 ) return;
    
    //allcoate the buffer for the new attribute to track
    allocateNewLiveBufferForAttributeAndType(attrbiuteName, attributeDirectionMap[attrbiuteName], attributeTypeMap[attrbiuteName]);
}

//get the CDatawrapper for the live value
/*!
 the returned object is not own by requester but only by DeviceController isntance
 */
CDataWrapper * DeviceController::getLiveCDataWrapperPtr() {
    return currentLiveValue.get();
}

void DeviceController::setupTracking() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    
    //init live buffer
    initializeAttributeIndexMap();
    
    //initialize timestamp buffer
    chaos::SingleBufferCircularBuffer<int64_t> *newBuffer = new chaos::SingleBufferCircularBuffer<int64_t>(10);
    int64AttributeLiveBuffer.insert(make_pair(timestampAttributeNameStr, newBuffer));
}

void DeviceController::stopTracking() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    deinitializeAttributeIndexMap();
}

void DeviceController::fetchCurrentDeviceValue() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    
    char *value = ioLiveDataDriver->retriveRawData();
    //check if some value has bee fetcher
    if(!value) return;
    
    currentLiveValue.reset(new CDataWrapper(value));
    free(value);
    
    if(trackingAttribute.size() == 0) return;
    CDataWrapper *tmpPtr = currentLiveValue.get();
    
    //add timestamp value
    int64AttributeLiveBuffer[timestampAttributeNameStr]->addValue(tmpPtr->getInt64Value(DataPackKey::CS_CSV_TIME_STAMP));
    
    //update buffer for tracked attribute
    for (std::vector<string>::iterator iter = trackingAttribute.begin();
         iter != trackingAttribute.end();
         iter++) {
        const char *key = (*iter).c_str();
        if(!tmpPtr->hasKey(key)) continue;
        
        switch (attributeTypeMap[*iter]) {
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
