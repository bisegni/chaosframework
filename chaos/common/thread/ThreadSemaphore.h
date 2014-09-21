//
//  ThreadSemaphore.h
//  CHAOSFramework
//
//	found on http://stackoverflow.com/questions/3928853/how-can-i-achieve-something-similar-to-a-semaphore-using-boost-in-c

#ifndef CHAOSFramework_ThreadSemaphore_h
#define CHAOSFramework_ThreadSemaphore_h

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_types.hpp>

namespace chaos{
	namespace common {
		namespace thread {
			class ThreadSemaphore {
				//The current semaphore count.
				unsigned int count_;
				
				//mutex_ protects count_.
				//Any code that reads or writes the count_ data must hold a lock on
				//the mutex.
				boost::mutex mutex_;
				
				//Code that increments count_ must notify the condition variable.
				boost::condition_variable condition_;
				
			public:
				explicit ThreadSemaphore(unsigned int initial_count)
				: count_(initial_count),
				mutex_(),
				condition_(){}
				
				unsigned int get_count()  {
					//The "lock" object locks the mutex when it's constructed,
					//and unlocks it when it's destroyed.
					boost::unique_lock<boost::mutex> lock(mutex_);
					return count_;
				}
				
				void signal(unsigned int num_of_thread = 1) {
					boost::unique_lock<boost::mutex> lock(mutex_);
					
					count_ += num_of_thread;
					
					//Wake up any waiting threads.
					//Always do this, even if count_ wasn't 0 on entry.
					//Otherwise, we might not wake up enough waiting threads if we
					//get a number of signal() calls in a row.
					condition_.notify_one();
				}
				
				void wait() {
					boost::unique_lock<boost::mutex> lock(mutex_);
					while (count_ == 0)
					{
						condition_.wait(lock);
					}
					--count_;
				}
				
			};
		}
	}
}
#endif
