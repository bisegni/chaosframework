//
//  global.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_global_h
#define CHAOSFramework_global_h

#include <string>
#include <vector>

#include "vfs/vfs.h"
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
#define CACHE_WORKER_NUMBER			2
		
		//answer configuration
#define OPT_ANSWER_WORKER_NUM		"answer_worker_num"
#define OPT_ANSWER_WORKER_THREAD	"answer_worker_thread"
#define ANSWER_WORKER_NUMBER		2
		
		//stage data indexer
#define OPT_INDEXER_WORKER_NUM		"indexer_worker_num"
#define INDEXER_WORKER_NUMBER		2
		
		//vfs driver configuration
#define OPT_VFS_STORAGE_DRIVER_IMPL		"vfs_storage_drvr_impl"
#define OPT_VFS_STORAGE_DOMAIN			"vfs_storage_domain"
#define OPT_VFS_STORAGE_DRIVER_KVP		"vfs_storage_driver_kvp"
		
#define OPT_VFS_INDEX_DRIVER_IMPL		"vfs_index_drvr_impl"
#define OPT_VFS_INDEX_DRIVER_SERVERS	"vfs_index_servers"
#define OPT_VFS_INDEX_DRIVER_KVP		"vfs_index_driver_kvp"
				
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
			
			//----------answer worker------------------
			unsigned int				answer_worker_num;
			worker::DataWorkerSetting	answer_worker_setting;

			//----------stage data indexer---------------
			unsigned int				indexer_worker_num;
			
			//----------vfs configuration----------------
			vfs::VFSManagerSetting		file_manager_setting;
		} ChaosDataServiceSetting;
	}
}

#endif
