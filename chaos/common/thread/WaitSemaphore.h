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
        void wait();
        void waitRaw();
        //!wait
        /*! 
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \param millisecToWait is the time, in milliseconds that the read will wait
         if no one unlock it first
         \return the object that someone has associated at this semaphore
         */
        void wait(unsigned long millisecToWait);
        
		//!wait
        /*!
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \param millisecToWait is the time, in milliseconds that the read will wait
         if no one unlock it first
         \return the object that someone has associated at this semaphore
         */
        void waitUSec(uint64_t microsecondsToWait);
		
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
         void notifyAll(){
           
            answered = true;
            wait_answer_condition.notify_all();
        }
        bool isInWait() {
            return inWait;
        }
    };
}
#endif
