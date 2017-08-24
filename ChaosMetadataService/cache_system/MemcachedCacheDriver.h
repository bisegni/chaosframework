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
#ifndef __CHAOSFramework__MemcachedCacheDriver__
#define __CHAOSFramework__MemcachedCacheDriver__

#include "CacheDriver.h"

#include <string>
#include <libmemcached/memcached.hpp>
#include <chaos/common/utility/ObjectFactoryRegister.h>

namespace chaos {
    namespace data_service {
        namespace cache_system {
            
			//! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
			DECLARE_CLASS_FACTORY(MemcachedCacheDriver, CacheDriver) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MemcachedCacheDriver)

				memcached_st *memcache_client;
				
				MemcachedCacheDriver(std::string alias);

				
				bool validateString(std::string& server_description, std::vector<std::string>& tokens);
            public:
				~MemcachedCacheDriver();
				
                int putData(void *element_key, uint8_t element_key_len, void *value, uint32_t value_len);
                
                int getData(void *element_key, uint8_t element_key_len, void **value, uint32_t& value_len);
                
                int addServer(std::string server_desc);
                
                int removeServer(std::string server_desc);
				
				int updateConfig();
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__MemcachedCacheDriver__) */
