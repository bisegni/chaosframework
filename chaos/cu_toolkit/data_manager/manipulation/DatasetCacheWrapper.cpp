/*
 *	DatasetCacheWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#include "DatasetCacheWrapper.h"

using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::manipulation;

#define INFO    INFO_LOG(DatasetCacheWrapper)
#define DBG     DBG_LOG(DatasetCacheWrapper)
#define ERR     ERR_LOG(DatasetCacheWrapper)

DatasetCacheWrapper::DatasetCacheWrapper(DatasetElement::DatasetElementPtr& _dataset_element):
dataset_element(_dataset_element){}

DatasetCacheWrapper::~DatasetCacheWrapper(){}


int DatasetCacheWrapper::setAttributeValue(const std::string&  ds_name,
                                              const std::string&  attr_name,
                                              void * value,
                                              uint32_t size) {
    chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingByName(attr_name);
    if(value_setting) {
        value_setting->setValue(value,
                                size);
    }
    return 0;
}

int DatasetCacheWrapper::setAttributeValue(const std::string& ds_name,
                                              const unsigned int attr_index,
                                              void * value,
                                              uint32_t size) {
    chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingForIndex(attr_index);
    if(value_setting) {
        value_setting->setValue(value,
                                size);
    }
    return 0;
}

