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

#ifndef ChaosFramework_MultiKeyObjectWaitSemaphore_h
#define ChaosFramework_MultiKeyObjectWaitSemaphore_h

#include <map>

#include <chaos/common/global.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <chaos/common/thread/ObjectWaitSemaphore.h>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
        //! MultiKeyObjectWaitSemaphore An implementation for a multi semaphore associate to a generic keys
    /*! \class MultiKeyObjectWaitSemaphore
     \brief
     This semaphore implementation work on concept that at every semaphore is associated a generic key. A key can represent an event, with an object associated to it,
     for which a thread wait. 
     \tparam T is the type of the key
     \tparam A is the type of the object that will be associated at the event identified by the key
     */
    template<typename T, typename A>
    class MultiKeyObjectWaitSemaphore {
            //! mutex for regulate the multithreading map access
        boost::shared_mutex mapAccessMutex;
        
            //! Represent the mapping between the key and it semaphore data structure
        map<T, ObjectWaitSemaphore<A> *> keySemaphoreMap;
        
    public:
            //!MultiKeyObjectWaitSemaphore
        /*! 
         Base constructor fro initialize all structure
         */
        MultiKeyObjectWaitSemaphore(){
        }
        
            //!~MultiKeyObjectWaitSemaphore
        /*! 
         Base destructor for deinitialize all structure
         */
        ~MultiKeyObjectWaitSemaphore(){
            for ( typename map<T, ObjectWaitSemaphore<A>* >::iterator iter = keySemaphoreMap.begin(); 
                 iter != keySemaphoreMap.end();
                 iter++) {
                delete(iter->second);
            }
        }
        
            //!initKey
        /*! 
         *  \param keyToInit key for wich the semaphore structure need to be initialized
         */
        void initKey(T keyToInit){
            if(keySemaphoreMap.count(keyToInit)) return;
            
            boost::unique_lock< boost::shared_mutex >  mapLock(mapAccessMutex);
            
            keySemaphoreMap.insert(make_pair(keyToInit, new ObjectWaitSemaphore<A>()));
        }
            //!setWaithedObjectForKey
        /*! 
         * set the waiting object for the waiting thread
         *  \key is the key that represent the waiting thread for the object. Afther the unlock operation
         the struccure is removed from map
         */
        bool setWaithedObjectForKey(T key, A obj){
            boost::unique_lock< boost::shared_mutex >  mapLock(mapAccessMutex);
            if(!keySemaphoreMap.count(key)) return false;
            ObjectWaitSemaphore<A> *ks = keySemaphoreMap[key];
            if(!ks) {
                //before i need to dealloc pointed data because noone wait for it
                return false;
            }
            //set the waiting object
            ks->setWaithedObject(obj);
            ks->unlock();
            return true;
        }
        
            //!wait
        /*! 
         \param waitOnKey identify the key event for wich the current call thread need to wait
         \return the object taht the thread was waiting for
         */
        A wait(T waitOnKey) {
            boost::unique_lock< boost::shared_mutex >  mapLock(mapAccessMutex);
            
            A result = NULL;
            
            ObjectWaitSemaphore<A> *ks = keySemaphoreMap[waitOnKey]; mapLock.unlock();
            if(!ks) return NULL;
            result =  ks->wait();
            keySemaphoreMap.erase(waitOnKey);
            delete(ks);
            return result;
        }
        
            //!wait
        /*! 
         \param waitOnKey identify the key event for wich the current call thread need to wait
         \return the object taht the thread was waiting for
         */
        A wait(T waitOnKey, unsigned int millisecToWait) {
            ObjectWaitSemaphore<A> *ks = NULL;
            
            boost::unique_lock< boost::shared_mutex >  lockMap(mapAccessMutex);
            ks = keySemaphoreMap[waitOnKey]; 
            lockMap.unlock();
            
            if(!ks) return NULL;
            
            A result = ks->wait(millisecToWait);
            lockMap.lock();
            if(keySemaphoreMap.count(waitOnKey)){
                keySemaphoreMap.erase(waitOnKey);
            }
			delete(ks);
            lockMap.unlock();
            return result;
        }
    };
}
#endif
