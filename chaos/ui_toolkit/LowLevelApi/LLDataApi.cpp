/*	
 *	LLDataApi.cpp
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
