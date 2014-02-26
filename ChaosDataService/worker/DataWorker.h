//
//  DataWorker.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DataWorker__
#define __CHAOSFramework__DataWorker__

#include "../dataservice_global.h"

#include <chaos/common/utility/StartableService.h>

#include <boost//thread.hpp>
#include <boost/lockfree/queue.hpp>

#define DEFAULT_JOB_THREAD 2

namespace chaos{
    namespace data_service {
		namespace worker {
			
			class DataWorker: public chaos::utility::StartableService  {
				//job queue list
				boost::lockfree::queue<WorkerJobInfoPtr> job_queue;
				
				//group thread
				boost::thread_group job_thread_group;
				
				//muthex for condition variable
				boost::mutex job_mutex;
				
				//condition for the threads
				boost::condition_variable job_condition;
				
				bool work;
			protected:
				DataWorkerSetting settings;
				
				void consumeJob();
				WorkerJobInfoPtr getNextOrWait();
				
				virtual void executeJob(WorkerJobInfoPtr job_info) = 0;
			public:
				DataWorker();
				virtual ~DataWorker();
				void init(void *init_data) throw (chaos::CException);
				void start() throw (chaos::CException);
				void stop() throw (chaos::CException);
				void deinit() throw (chaos::CException);
				bool submitJobInfo(WorkerJobInfoPtr job_info);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DataWorker__) */
