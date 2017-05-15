/*
 *	AsyncCentralManager.h
 *	!CHAOS
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


#ifndef __CHAOSFramework__AsyncCentralManager__
#define __CHAOSFramework__AsyncCentralManager__

#include <chaos/common/async_central/AsyncPoolRunner.h>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/Delegate.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>


namespace chaos {
	namespace common {
		namespace async_central {
			
			//! forward declaration
			class TimerHandler;
			
			/*!
			 Managment class fot the async central
			 */
			class AsyncCentralManager :
			public utility::Singleton<AsyncCentralManager>,
			public utility::InizializableService {
			  friend class utility::Singleton<AsyncCentralManager>;
                //job
                std::auto_ptr<AsyncPoolRunner>  async_pool_runner;
                
                //timer
				boost::asio::io_service         asio_service;
                boost::asio::io_service::work   asio_default_work;
                boost::thread_group             asio_thread_group;
                
                boost::mutex mutex;
				
				AsyncCentralManager();
				~AsyncCentralManager();
				// Initialize instance
				void init(void *inti_data) throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
			public:
                //!ad a timer specifying the timeout and repeat time in milliseconds
				int addTimer(TimerHandler *timer_handler,
                             uint64_t timeout,
                             uint64_t repeat);
				//!stop and remove a timer
				int removeTimer(TimerHandler *timer_handler);
                
                //!submit a runnable
                void submitAsyncJob(AsyncRunnable *runnable);
			};
		}
	}
}


#endif /* defined(__CHAOSFramework__AsyncCentralManager__) */
