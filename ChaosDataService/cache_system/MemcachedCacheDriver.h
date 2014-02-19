//
//  MemcachedCacheDriver.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__MemcachedCacheDriver__
#define __CHAOSFramework__MemcachedCacheDriver__

#include "CacheDriver.h"

#include <string>
#include <libmemcached/memcached.hpp>
namespace chaos {
    namespace data_service {
        namespace cache_system {
            
			//! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
            class MemcachedCacheDriver : public CacheDriver {
				memcached_st *memcache_client;
				
				bool validateString(std::string& server_description, std::vector<std::string> tokens);
            public:
				MemcachedCacheDriver();
				~MemcachedCacheDriver();
				
                int putData(uint32_t element_hash, void *value, uint32_t value_len);
                
                int getData(uint32_t element_hash, void **value, uint32_t& value_len);
                
                int addServer(std::string server_desc);
                
                int removeServer(std::string server_desc);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__MemcachedCacheDriver__) */
