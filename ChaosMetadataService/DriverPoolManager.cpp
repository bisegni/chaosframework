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

#include "DriverPoolManager.h"
#include "ChaosMetadataService.h"

#if CHAOS_PROMETHEUS
#include "cache_system/CacheDriverMetricCollector.h"
#endif

#include <limits>
#include <chaos/common/global.h>

#define DP_LOG_INFO INFO_LOG(DriverPoolManager)
#define DP_LOG_DBG  DBG_LOG(DriverPoolManager)
#define DP_LOG_ERR ERR_LOG(DriverPoolManager)

using namespace chaos::metadata_service;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::metadata_service::cache_system;
using namespace chaos::service_common::persistence::data_access;
using namespace chaos::metadata_service::object_storage::abstraction;

//-------------------------------------------global pool---------------------------------------
DriverPoolManager::DriverPoolManager():
enable_storage_driver(false),
enable_persistence_driver(false),
enable_cache_driver(false){
    if(ChaosMetadataService::getInstance()->setting.testing_mode == false) {
        enable_storage_driver = enable_persistence_driver = enable_cache_driver = true;
    } else {
        enable_storage_driver = ChaosMetadataService::getInstance()->setting.object_storage_setting.test_find;
    }
}

DriverPoolManager::~DriverPoolManager() {}

void DriverPoolManager::init(void *init_data)  {
    if(enable_cache_driver) {
        //init cache pool
        const std::string cache_impl_name = ChaosMetadataService::getInstance()->setting.cache_driver_setting.cache_driver_impl+"CacheDriver";
#if CHAOS_PROMETHEUS
        cache_driver.reset(new CacheDriverMetricCollector(ObjectFactoryRegister<chaos::metadata_service::cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name)), cache_impl_name);
#else
        cache_driver.reset(ObjectFactoryRegister<chaos::metadata_service::cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name),
                           cache_impl_name);
#endif
        
        if(cache_driver.get() == NULL) throw chaos::CException(-1, CHAOS_FORMAT("No %1% Cache Driver found", %cache_impl_name), __PRETTY_FUNCTION__);
        cache_driver.init(NULL, __PRETTY_FUNCTION__);
    }
    
    //init persistence pool
    if(enable_persistence_driver) {
        const std::string persistence_impl_name = ChaosMetadataService::getInstance()->setting.persistence_implementation+"PersistenceDriver";
        persistence_driver.reset(ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(persistence_impl_name),
                                 persistence_impl_name);
        if(persistence_driver.get() == NULL) throw chaos::CException(-1, CHAOS_FORMAT("No %1% Persistence Driver found", %persistence_impl_name), __PRETTY_FUNCTION__);
        persistence_driver.init(NULL, __PRETTY_FUNCTION__);
    }
    
    if(enable_storage_driver) {
        const std::string storage_impl_name = ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl + "ObjectStorageDriver";
        storage_driver.reset(ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(storage_impl_name),
                             storage_impl_name);
        if(storage_driver.get() == NULL) throw chaos::CException(-1, CHAOS_FORMAT("No %1% Object Storage Driver found", %storage_impl_name), __PRETTY_FUNCTION__);
        storage_driver.init(NULL, __PRETTY_FUNCTION__);
    }
}

void DriverPoolManager::deinit()  {
    if(enable_storage_driver) {storage_driver.deinit(__PRETTY_FUNCTION__);}
    if(enable_persistence_driver) {persistence_driver.deinit(__PRETTY_FUNCTION__);}
    if(enable_cache_driver) {cache_driver.deinit(__PRETTY_FUNCTION__);}
}

//--------------cach driver pool method--------------
chaos::metadata_service::cache_system::CacheDriver& DriverPoolManager::getCacheDrv() {
    return *cache_driver;
}

AbstractPersistenceDriver& DriverPoolManager::getPersistenceDrv() {
    return *persistence_driver;
}

AbstractPersistenceDriver& DriverPoolManager::getObjectStorageDrv() {
    return *storage_driver;
}
