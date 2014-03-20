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

namespace chaos{
    namespace data_service {

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

		} ChaosDataServiceSetting;
	}
}

#endif
