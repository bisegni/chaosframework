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

using namespace chaos;
using namespace chaos::ui;
using namespace std;
using namespace std;

#define MSEC_WAIT_OPERATION 1000

DeviceController::DeviceController(string& _deviceID):deviceID(_deviceID) {
    mdsChannel = NULL;
    deviceChannel = NULL;
    ioLiveDataDriver = NULL;
    millisecToWaitOnOperation = MSEC_WAIT_OPERATION;
}

DeviceController::~DeviceController() {
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

void DeviceController::getDeviceId(string& dId) {
    dId.assign(deviceID);
}

void DeviceController::updateChannel() throw(CException) {
    
        //make the live driver    
    if(!mdsChannel){
        mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
        if(!mdsChannel) throw CException(-1, "No MDS Channel created", "DeviceController::init");
    }
    
    lastDeviceDefinition.reset(mdsChannel->getLastDatasetForDevice(deviceID, millisecToWaitOnOperation));
    if(!lastDeviceDefinition.get()) throw CException(-2, "No device dataset received", "DeviceController::updateChannel");
        
    datasetDB.addAttributeToDataSetFromDataWrapper(*lastDeviceDefinition.get());
    
    deviceAddress.reset(mdsChannel->getNetworkAddressForDevice(deviceID, millisecToWaitOnOperation));
    if(!deviceAddress.get()) throw CException(-3, "No Address found for device", "DeviceController::init");
        
        //update live data driver
    ioLiveDataDriver = new IOMemcachedDriver();
    if(ioLiveDataDriver) {
        ioLiveDataDriver->init();
        ioLiveDataDriver->updateConfiguration(lastDeviceDefinition.get());
    }

        //allocate device channel
    if(!deviceChannel){
        deviceChannel = LLRpcApi::getInstance()->getNewDeviceMessageChannel(deviceAddress.get());
        if(!deviceChannel) throw CException(-4, "Invalid device channel created", "DeviceController::init");
    }else{
        deviceChannel->setNewAddress(deviceAddress.get());  
    }
}

int DeviceController::setScheduleDelay(int32_t millisecDelay) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->setScheduleDelay(millisecDelay);
}


void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName) {
    datasetDB.getDeviceDatasetAttributesName(deviceID, attributesName);
}

void DeviceController::getAttributeDescription(string& attributesName, string& attributeDescription) {
    datasetDB.getDeviceAttributeDescription(deviceID, attributesName, attributeDescription);
}

/*!
 Get all attribute name
 */
void DeviceController::getDeviceDatasetAttributesName(vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType) {
     datasetDB.getDeviceDatasetAttributesName(deviceID, attributesName, directionType);
}

/*!
 Get range valu einfo for attrbiute name
 */
void DeviceController::getDeviceAttributeRangeValueInfo(string& attributesName, CUSchemaDB::RangeValueInfo& rangeInfo) {
    datasetDB.getDeviceAttributeRangeValueInfo(deviceID, attributesName, rangeInfo);
}

int DeviceController::getDeviceAttributeDirection(string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {
    return datasetDB.getDeviceAttributeDirection(deviceID, attributesName, directionType);
}

int DeviceController::initDevice() {
    CHAOS_ASSERT(mdsChannel && deviceChannel)
    int err = 0;
    if(!lastDeviceDefinition.get()) return -1;
    
        //initialize the devica with the metadataserver data
    err = deviceChannel->initDevice(lastDeviceDefinition.get());
        //configure the live data with the same server where the device write
    return err;
}


int DeviceController::startDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->startDevice();    
}

int DeviceController::stopDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->stopDevice();  
}

int DeviceController::deinitDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->deinitDevice(); 
}

    //!Get device state
/*!
 Return the current device state
 */
int DeviceController::getState(CUStateKey::ControlUnitState& deviceState) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->getState(deviceState); 
}

int DeviceController::setInt32AttributeValue(string& attributeName, int32_t attributeValue) {
    CDataWrapper attributeValuePack;
    //attributeValuePack.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, attributeName);
    //attributeValuePack.addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_VALUE, attributeValue);
    attributeValuePack.addInt32Value(attributeName.c_str(), attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack);
}

int DeviceController::setDoubleAttributeValue(string& attributeName, double_t attributeValue) {
    CDataWrapper attributeValuePack;
    //attributeValuePack.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, attributeName);
    //attributeValuePack.addDoubleValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_VALUE, attributeValue);
    attributeValuePack.addDoubleValue(attributeName.c_str(), attributeValue);
    return deviceChannel->setAttributeValue(attributeValuePack);
}

/*!
 Initialize the map for the devices
 \param initiDevicedescription the reference to CDataWrapper that contain device initialization information
 */
void DeviceController::initializeAttributeIndexMap(CDataWrapper& initiDevicedescription) {
         boost::recursive_mutex::scoped_lock lock(trackMutext);
    if(!initiDevicedescription.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID) || !initiDevicedescription.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
        return;
    }
    string attributeName;
    int32_t attributeType = 0;
    int32_t attributeDirection = 0;
    auto_ptr<CDataWrapper> attributeDescription;
    
    attributeTypeMap.clear();
    attributeDirectionMap.clear();
    auto_ptr<CMultiTypeDataArrayWrapper> datasetDesription(initiDevicedescription.getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
    for (int idx=0; idx < datasetDesription->size(); idx++) {
        attributeDescription.reset(datasetDesription->getCDataWrapperElementAtIndex(idx));
        if(!attributeDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME) || 
           !attributeDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION) ||
           !attributeDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE)) continue;
        
            //alocate the buffer for realtime data
        attributeName = attributeDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME);
        attributeType = attributeDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE);
        attributeDirection = attributeDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION);
        
        attributeDirectionMap.insert(make_pair(attributeName, (DataType::DataSetAttributeIOAttribute)attributeDirection));
        attributeTypeMap.insert(make_pair(attributeName, (DataType::DataType)attributeType));
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
    //!DeInitialize the map for the devices
/*!
 Dispose all memory used for live data buffer
 */
void DeviceController::deinitializeAttributeIndexMap() {
         boost::recursive_mutex::scoped_lock lock(trackMutext);
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
CDataWrapper* DeviceController::getLiveCDataWrapperPtr() {
    return currentLiveValue.get();
}

void DeviceController::setupTracking() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    CHAOS_ASSERT(lastDeviceDefinition.get())
    
        //init live buffer
    initializeAttributeIndexMap(*lastDeviceDefinition.get());
}

void DeviceController::stopTracking() {
    boost::recursive_mutex::scoped_lock lock(trackMutext);
    deinitializeAttributeIndexMap();
}

void DeviceController::fetchCurrentDeviceValue() {
     boost::recursive_mutex::scoped_lock lock(trackMutext);
    
    char *value = ioLiveDataDriver->retriveRawData(deviceID);
    if (value) {
        currentLiveValue.reset(new CDataWrapper(value));
        free(value);
    }
    if(trackingAttribute.size() == 0) return;
    CDataWrapper *tmpPtr = currentLiveValue.get();
        //update buffer for tracked attribute
    for (std::vector<string>::iterator iter = trackingAttribute.begin(); 
         iter != trackingAttribute.end(); 
         iter++) {
        const char *key = (*iter).c_str();
        
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
        }
    }
}
