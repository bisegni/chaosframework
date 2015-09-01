/*
 *	CouchbaseCacheDriver.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__CouchbaseCacheDriver__
#define __CHAOSFramework__CouchbaseCacheDriver__

#include "CacheDriver.h"

#include <vector>
#include <string>

#include <libcouchbase/couchbase.h>

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

namespace chaos {
    namespace data_service {
        namespace cache_system {
            
			
			struct ResultValue {
				void		*value;
				uint32_t	value_len;
				lcb_error_t err;
			};
			
			//! Abstraction of the chache driver
            /*!
             This class represent the abstraction of the
             work to do on cache. Cache system is to be intended as global
             to all CacheDriver instance.
             */
			DECLARE_CLASS_FACTORY(CouchbaseCacheDriver, CacheDriver) {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(CouchbaseCacheDriver)
				lcb_t					instance;
				struct lcb_create_st	create_options;
				lcb_error_t				last_err;
				std::string				last_err_str;
				
				std::string				bucket_name;
				std::string				bucket_user;
				std::string				bucket_pwd;
				
				std::string all_server_str;
				boost::shared_mutex	mutex_server;
				std::vector<std::string> all_server_to_use;
				typedef std::vector<std::string>::iterator ServerIterator;

                ResultValue get_result;
				
				CouchbaseCacheDriver(std::string alias);

				
				bool validateString(std::string& server_description);
				
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
				
                int putData(void *element_key, uint8_t element_key_len, void *value, uint32_t value_len);
                
                int getData(void *element_key, uint8_t element_key_len, void **value, uint32_t& value_len);
                
                int addServer(std::string server_desc);
                
                int removeServer(std::string server_desc);
				
				int updateConfig();
				
				//! init
				void init(void *init_data) throw (chaos::CException);
				
				//!deinit
				void deinit() throw (chaos::CException);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__CouchbaseCacheDriver__) */
