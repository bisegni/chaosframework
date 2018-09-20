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
#ifndef __CHAOSFramework__CouchbaseCacheDriver__
#define __CHAOSFramework__CouchbaseCacheDriver__

#include "CacheDriver.h"

#include <vector>
#include <string>

#include <libcouchbase/couchbase.h>

#include <chaos/common/pool/ResourcePool.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

#define COUCHBASE_DEFAULT_TIMEOUT 5000000
namespace chaos {
    namespace metadata_service {
        namespace cache_system {
            typedef chaos::common::pool::ResourcePool< lcb_t > CouchbasePool;
            typedef CouchbasePool::ResourcePoolHelper CouchbasePoolHelper;
            typedef CouchbasePool::ResourceSlot CouchbasePoolSlot;
            
            //! cache driver pool implementation
            class CouchbaseDriverPool:
            public CouchbasePoolHelper,
            public chaos::common::utility::InizializableService {
                friend class DriverPoolManager;
                //!created instances
                unsigned int instance_created;
                
                //!keep track of how many instance in pol need to be present at startup
                unsigned int minimum_instance_in_pool;
                
                //pool container
                ChaosStringVector           all_server_to_use;
            protected:

                bool validateString(std::string& server_description);
                
                //resource pool handler
                lcb_t *allocateResource(const std::string& pool_identification,
                                             uint32_t& alive_for_ms);
                void deallocateResource(const std::string& pool_identification,
                                        lcb_t *pooled_driver);
            public:
                CouchbasePool pool;
                CouchbaseDriverPool();
                ~CouchbaseDriverPool();
                void init(void *init_data);
                void deinit();
                int addServer(std::string server_desc);
                int removeServer(std::string server_desc);
            };
            
            typedef enum ResultType {
                ResultTypeGet,
                ResultTypeMultiGet,
                ResultTypeStore
            } ResultType;
            
            struct Result {
                const ResultType     return_type;
                mutable lcb_error_t		err;
                mutable std::string		err_str;
                Result(ResultType _return_type);
                virtual ~Result();
            };
            
            struct GetResult:
            public Result {
                CacheData& cached_data;
                GetResult(CacheData& _cached_data);
            };
            
            struct MultiGetResult:
            public Result {
                MultiCacheData& multi_cached_data;
                MultiGetResult(MultiCacheData& _multi_cached_data);
            };
            
            struct StoreResult:
            public Result {
                StoreResult();
            };
            
            //! Couchbase implementation for cache driver
            /*!
             This driver uses couchbase for implementa cache driver.
             */
            DECLARE_CLASS_FACTORY(CouchbaseCacheDriver, CacheDriver) {
                REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(CouchbaseCacheDriver)
                friend class CouchbaseDriverPool;

                CouchbaseDriverPool driver_pool;
                CouchbaseCacheDriver(std::string alias);
                
                static void errorCallback(lcb_t instance,
                                          lcb_error_t err,
                                          const char *errinfo);
                static void getCallback(lcb_t instance,
                                        const void *cookie,
                                        lcb_error_t error,
                                        const lcb_get_resp_t *resp);
                static void setCallback(lcb_t instance,
                                        const void *cookie,
                                        lcb_storage_t operation,
                                        lcb_error_t error,
                                        const lcb_store_resp_t *resp);
            public:
                ~CouchbaseCacheDriver();
                
                int putData(const std::string& key,
                            CacheData data);
                
                int getData(const std::string& key,
                            CacheData& data);
                
                int getData(const ChaosStringVector& keys,
                            MultiCacheData& multi_data);
                
                int addServer(const std::string& server_desc);
                
                int removeServer(const std::string& server_desc);
                
                //! init
                void init(void *init_data);
                
                //!deinit
                void deinit();
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__CouchbaseCacheDriver__) */
