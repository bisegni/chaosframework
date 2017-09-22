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
                //WaitSemaphore wait_sem;
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
