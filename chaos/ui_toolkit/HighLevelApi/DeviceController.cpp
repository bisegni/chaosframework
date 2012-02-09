    //
    //  DeviceLiveDataFetcher.cpp
    //  CHAOSFramework
    //
    //  Created by Bisegni Claudio on 07/02/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

#include "DeviceController.h"
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/common/io/IOMemcachedDriver.h>

using namespace chaos;
using namespace chaos::ui;
DeviceController::DeviceController(string& _deviceID):deviceID(_deviceID) {
    mdsChannel = NULL;
    deviceChannel = NULL;
    ioLiveDataDriver = NULL;
    liveDataThread = new CThread(this);
}

DeviceController::~DeviceController() {
    if(liveDataThread){
        stopTracking();
        liveDataThread->stop();
        delete(liveDataThread);
    }
    
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

void DeviceController::updateChannel() throw(CException) {
    
        //make the live driver
    ioLiveDataDriver = new IOMemcachedDriver();
    if(ioLiveDataDriver) {
        ioLiveDataDriver->init();
    }
    
    if(!mdsChannel){
        mdsChannel = LLRpcApi::getInstance()->getNewMetadataServerChannel();
        if(!mdsChannel) throw CException(0, "No MDS Channel created", "DeviceController::init");
    }
    
    deviceAddress.reset(mdsChannel->getNetworkAddressForDevice(deviceID, 2000));
    if(!deviceAddress.get()) throw CException(1, "No Address found for device", "DeviceController::init");
    
    if(!deviceChannel){
        deviceChannel = LLRpcApi::getInstance()->getNewDeviceMessageChannel(deviceAddress.get());
        if(!deviceChannel) throw CException(1, "No Address found for device", "DeviceController::init");
    }else{
        deviceChannel->setNewAddress(deviceAddress.get());  
    }
}

int DeviceController::initDevice() {
    CHAOS_ASSERT(mdsChannel && deviceChannel)
    int err = 0;
    auto_ptr<CDataWrapper> lastDeviceInfo(mdsChannel->getLastDatasetForDevice(deviceID));
    if(!lastDeviceInfo.get()) return -1;
    
        //initialize the devica with the metadataserver data
    err = deviceChannel->initDevice(lastDeviceInfo.get());
        //configure the live data with the same server where the device write
    if(err == 0 || err == -2){
        ioLiveDataDriver->updateConfiguration(lastDeviceInfo.get());
        
        initializeAttributeIndexMap(*lastDeviceInfo.get());
    }
    return err;
}

/*!
 Initialize the map for the devices
 \param initiDevicedescription the reference to CDataWrapper that contain device initialization information
 */
void DeviceController::initializeAttributeIndexMap(CDataWrapper& initiDevicedescription) {
    if(!initiDevicedescription.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID) || !initiDevicedescription.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
        return;
    }
    auto_ptr<CDataWrapper> attributeDescription;
    auto_ptr<CMultiTypeDataArrayWrapper> datasetDesription(initiDevicedescription.getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
    for (int idx=0; idx < datasetDesription->size(); idx++) {
        attributeDescription.reset(datasetDesription->getCDataWrapperElementAtIndex(idx));
        if(!attributeDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME) || 
           !attributeDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)) continue;
        
        
    }
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

int DeviceController::setScheduleDelay(int32_t millisecDelay){
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->setScheduleDelay(millisecDelay);
}

void DeviceController::startTracking() {
    CHAOS_ASSERT(liveDataThread)
    int64_t uSecdelay = 1000000;//default to 1 sec
                                //get the default schedule value if exist on metadata server
    auto_ptr<CDataWrapper> lastDeviceInfo(mdsChannel->getLastDatasetForDevice(deviceID));
    if(!lastDeviceInfo.get() && lastDeviceInfo->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
        uSecdelay = lastDeviceInfo->getInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
    }
    liveDataThread->setDelayBeetwenTask(uSecdelay);
    liveDataThread->start();
}

void DeviceController::stopTracking() {
    CHAOS_ASSERT(liveDataThread)
    liveDataThread->stop();
}

void DeviceController::executeOnThread(const string& threadID) throw(CException) {
    fetchCurrentDeviceValue();
}

void DeviceController::fetchCurrentDeviceValue() {
    char *value = ioLiveDataDriver->retriveRawData(deviceID);
    if (value) {
        currentLiveValue.reset(new CDataWrapper(value, true, true));
    }
}