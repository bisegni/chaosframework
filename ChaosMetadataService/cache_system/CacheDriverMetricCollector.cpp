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

using namespace chaos::common::metric;
using namespace chaos::metadata_service::cache_system;

static CHistogramBoudaries size_buondaries = {1024, 2*1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024, 1024*1024};

CacheDriverMetricCollector::CacheDriverMetricCollector(CacheDriver *_wrapped_cache_driver):
CacheDriver(_wrapped_cache_driver->getName()),
wrapped_cache_driver(_wrapped_cache_driver){
    //init the shared collector
    MetricManager::getInstance()->createCounterFamily("mds_cache_set_dataset_count", "Is the number of dataset that are stored in cache");
    metric_counter_set_pack_count_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_set_dataset_count");
    
    MetricManager::getInstance()->createCounterFamily("mds_cache_set_dataset_bandwith", "Is the bandwidth of dataset that are stored in cache (in byte)");
    metric_counter_set_bandwith_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_set_dataset_bandwith");
    
    MetricManager::getInstance()->createCounterFamily("mds_cache_get_dataset_count", "Is the number of dataset that are retrieved from the cache");
    metric_counter_get_pack_count_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_get_dataset_count");
    
    MetricManager::getInstance()->createCounterFamily("mds_cache_get_dataset_bandwith", "Is the bandwidth of dataset that are retrieved from the cache (in byte)");
    metric_counter_get_bandwith_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_get_dataset_bandwith");
    
    MetricManager::getInstance()->createHistogramFamily("mds_cache_set_object_size", "Is the counter (per size) of elements stored in cache");
    metric_histogram_set_size_uptr = MetricManager::getInstance()->getNewHistogramFromFamily("mds_cache_set_object_size", {}, size_buondaries);
    
    MetricManager::getInstance()->createHistogramFamily("mds_cache_get_object_size", "Is the counter (per size) of elements retrieved from cache");
    metric_histogram_get_size_uptr = MetricManager::getInstance()->getNewHistogramFromFamily("mds_cache_set_object_size", {}, size_buondaries);
}

CacheDriverMetricCollector::~CacheDriverMetricCollector() {
    delete wrapped_cache_driver;
}

int CacheDriverMetricCollector::putData(const std::string& key,
                                        CacheData data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    if(data &&
       data->size()) {
        metric_histogram_set_size_uptr->observe(data->size());
        (*metric_counter_set_pack_count_uptr)++;
        (*metric_counter_set_bandwith_uptr) += data->size();
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
    if(data &&
       data->size()) {
        metric_histogram_get_size_uptr->observe(data->size());
        (*metric_counter_get_pack_count_uptr)++;
        (*metric_counter_get_bandwith_uptr)+=data->size();
    }
    return err;
}
int CacheDriverMetricCollector::getData(const ChaosStringVector&    keys,
                                        MultiCacheData&             multi_data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    int err =  wrapped_cache_driver->getData(keys,
                                             multi_data);
    for (auto&& element : multi_data) {
        if(element.second) {
            metric_histogram_get_size_uptr->observe(element.second->size());
            (*metric_counter_get_pack_count_uptr)++;
            (*metric_counter_get_bandwith_uptr)+=element.second->size();
        }
    }
    return err;
}
int CacheDriverMetricCollector::addServer(const std::string& server_desc) {
    CHAOS_ASSERT(wrapped_cache_driver)
    return wrapped_cache_driver->addServer(server_desc);
}

int CacheDriverMetricCollector::removeServer(const std::string& server_desc) {
    CHAOS_ASSERT(wrapped_cache_driver)
    return wrapped_cache_driver->removeServer(server_desc);
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
