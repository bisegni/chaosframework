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

#include "DatasetCacheWrapper.h"

using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::manipulation;

#define INFO    INFO_LOG(DatasetCacheWrapper)
#define DBG     DBG_LOG(DatasetCacheWrapper)
#define ERR     ERR_LOG(DatasetCacheWrapper)

DatasetCacheWrapper::DatasetCacheWrapper(chaos::cu::data_manager::DatasetElement::DatasetElementPtr& _dataset_element):
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

