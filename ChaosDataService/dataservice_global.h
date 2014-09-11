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

#include "vfs/vfs.h"
#include "db_system/db_system.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

namespace chaos{
    namespace data_service {

#define OPT_RUN_MODE				"run_mode"
		
		//cache configuration
#define OPT_CACHE_SERVER_LIST		"cache_servers"
#define OPT_CACHE_DRIVER			"cache_driver"
#define OPT_CACHE_WORKER_NUM		"cache_worker_num"
#define OPT_CACHE_WORKER_THREAD		"cache_worker_thread"
#define CACHE_WORKER_NUMBER			1
	
		//query data consumer
#define OPT_QUERY_CONSUMER_VFILE_MANTAINANCE_DELAY		"qdc_vfile_mantain_delay"
#define QUERY_CONSUMER_VFILE_MANTAINANCE_DEFAULT_DELAY	60
#define OPT_QUERY_CONSUMER_QM_THREAD_POOL_SIZE			"query_thread_pool_size"
#define QUERY_CONSUMER_QM_THREAD_POOL_SIZE				1
		
		//answer configuration
//#define OPT_ANSWER_WORKER_NUM		"answer_worker_num"
//#define OPT_ANSWER_WORKER_THREAD	"answer_worker_thread"
//#define ANSWER_WORKER_NUMBER		1
		
		//stage data indexer
#define OPT_INDEXER_WORKER_NUM			"indexer_worker_num"
#define OPT_INDEXER_SCAN_DELAY			"indexer_scan_delay"
#define INDEXER_DEFAULT_WORKER_NUMBER	1
#define INDEXER_DEFAULT_SCAN_DELAY		60
		
		//vfs driver configuration
#define OPT_VFS_STORAGE_DRIVER_IMPL		"vfs_storage_drvr_impl"
#define OPT_VFS_STORAGE_DOMAIN			"vfs_storage_domain"
#define OPT_VFS_STORAGE_DRIVER_KVP		"vfs_storage_driver_kvp"
		
#define OPT_DB_DRIVER_IMPL		"db_drvr_impl"
#define OPT_DB_DRIVER_SERVERS	"db_servers"
#define OPT_DB_DRIVER_KVP		"db_driver_kvp"
				
		namespace worker {
			
			typedef struct WorkerJob {
				WorkerJob(){};
				virtual ~WorkerJob(){};
			} WorkerJob, *WorkerJobPtr;
			
			typedef struct DataWorkerSetting {
				unsigned int job_thread_number;
			} DataWorkerSetting;
		}
     
		typedef std::vector<std::string>			CacheServerList;
		typedef std::vector<std::string>::iterator	CacheServerListIterator;
		
		//! Setting for dataservice configuration
		typedef struct ChaosDataServiceSetting {

			//!cache configuration
			std::string					cache_driver_impl;
			CacheServerList				startup_chache_servers;
			unsigned int				caching_worker_num;
			worker::DataWorkerSetting	caching_worker_setting;
			
			//----------query consumer---------------
			unsigned int				vfile_mantainer_delay;
			unsigned int				query_manager_thread_poll_size;
			
			//----------answer worker------------------
			//unsigned int				answer_worker_num;
			//worker::DataWorkerSetting	answer_worker_setting;

			//----------stage data indexer---------------
			unsigned int				indexer_worker_num;
			unsigned int				indexer_scan_delay;
			
			//----------vfs configuration----------------
			vfs::VFSManagerSetting		file_manager_setting;
			
			//-------------index system------------------
			//! current index driver implementaiton to use
			std::string index_driver_impl;
			
			//! the instance of the index driver for this manager
			::chaos::data_service::db_system::DBDriverSetting db_driver_setting;
		} ChaosDataServiceSetting;
	}
}

#endif
