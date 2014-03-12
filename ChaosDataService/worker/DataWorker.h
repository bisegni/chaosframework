/*
 *	DataWorker.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DataWorker__
#define __CHAOSFramework__DataWorker__

#include "../dataservice_global.h"

#include <chaos/common/utility/StartableService.h>

#include <boost//thread.hpp>
#include <boost/lockfree/queue.hpp>

#define DEFAULT_JOB_THREAD 1

namespace chaos{
    namespace data_service {
		namespace worker {
			
			class DataWorker: public chaos::utility::StartableService  {
				//job queue list
				boost::lockfree::queue<WorkerJobPtr> job_queue;
				
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
				WorkerJobPtr getNextOrWait();
				
				virtual void executeJob(WorkerJobPtr job_info) = 0;
			public:
				DataWorker();
				virtual ~DataWorker();
				void init(void *init_data) throw (chaos::CException);
				void start() throw (chaos::CException);
				void stop() throw (chaos::CException);
				void deinit() throw (chaos::CException);
				bool submitJobInfo(WorkerJobPtr job_info);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DataWorker__) */
