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

#ifndef __CHAOSFramework__DriverPoolManager__
#define __CHAOSFramework__DriverPoolManager__

#include "cache_system/cache_system.h"
#include "persistence/persistence.h"
#include "object_storage/object_storage.h"

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/pool/ResourcePool.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos{
    namespace metadata_service {
        //!cache
        //! forward declaration
        class DriverPoolManager;
        
        
        typedef chaos::common::pool::ResourcePool<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> ObjectStoragePool;
        typedef ObjectStoragePool::ResourcePoolHelper ObjectStoragePoolHelper;
        typedef ObjectStoragePool::ResourceSlot ObjectStoragePoolSlot;
        
        //! cache driver pool implementation
        class ObjectStorageDriverPool:
        public ObjectStoragePoolHelper,
        public chaos::common::utility::InizializableService {
            friend class DriverPoolManager;
            //!created instances
            unsigned int instance_created;
            
            //!keep track of how many instance in pol need to be present at startup
            unsigned int minimum_instance_in_pool;
            
            //complete implementation name of cache driver
            std::string impl_name;
            
            //pool container
            ObjectStoragePool pool;
            
            ObjectStorageDriverPool();
            ~ObjectStorageDriverPool();
        protected:
            //resource pool handler
            chaos::service_common::persistence::data_access::AbstractPersistenceDriver* allocateResource(const std::string& pool_identification,
                                                                                                         uint32_t& alive_for_ms);
            void deallocateResource(const std::string& pool_identification,
                                    chaos::service_common::persistence::data_access::AbstractPersistenceDriver* pooled_driver);
            
            void init(void *init_data);
            void deinit();
        };
        
        typedef chaos::common::pool::ResourcePool<chaos::metadata_service::cache_system::CacheDriver> CachePool;
        typedef CachePool::ResourcePoolHelper CachePoolHelper;
        typedef CachePool::ResourceSlot CachePoolSlot;
        
        //! cache driver pool implementation
        class CacheDriverPool:
        public CachePoolHelper,
        public chaos::common::utility::InizializableService {
            friend class DriverPoolManager;
            //!created instances
            unsigned int instance_created;
            
            //!keep track of how many instance in pol need to be present at startup
            unsigned int minimum_instance_in_pool;
            
            //complete implementation name of cache driver
            std::string cache_impl_name;
            
            //pool container
            CachePool pool;
            
            CacheDriverPool();
            ~CacheDriverPool();
        protected:
            //resource pool handler
            chaos::metadata_service::cache_system::CacheDriver* allocateResource(const std::string& pool_identification,
                                                                             uint32_t& alive_for_ms);
            void deallocateResource(const std::string& pool_identification,
                                    chaos::metadata_service::cache_system::CacheDriver* pooled_driver);
            
            void init(void *init_data);
            void deinit();
        };
        
        
        //!base singleto class for driver pool system
        class DriverPoolManager:
        public chaos::common::utility::Singleton<DriverPoolManager>,
        public chaos::common::async_central::TimerHandler,
        public chaos::common::utility::InizializableService {
            friend class chaos::common::utility::Singleton<DriverPoolManager>;
            chaos::common::utility::InizializableServiceContainer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> persistence_driver;
            chaos::common::utility::InizializableServiceContainer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> storage_driver;
            //drivers pool;
            CacheDriverPool         cache_pool;
            //ObjectStorageDriverPool obj_storage_pool;
            //private contructor and destructor
            DriverPoolManager();
            ~DriverPoolManager();
        protected:
            //timer handler
            void timeout();
            void init(void *init_data);
            void deinit();
            
        public:
            CachePoolSlot *getCacheDriverInstance();
            void releaseCacheDriverInstance(CachePoolSlot *cache_driver_instance);
            
            chaos::service_common::persistence::data_access::AbstractPersistenceDriver& getPersistenceDrv();
            template<typename T>
            T* getPersistenceDataAccess() {
                if(persistence_driver.get() == NULL) throw CException(-1, "No Persistence Driver Found", __PRETTY_FUNCTION__);
                return persistence_driver->getDataAccess<T>();
            }
            
            chaos::service_common::persistence::data_access::AbstractPersistenceDriver& getObjectStorageDrv();
            template<typename T>
            T* getStorageDataAccess() {
                if(storage_driver.get() == NULL) throw CException(-1, "No Storage Driver Found", __PRETTY_FUNCTION__);
                return storage_driver->getDataAccess<T>();
            }
        };
    }
}

#endif /* defined(__CHAOSFramework__DriverPoolManager__) */
