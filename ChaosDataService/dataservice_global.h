//
//  global.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_global_h
#define CHAOSFramework_global_h

#include <stdint.h>
#include <string>
#include <vector>

#include "db_system/db_system.h"
#include "cache_system/cache_system.h"
#include "object_storage/object_storage.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

namespace chaos{
    namespace data_service {

#define OPT_CACHE_ONLY				"cache-only"
#define OPT_RUN_MODE				"run-mode"
		
		//cache configuration
#define OPT_CACHE_LOG_METRIC                    "cache-log-metric"
#define OPT_CACHE_LOG_METRIC_UPDATE_INTERVAL    "cache-log-metric-update-interval"
#define OPT_CACHE_SERVER_LIST                   "cache-servers"
#define OPT_CACHE_DRIVER                        "cache-driver"
#define OPT_CACHE_DRIVER_KVP                    "cache-driver-kvp"
#define OPT_CACHE_DRIVER_POOL_MIN_INSTANCE      "cache-driver-pool-min-instance"
        
#define OPT_DATA_WORKER_LOG_METRIC                 "data-worker-log-metric"
#define OPT_DATA_WORKER_LOG_METRIC_UPDATE_INTERVAL "data-worker-log-metric-update-interval"
#define OPT_DATA_WORKER_NUM		"data-worker-num"
#define OPT_DATA_WORKER_THREAD  "data-worker-thread"
#define DATA_WORKER_NUMBER		1
	
		//query data consumer
#define OPT_QUERY_CONSUMER_VFILE_MANTAINANCE_DELAY		"qdc-vfile-mantain-delay"
#define QUERY_CONSUMER_VFILE_MANTAINANCE_DEFAULT_DELAY	60
#define OPT_QUERY_CONSUMER_QM_THREAD_POOL_SIZE			"query-thread-pool-size"
#define QUERY_CONSUMER_QM_THREAD_POOL_SIZE				1
		
		//answer configuration
//#define OPT_ANSWER_WORKER_NUM		"answer_worker_num"
//#define OPT_ANSWER_WORKER_THREAD	"answer_worker_thread"
//#define ANSWER_WORKER_NUMBER		1
		
		//stage data indexer
#define OPT_INDEXER_WORKER_NUM			"indexer-worker-num"
#define OPT_INDEXER_SCAN_DELAY			"indexer-scan-delay"
#define INDEXER_DEFAULT_WORKER_NUMBER	1
#define INDEXER_DEFAULT_SCAN_DELAY		60
        
#define OPT_DB_DRIVER_IMPL		"db-drvr-impl"
#define OPT_DB_DRIVER_SERVERS	"db-servers"
#define OPT_DB_DRIVER_KVP		"db-driver-kvp"
				

		//! Setting for dataservice configuration
		typedef struct ChaosDataServiceSetting {
			bool						cache_only;
            
			//!cache configuration
			chaos::data_service::cache_system::CacheDriverSetting cache_driver_setting;
			
			//----------query consumer---------------
			unsigned int				vfile_mantainer_delay;
			unsigned int				query_manager_thread_poll_size;
			

			//----------stage data indexer---------------
			unsigned int				indexer_worker_num;
			unsigned int				indexer_scan_delay;
			
			//----------object storage configuration----------------
            object_storage::ObjStorageSetting		object_storage_setting;
			
			//-------------index system------------------
			//! current db driver implementaiton to use
			std::string db_driver_impl;
			
			//! the instance of the index driver for this manager
			::chaos::data_service::db_system::DBDriverSetting db_driver_setting;
		} ChaosDataServiceSetting;
	}
}

#endif
