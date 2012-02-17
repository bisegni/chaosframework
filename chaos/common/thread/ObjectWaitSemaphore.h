/*	
 *	ObjectWaitSemaphore.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright <#Year#> INFN, National Institute of Nuclear Physics
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
     at the istance of this semaphore and all waith method return will return it(if it has been set). this class can mange only one thread
     waiting for one object
     \tparam T the type of the waited object
     */
    template<typename T>
    class ObjectWaitSemaphore {
            //! notify that a thread still wating answer
        bool inWait;
            //! sign that an unlock has been given with the answer
        bool answered;
            //! object waited by waiting lock
        T objecForWait;
            //! mutext used for unlock and wait esclusive access
        boost::mutex wait_answer_mutex;
            //! condition variable for wait the answer
        condition_variable wait_answer_condition;
    public:

            //!ObjectWaitSemaphore
        /*! 
         Default Constructor, initialize all internal variable
         */
        ObjectWaitSemaphore(){
            objecForWait = NULL;
            answered = false;
            inWait = false;
        }
        
            //!~ObjectWaitSemaphore
        /*! 
         Default Destructor, free the waited object if it's not has been keeped
         */
        ~ObjectWaitSemaphore(){
            if(objecForWait) delete(objecForWait);
        }
        
            //!setWaithedObject
        /*! 
         set the waited object
         */
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
