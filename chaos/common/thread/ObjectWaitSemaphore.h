//
//  ObjectWaitSemaphore.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 28/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ObjectWaitSemaphore_h
#define CHAOSFramework_ObjectWaitSemaphore_h

#include <chaos/common/global.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace chaos {
    using namespace boost;
    
        //! Wait sempahore implementation
    /*! \class ObjectWaitSemaphore
     This implementation permit to a thread to be weakupped from another thread. In addition an object can be associated
     at the istance of this semaphore and all waith method return will return it(if it has been set)
     */
    template<typename T>
    class ObjectWaitSemaphore {
        T objecForWait;
        boost::mutex wait_answer_mutex;
        condition_variable wait_answer_condition;
    public:
        bool answered;
        bool inWait;
        
        ObjectWaitSemaphore(){
            objecForWait = NULL;
            answered = false;
            inWait = false;
        }
        
        ~ObjectWaitSemaphore(){
            if(objecForWait) delete(objecForWait);
        }
        
        void setWaithedObject(T _objecForWait){
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            objecForWait = _objecForWait;
        }
        
            //!wait
        /*! 
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \return the object that someone has associated at this semaphore
         */
        T wait() {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return NULL;
            inWait = true;
            while ( ! answered )
                wait_answer_condition.wait(lock);
            inWait = false;
            T result = objecForWait;
            objecForWait = NULL;
            return result;
        }
        
            //!wait
        /*! 
         the thread that call this method will wait or an ammount of millisecond
         or a unlock event
         \param millisecToWait is the time, in milliseconds that the read will wait
         if no one unlock it first
         \return the object that someone has associated at this semaphore
         */
        T wait(unsigned int millisecToWait) {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return NULL;
            inWait = true;
            wait_answer_condition.timed_wait(lock, posix_time::milliseconds(millisecToWait));
            inWait = false;
            T result = objecForWait;
            objecForWait = NULL;
            return result;
        }
        
            //!unlock
        /*! 
         unlock the waiting thread
         */
        void unlock(){
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            answered = true;
            wait_answer_condition.notify_one();
        }
    };
}
#endif
