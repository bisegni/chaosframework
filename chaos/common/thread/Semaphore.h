//
//  Semaphore.h
//  CHAOSFramework
//
//  Created by Sander Cox on http://stackoverflow.com/questions/3928853/how-can-i-achieve-something-similar-to-a-semaphore-using-boost-in-c
//  Modified by claudio Bisegni
//

#ifndef __CHAOSFramework__Semaphore__
#define __CHAOSFramework__Semaphore__

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_types.hpp>

namespace chaos {
    namespace common  {
        namespace thread {
            
            class Semaphore {
                //The current semaphore count.
                unsigned int count_;
                
                //Code that increments count_ must notify the condition variable.
                boost::condition_variable condition_;
                
                //Any code that reads or writes the count_ data must hold a lock on
                //the mutex.
                boost::mutex mutex_;
            public:

                explicit Semaphore(unsigned int initial_count);
                
                void notify();
                
                void wait();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__Semaphore__) */
