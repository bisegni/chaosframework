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
                ChaosUniquePtr<AsyncPoolRunner>  async_pool_runner;
                
                //timer
                boost::asio::io_service         asio_service;
                boost::asio::io_service::work   asio_default_work;
                boost::thread_group             asio_thread_group;
                
                boost::mutex mutex;
				
				AsyncCentralManager();
				~AsyncCentralManager();
				// Initialize instance
				void init(void *inti_data);
				
				// Deinit the implementation
				void deinit();
			public:
                //!ad a timer specifying the timeout and repeat time in milliseconds
				int addTimer(TimerHandler *timer_handler,
                             uint64_t timeout,
                             uint64_t repeat);
				//!stop and remove a timer
				int removeTimer(TimerHandler *timer_handler);
                
                //!submit a runnable
                void submitAsyncJob(ChaosUniquePtr<AsyncRunnable> runnable);
			};
		}
	}
}


#endif /* defined(__CHAOSFramework__AsyncCentralManager__) */
