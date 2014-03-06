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
		
		typedef struct ChaosDataServiceSetting {
			worker::DataWorkerSetting	worker_setting;
			std::string		cache_driver_impl;
			CacheServerList	startup_chache_servers;
		} ChaosDataServiceSetting;
	}
}

#endif
