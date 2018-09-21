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

#include "CacheDriverMetricCollector.h"
#include <chaos/common/global.h>

using namespace chaos::data_service::cache_system;

// static initialization
ChaosSharedPtr<CacheDriverSharedMetricIO> CacheDriverMetricCollector::shared_collector;

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
    delete wrapped_cache_driver;
}

int CacheDriverMetricCollector::putData(const std::string& key,
                                        CacheData data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    if(data->size()) {
        shared_collector->incrementSetBandWidth((uint32_t)data->size());
    }
    int err =  wrapped_cache_driver->putData(key,
                                             data);
    return err;
}

int CacheDriverMetricCollector::getData(const std::string& key,
                                        CacheData& data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    int err =  wrapped_cache_driver->getData(key,
                                             data);
    if(data->size()) {
        shared_collector->incrementGetBandWidth((uint32_t)data->size());
    }
    return err;
}
int CacheDriverMetricCollector::getData(const ChaosStringVector&    keys,
                                        MultiCacheData&             multi_data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    int err =  wrapped_cache_driver->getData(keys,
                                             multi_data);
    if(multi_data.size()) {
        shared_collector->incrementGetBandWidth((uint32_t)multi_data.size());
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

void CacheDriverMetricCollector::init(void *init_data)  {
    CHAOS_ASSERT(wrapped_cache_driver)
    InizializableService::initImplementation(wrapped_cache_driver,
                                              init_data,
                                              wrapped_cache_driver->getName(),
                                              __PRETTY_FUNCTION__);
}

void CacheDriverMetricCollector::deinit()  {
    CHAOS_ASSERT(wrapped_cache_driver)
    InizializableService::deinitImplementation(wrapped_cache_driver,
                                                wrapped_cache_driver->getName(),
                                                __PRETTY_FUNCTION__);
}
