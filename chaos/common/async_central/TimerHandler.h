/*
 *	TimerHandler.h
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

#ifndef __CHAOSFramework__TimerHandler__
#define __CHAOSFramework__TimerHandler__
#include <boost/asio.hpp>
#include <chaos/common/thread/WaitSemaphore.h>
namespace chaos {
	namespace common {
		namespace async_central {
		
			//! forward declaration
			class AsyncCentralManager;
			
			/*!
			 UV timer c++ abstraction
			 */
            class TimerHandler {
				friend class AsyncCentralManager;
                //boost timer
                WaitSemaphore wait_sem;
                boost::asio::deadline_timer *timer;
                uint64_t    delay;
                void timerTimeout(const boost::system::error_code& error);
                void wait(uint64_t _delay);
                void removeTimer();
			public:
				TimerHandler();
				~TimerHandler();
				virtual void timeout() = 0;
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__TimerHandler__) */
