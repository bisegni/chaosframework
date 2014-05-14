/*
 *	AsyncCentralManager.h
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


#ifndef __CHAOSFramework__AsyncCentralManager__
#define __CHAOSFramework__AsyncCentralManager__

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/Delegate.h>

//#include <boost/thread.hpp>
//#include <boost/scoped_ptr.hpp>
#include <map>
#include <uv.h>
namespace chaos {
	namespace common {
		namespace async_central {
			
			//! forward declaration
			class TimerHanlder;

			typedef uv_thread_t AcmThreadID;
			
			/*!
			 Managment class fot the async central
			 */
			class AsyncCentralManager :
			public Singleton<AsyncCentralManager>,
			public chaos::utility::InizializableService {
				friend class Singleton<AsyncCentralManager>;
				uv_thread_t	thread_loop_id;
				uv_loop_t *uv_l;
				static bool looping;
				
				static void _internalEventLoop(void *args);
				
				AsyncCentralManager();
				~AsyncCentralManager();
				// Initialize instance
				void init(void *inti_data) throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
			public:
				int addTimer(TimerHanlder *timer_handler, uint64_t timeout, uint64_t repeat);
				
				void removeTimer(TimerHanlder *timer_handler);
				
				int addThread(utility::delegate::Delegate *thread_delegate, AcmThreadID *thread_id);
				
				void joinThread(AcmThreadID *thread_id);
			};
		}
	}
}


#endif /* defined(__CHAOSFramework__AsyncCentralManager__) */
