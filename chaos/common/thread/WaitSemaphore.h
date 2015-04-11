/*	
 *	WaitSemaphore.h
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

#ifndef CHAOSFramework_WaitSemaphore_h
#define CHAOSFramework_WaitSemaphore_h

#include <chaos/common/global.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace chaos {
    using namespace boost;
    
    //! Wait sempahore implementation
    /*! \class WaitSemaphore
     This implementation permit to a thread to be weakupped from another thread. In addition an object can be associated
     at the istance of this semaphore and all waith method return will return it(if it has been set). this class can mange only one thread
     waiting for one object
     \tparam T the type of the waited object
     */
    class WaitSemaphore {
        //! notify that a thread still wating answer
        bool inWait;
        //! sign that an unlock has been given with the answer
        bool answered;
        //! mutext used for unlock and wait esclusive access
        boost::mutex wait_answer_mutex;
        //! condition variable for wait the answer
        boost::condition_variable wait_answer_condition;
    public:
        
        //!WaitSemaphore
        /*! 
         Default Constructor, initialize all internal variable
         */
        WaitSemaphore():
        answered(false),
        inWait(false){}
        
        //!~WaitSemaphore
        /*! 
         Default Destructor, free the waited object if it's not has been keeped
         */
        ~WaitSemaphore(){}
        
        
        //!wait
        /*! 
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \return the object that someone has associated at this semaphore
         */
        void wait() {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return;
            inWait = true;
            while(!answered){wait_answer_condition.wait(lock);};
            inWait = false;
            answered = false;
        }
        
        //!wait
        /*! 
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \param millisecToWait is the time, in milliseconds that the read will wait
         if no one unlock it first
         \return the object that someone has associated at this semaphore
         */
        void wait(unsigned long millisecToWait) {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return;
            inWait = true;
            do {} while(wait_answer_condition.timed_wait(lock, posix_time::milliseconds(millisecToWait)) && !answered);
            inWait = false;
            answered = false;
        }
        
		//!wait
        /*!
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \param millisecToWait is the time, in milliseconds that the read will wait
         if no one unlock it first
         \return the object that someone has associated at this semaphore
         */
        void waitUSec(uint64_t microsecondsToWait) {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return;
            inWait = true;
            do {} while(wait_answer_condition.timed_wait(lock, posix_time::microseconds(microsecondsToWait)) && !answered);
            inWait = false;
            answered = false;
        }
		
        //!unlock
        /*! 
         unlock the waiting thread
         */
        void unlock(){
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
                //if(!inWait) return;
            answered = true;
            wait_answer_condition.notify_one();
        }
        
        bool isInWait() {
            return inWait;
        }
    };
}
#endif
