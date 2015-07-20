/*
 *	CacheDriverMetricCollector.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "CacheDriverMetricCollector.h"
#include <chaos/common/global.h>

using namespace chaos::data_service::cache_system;

// static initialization
boost::shared_ptr<CacheDriverSharedMetricIO> CacheDriverMetricCollector::shared_collector;

CacheDriverMetricCollector::CacheDriverMetricCollector(CacheDriver *_wrapped_cache_driver):
CacheDriver(_wrapped_cache_driver->getName()),
wrapped_cache_driver(_wrapped_cache_driver){
    //init the shared collector
    //lock mutex
    boost::unique_lock<boost::mutex> wr(mutex_shared_collector);
    
    //check if the static instance needs to be initlized
    if(shared_collector.get() == NULL){
        //initliae it
        shared_collector.reset(new CacheDriverSharedMetricIO("CacheDriverMetricCollector"));
        CHAOS_ASSERT(shared_collector.get())
    }
}

CacheDriverMetricCollector::~CacheDriverMetricCollector() {
    CHK_AND_DELETE_OBJ_POINTER(wrapped_cache_driver)
}

int CacheDriverMetricCollector::putData(void *element_key,
                                        uint8_t element_key_len,
                                        void *value,
                                        uint32_t value_len) {
    CHAOS_ASSERT(wrapped_cache_driver)
    if(value_len) {
        shared_collector->incrementSetBandWidth(value_len);
    }
    int err =  wrapped_cache_driver->putData(element_key,
                                             element_key_len,
                                             value,
                                             value_len);
    return err;
}

int CacheDriverMetricCollector::getData(void *element_key,
                                        uint8_t element_key_len,
                                        void **value,
                                        uint32_t& value_len) {
    CHAOS_ASSERT(wrapped_cache_driver)
    int err =  wrapped_cache_driver->getData(element_key,
                                             element_key_len,
                                             value,
                                             value_len);
    if(value_len) {
        shared_collector->incrementGetBandWidth(value_len);
    }
    return err;
}

int CacheDriverMetricCollector::addServer(std::string server_desc) {
    CHAOS_ASSERT(wrapped_cache_driver)
    return wrapped_cache_driver->addServer(server_desc);
}

int CacheDriverMetricCollector::removeServer(std::string server_desc) {
    CHAOS_ASSERT(wrapped_cache_driver)
    return wrapped_cache_driver->removeServer(server_desc);
}

int CacheDriverMetricCollector::updateConfig() {
    CHAOS_ASSERT(wrapped_cache_driver)
    return wrapped_cache_driver->updateConfig();
}

void CacheDriverMetricCollector::init(void *init_data) throw (chaos::CException) {
    CHAOS_ASSERT(wrapped_cache_driver)
    InizializableService::initImplementation(wrapped_cache_driver,
                                              init_data,
                                              wrapped_cache_driver->getName(),
                                              __PRETTY_FUNCTION__);
}

void CacheDriverMetricCollector::deinit() throw (chaos::CException) {
    CHAOS_ASSERT(wrapped_cache_driver)
    InizializableService::deinitImplementation(wrapped_cache_driver,
                                                wrapped_cache_driver->getName(),
                                                __PRETTY_FUNCTION__);
}