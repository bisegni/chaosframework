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

#include <limits>
#include <chaos/common/global.h>

#define DP_LOG_INFO INFO_LOG(DriverPoolManager)
#define DP_LOG_DBG  DBG_LOG(DriverPoolManager)
#define DP_LOG_ERR ERR_LOG(DriverPoolManager)

using namespace chaos::data_service;
using namespace chaos::metadata_service;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::data_service::cache_system;
using namespace chaos::service_common::persistence::data_access;
using namespace chaos::data_service::object_storage::abstraction;
//storage pool
//-------------------------------------------cache pool---------------------------------------

ObjectStorageDriverPool::ObjectStorageDriverPool():
instance_created(0),
minimum_instance_in_pool(3),
impl_name(ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl + "ObjectStorageDriver"),
pool("object_storage_driver",
     this,
     minimum_instance_in_pool) {}

ObjectStorageDriverPool::~ObjectStorageDriverPool() {}

AbstractPersistenceDriver* ObjectStorageDriverPool::allocateResource(const std::string& pool_identification,
                                                       uint32_t& alive_for_ms) {
    AbstractPersistenceDriver *pooled_driver = NULL;
    DEBUG_CODE(DP_LOG_INFO << "New pool request allocation for object storage driver:" << impl_name;)
    //increment and check instance created
    if(instance_created+1 > minimum_instance_in_pool) {
        alive_for_ms = 1000*60*10; //one hour
    } else {
        //we want at least two active driver instance
        alive_for_ms = std::numeric_limits<uint32_t>::max();
    }
    DEBUG_CODE(DP_LOG_INFO << "requested resource need to live for ms:" << alive_for_ms;)
    
    //pooled_driver
    try{
        pooled_driver = ObjectFactoryRegister<AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(impl_name);
        InizializableService::initImplementation(pooled_driver,
                                                 NULL,
                                                 pooled_driver->getName(),
                                                 __PRETTY_FUNCTION__);
        //increment create counter
        instance_created++;

        DEBUG_CODE(DP_LOG_INFO << "Allocation done for storage driver:" << impl_name << " total created:" << instance_created;)
    } catch(chaos::CException& ex) {
        DP_LOG_ERR << CHAOS_FORMAT("Erro allocation storage driver %1%", %impl_name);
    }
    return pooled_driver;
}

void ObjectStorageDriverPool::deallocateResource(const std::string& pool_identification,
                                                 AbstractPersistenceDriver* pooled_driver) {
    //decrement instance created
    instance_created--;
    InizializableService::deinitImplementation(pooled_driver,
                                               pooled_driver->getName(),
                                               __PRETTY_FUNCTION__);
    delete(pooled_driver);
}

void ObjectStorageDriverPool::init(void *init_data) throw (chaos::CException) {}

void ObjectStorageDriverPool::deinit() throw (chaos::CException) {}

//-------------------------------------------cache pool---------------------------------------

CacheDriverPool::CacheDriverPool():
instance_created(0),
cache_impl_name(ChaosMetadataService::getInstance()->setting.cache_driver_setting.cache_driver_impl + "CacheDriver"),
minimum_instance_in_pool(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_pool_min_instances_number),
pool("cache_driver",
     this,
     minimum_instance_in_pool) {}

CacheDriverPool::~CacheDriverPool() {}

CacheDriver* CacheDriverPool::allocateResource(const std::string& pool_identification,
                                               uint32_t& alive_for_ms) {
    CacheDriver *pooled_driver = NULL;
    DEBUG_CODE(DP_LOG_INFO << "New pool request allocation for cache driver:" << cache_impl_name;)
    //increment and check instance created
    if(instance_created+1 > minimum_instance_in_pool) {
        alive_for_ms = 1000*60*10; //one hour
    } else {
        //we want at least two active driver instance
        alive_for_ms = std::numeric_limits<uint32_t>::max();
    }
    DEBUG_CODE(DP_LOG_INFO << "requested resource need to live for ms:" << alive_for_ms;)
    
    //pooled_driver
    try{
        pooled_driver = ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name);
        if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.log_metric) {
            DP_LOG_INFO << "Enable cache log metric";
            pooled_driver = new cache_system::CacheDriverMetricCollector(pooled_driver);
        }
        InizializableService::initImplementation(pooled_driver,
                                                 &ChaosMetadataService::getInstance()->setting.cache_driver_setting,
                                                 pooled_driver->getName(),
                                                 __PRETTY_FUNCTION__);
        
        //configure the driver
        //add server to cache driver
        for(cache_system::CacheServerListIterator iter = ChaosMetadataService::getInstance()->setting.cache_driver_setting.startup_chache_servers.begin();
            iter != ChaosMetadataService::getInstance()->setting.cache_driver_setting.startup_chache_servers.end();
            iter++) {
            pooled_driver->addServer(*iter);
        }
        
        //fix the update on server
        if(pooled_driver->updateConfig()<0){
            DP_LOG_ERR<<" cannot complete initialization of cache driver:\""<<cache_impl_name<<"\" removing..";
            delete pooled_driver;
            return NULL;
        }
        
        //increment create counter
        instance_created++;
        
        //porint infor debug
        DEBUG_CODE(DP_LOG_INFO << "Allocation done for cache driver:" << cache_impl_name << " total created:" << instance_created;)
    } catch(chaos::CException& ex) {
        
    }
    return pooled_driver;
}

void CacheDriverPool::deallocateResource(const std::string& pool_identification,
                                         CacheDriver* pooled_driver) {
    //decrement instance created
    instance_created--;
    InizializableService::deinitImplementation(pooled_driver,
                                               pooled_driver->getName(),
                                               __PRETTY_FUNCTION__);
    delete(pooled_driver);
}

void CacheDriverPool::init(void *init_data) throw (chaos::CException) {
    //check for the presence of the cache server
    if(!ChaosMetadataService::getInstance()->setting.cache_driver_setting.cache_driver_impl.size()) {LOG_AND_TROW(DP_LOG_ERR, -1, "No cache implemetation provided");}
    if(!ChaosMetadataService::getInstance()->setting.cache_driver_setting.startup_chache_servers.size()) {LOG_AND_TROW(DP_LOG_ERR, -2, "No cache servers provided")}
}

void CacheDriverPool::deinit() throw (chaos::CException) {
    
}

//-------------------------------------------global pool---------------------------------------
DriverPoolManager::DriverPoolManager() {
    
}

DriverPoolManager::~DriverPoolManager() {
    
}

void DriverPoolManager::init(void *init_data) throw (chaos::CException) {
    InizializableService::initImplementation(cache_pool, NULL, "CacheDriverPool", __PRETTY_FUNCTION__);
}

void DriverPoolManager::deinit() throw (chaos::CException) {
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(cache_pool, "CacheDriverPool", __PRETTY_FUNCTION__);)
}

void DriverPoolManager::timeout() {
    cache_pool.pool.maintenance();
}

//--------------cach driver pool method--------------
CachePoolSlot* DriverPoolManager::getCacheDriverInstance() {
    return cache_pool.pool.getNewResource();
}

void DriverPoolManager::releaseCacheDriverInstance(CachePoolSlot *cache_driver_instance) {
    cache_pool.pool.releaseResource(cache_driver_instance);
}

ObjectStoragePoolSlot *DriverPoolManager::getObjectStorageInstance() {
    return obj_storage_pool.pool.getNewResource();
}

void DriverPoolManager::releaseObjectStorageInstance(ObjectStoragePoolSlot *obj_storage_driver_instance) {
    obj_storage_pool.pool.releaseResource(obj_storage_driver_instance);
}
