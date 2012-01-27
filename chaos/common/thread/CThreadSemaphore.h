//
//  CThreadSemaphore.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_CThreadSemaphore_h
#define ChaosFramework_CThreadSemaphore_h

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace chaos {
    
    class CThreadSemaphore {
        bool locked;
        boost::mutex mx;
        boost::condition_variable cv;
    
    public:
        
        CThreadSemaphore(){
            locked = true;
        }
        
        void wait() {
            boost::unique_lock<boost::mutex> lck(mx);
            while(locked) cv.wait(lck);
            locked=true;
        }
        
        void unlock(){
            {
                boost::lock_guard<boost::mutex> lck(mx);
                locked=false;
            }
            cv.notify_one();
        }
    };
}


#endif
