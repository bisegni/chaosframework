/*
 *	DriverPoolManager.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DriverPoolManager__
#define __CHAOSFramework__DriverPoolManager__

#include "cache_system/cache_system.h"
#include "persistence/persistence.h"

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/pool/ResourcePool.h>
#include <chaos/common/async_central/async_central.h>

namespace chaos{
    namespace data_service {
        //!cache
        //! forward declaration
        class DriverPoolManager;
        
        typedef chaos::common::pool::ResourcePool<chaos::data_service::cache_system::CacheDriver> CachePool;
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
            chaos::data_service::cache_system::CacheDriver* allocateResource(const std::string& pool_identification,
                                                                             uint32_t& alive_for_ms);
            void deallocateResource(const std::string& pool_identification,
                                    chaos::data_service::cache_system::CacheDriver* pooled_driver);

            void init(void *init_data) throw (chaos::CException);
            void deinit() throw (chaos::CException);
        };
        
        
        //!base singleto class for driver pool system
        class DriverPoolManager:
        public chaos::common::utility::Singleton<DriverPoolManager>,
        public chaos::common::async_central::TimerHandler,
        public chaos::common::utility::InizializableService {
            friend class chaos::common::utility::Singleton<DriverPoolManager>;
            
            //drivers pool;
            CacheDriverPool cache_pool;
            
            //private contructor and destructor
            DriverPoolManager();
            ~DriverPoolManager();
        protected:
            //timer handler
            void timeout();
            void init(void *init_data) throw (chaos::CException);
            void deinit() throw (chaos::CException);
            
        public:
            CachePoolSlot *getCacheDriverInstance();
            void releaseCacheDriverInstance(CachePoolSlot *cache_driver_instance);
        };
    }
}

#endif /* defined(__CHAOSFramework__DriverPoolManager__) */
