//
//  Semaphore.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 8/24/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/thread/Semaphore.h>

namespace thread = chaos::common::thread;

thread::Semaphore::Semaphore(unsigned int initial_count) : count_(initial_count), mutex_(), condition_() {
}


void thread::Semaphore::notify() {
    boost::unique_lock<boost::mutex> lock(mutex_);
    
    ++count_;
    
    //Wake up any waiting threads.
    //Always do this, even if count_ wasn't 0 on entry.
    //Otherwise, we might not wake up enough waiting threads if we
    //get a number of signal() calls in a row.
    condition_.notify_one();
}

void thread::Semaphore::wait() {
    boost::unique_lock<boost::mutex> lock(mutex_);
    while (count_ == 0) {
        condition_.wait(lock);
    }
    --count_;
}