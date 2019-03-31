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

#ifndef CHAOSFramework_cache_system_types_h
#define CHAOSFramework_cache_system_types_h
#include "../worker/DataWorker.h"

namespace chaos {
	namespace metadata_service {
		namespace cache_system {
			
			typedef std::vector<std::string>			CacheServerList;
			typedef std::vector<std::string>::iterator	CacheServerListIterator;
			
			//! index driver setting
			typedef struct CacheDriverSetting {
				//! driver implementation
				std::string					cache_driver_impl;
                //! list of the cache server
                CacheServerList             startup_chache_servers;
				//! kv param for the implementations of the driver
				std::map<std::string, std::string> key_value_custom_param;
                
                //! true if we need to log metric
                bool                log_metric;
                
                //! determinate the update interval of the metric
                uint32_t            log_metric_update_interval;
                
                uint32_t            caching_pool_min_instances_number;
			} CacheDriverSetting;
		}
	}
}

#endif
