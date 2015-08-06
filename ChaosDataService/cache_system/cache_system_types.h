/*
 *	cache_system_types.h
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

#ifndef CHAOSFramework_cache_system_types_h
#define CHAOSFramework_cache_system_types_h
#include "../worker/DataWorker.h"

namespace chaos {
	namespace data_service {
		namespace cache_system {
			
			typedef std::vector<std::string>			CacheServerList;
			typedef std::vector<std::string>::iterator	CacheServerListIterator;
			
			//! index driver setting
			typedef struct CacheDriverSetting {
                //! true if we need to log metric
                bool                        log_metric;
                
                //! determinate the update interval of the metric
                uint32_t                    log_metric_update_interval;
                
				//! driver implementation
				std::string					cache_driver_impl;
				
				//! list of the cache server
				CacheServerList				startup_chache_servers;
				
                bool                        caching_worker_log_metric;
                
                uint32_t                    caching_worker_log_metric_update_interval;
                
				//!number of cache worker
				unsigned int				caching_worker_num;
				
				//!cache worker setting
				worker::DataWorkerSetting	caching_worker_setting;
				
				//! kv param for the implementations of the driver
				std::map<std::string, std::string> key_value_custom_param;
			} CacheDriverSetting;
		}
	}
}

#endif
