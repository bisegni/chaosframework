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

CacheDriverMetricCollector::CacheDriverMetricCollector(CacheDriver *_wrapped_cache_driver):
CacheDriver(_wrapped_cache_driver->getName()),
wrapped_cache_driver(_wrapped_cache_driver){
    //init the shared collector
    MetricManager::getInstance()->createCounterFamily("mds_cache_set_dataset_count", "Is the number of dataset that are stored in cache");
    set_pack_count_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_set_dataset_count");
    MetricManager::getInstance()->createCounterFamily("mds_cache_set_dataset_bandwith", "Is the bandwidth of dataset that are stored in cache (in kbyte)");
    set_bandwith_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_set_dataset_bandwith");
    MetricManager::getInstance()->createCounterFamily("mds_cache_get_dataset_count", "Is the number of dataset that are retrieved from the cache");
    get_pack_count_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_get_dataset_count");
    MetricManager::getInstance()->createCounterFamily("mds_cache_get_dataset_bandwith", "Is the bandwidth of dataset that are retrieved from the cache (in kbyte)");
    get_bandwith_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_cache_get_dataset_bandwith");
}

CacheDriverMetricCollector::~CacheDriverMetricCollector() {
    delete wrapped_cache_driver;
}

int CacheDriverMetricCollector::putData(const std::string& key,
                                        CacheData data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    if(data->size()) {
        (*set_pack_count_uptr)++;
        (*set_bandwith_uptr) += data->size();
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
        (*get_pack_count_uptr)++;
        (*get_bandwith_uptr)+=data->size();
    }
    return err;
}
int CacheDriverMetricCollector::getData(const ChaosStringVector&    keys,
                                        MultiCacheData&             multi_data) {
    CHAOS_ASSERT(wrapped_cache_driver)
    int err =  wrapped_cache_driver->getData(keys,
                                             multi_data);
    if(multi_data.size()) {
        (*get_pack_count_uptr)++;
        (*get_bandwith_uptr)+=multi_data.size();
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
