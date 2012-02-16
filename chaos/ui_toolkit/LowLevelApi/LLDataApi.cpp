//
//  LLDataApi.cpp
//  UIToolkit
//
//  Created by Claudio Bisegni on 01/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "LLDataApi.h"

#include "../../common/global.h"
#include "../../common/io/IOMemcachedDriver.h"

using namespace chaos;
using namespace chaos::ui;

DeviceDatasetCache *LLDataApi::datasetCache = NULL;

LLDataApi::LLDataApi() {
    liveDriver = new IOMemcachedDriver();
    liveDriver->init();
}

LLDataApi::~LLDataApi() {
    if(liveDriver){
        liveDriver->deinit();
        DELETE_OBJ_POINTER(liveDriver)
    }
}

char *LLDataApi::getLastDatasetForDeviceKey(string& deviceKey) {
    CHAOS_ASSERT(liveDriver)
    size_t dataDimension = 0;
        //get the raw data
    char *result = liveDriver->retriveRawData(deviceKey, &dataDimension);
    
    return result;
}

int32_t LLDataApi::getLastInt32ForDeviceKey(string& dsKey, string& dsAttrbiuteKey) {
    return 0;
}

int64_t LLDataApi::getLastInt64ForDeviceKey(string& dsKey, string& dsAttrbiuteKey) {
    return 0;
}

double_t LLDataApi::getLastDoubleForDeviceKey(string& dsKey, string& dsAttrbiuteKey) {
    return (double_t)0.0;
}
