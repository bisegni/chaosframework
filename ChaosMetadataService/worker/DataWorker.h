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

#ifndef __CHAOSFramework__DataWorker__
#define __CHAOSFramework__DataWorker__

//#include "../dataservice_global.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/StartableService.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include <queue>

#define DEFAULT_JOB_THREAD 1

namespace chaos {
    namespace data_service {
		namespace worker {
			
			typedef struct WorkerJob {
				WorkerJob(){};
				virtual ~WorkerJob(){};
            } WorkerJob;
            typedef ChaosSharedPtr<WorkerJob> WorkerJobPtr;
			
			typedef struct DataWorkerSetting {
                unsigned int        instances;
				unsigned int        thread_number;
                bool                log_metric;
                uint32_t            log_metric_update_interval;
			} DataWorkerSetting;
			
			class DataWorker:
			public common::utility::StartableService  {
				//job queue list
                std::queue<WorkerJobPtr> job_queue;
				
				//group thread
				boost::thread_group job_thread_group;
				
				//muthex for condition variable
				boost::mutex mutex_job;
				boost::mutex mutex_submit;
                
				//condition for the threads
				boost::condition_variable consume_job_condition;
				boost::condition_variable push_condition;
                
				bool work;
				
                boost::atomic<uint64_t> job_in_queue;
                
                unsigned int max_element;
			protected:
				void * * thread_cookie;
				void consumeJob(void *cookie);
				WorkerJobPtr getNextOrWait(boost::unique_lock<boost::mutex>& lock);
				
				virtual void executeJob(WorkerJobPtr job_info, void* cookie) = 0;
			public:
				DataWorker();
				virtual ~DataWorker();
				void init(void *init_data) throw (chaos::CException);
				void start() throw (chaos::CException);
				void stop() throw (chaos::CException);
				void deinit() throw (chaos::CException);
                void setMaxElement(uint64_t new_max_element);
                virtual int submitJobInfo(WorkerJobPtr job_info, int64_t milliseconds_to_wait = common::constants::MDSHistoryQueuePushTimeoutinMSec);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DataWorker__) */
