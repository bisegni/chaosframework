//
//  BoostSpinLock.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__BoostSpinLock__
#define __CHAOSFramework__BoostSpinLock__
#include <boost/atomic.hpp>
namespace chaos {
	namespace common {
		namespace thread {
			
			class Spinlock {
			private:
				typedef enum {Locked, Unlocked} LockState;
				boost::atomic<LockState> state_;
				
			public:
				Spinlock() : state_(Unlocked) {}
				
				void lock()
				{
					while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) {
						/* busy-wait */
					}
				}
				void unlock()
				{
					state_.store(Unlocked, boost::memory_order_release);
				}
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__BoostSpinLock__) */
