/*
 *	GarbageThread.h
 *	!CHOAS
 *	Created by Flaminio Antonucci.
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

#ifndef CachingSystem_GarbageThread_h
#define CachingSystem_GarbageThread_h

#include <vector>
#include <boost/date_time.hpp>
#include <boost/thread.hpp>

#include <chaos/common/caching_system/common_buffer/IteratorGarbage.h>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include <chaos/common/caching_system/common_buffer/helper/IdFactory.h>


namespace chaos {
    
    
    namespace caching_system {
        
        namespace caching_thread{
            
            template<typename T>
            /*!
             * This class represent the Garbage collector thread for an entire device tracker. It has the responsability
             * of deleting old object and refreshing the pointer to last valid element. It is istanced direclty from
             * AbstractDeviceTracker. When you create a new buffer, remeber to add it to this object, so you can be sure
             * that it will be managed
             */
            
            class GarbageThread{
                
                
                
            private:
                
                //!< Iterator for all queues gave to this istance of Gabage collector
                std::vector<IteratorGarbage<T>* > queues;
                //!< Mutex lock for operation of deleting and inserting new queue to purge
                boost::mutex* _access;
                bool interrupted;
                //!< This time represent time interval for gabageThread to perform an update
                boost::posix_time::milliseconds* timeToSleep;
                //!<condition used to sincronize closing beetween gatbageTrhead and TrackerThread
                boost::condition_variable* conditionClose;
                boost::mutex* closingLockMutex;
                
                
            protected:
                
                
            public:
                
                /*!
                 * This thread should be instanced by a tracker. parameters are:
                 * \param millisToSleep time needed to perform a garbagin on the queue
                 * \param conditionClose condition for closing thread
                 * \param closingLockMutex mutex to syncronize closing with tracker
                 *
                 */
                GarbageThread(uint64_t millisToSleep,boost::condition_variable* conditionClose, boost::mutex* closingLockMutex){
                    // this->queues=new std::vector<IteratorGarbage<T>* >();
                    interrupted=false;
                    timeToSleep=new boost::posix_time::milliseconds(millisToSleep);
                    //creating acces lock to data structure
                    _access=new boost::mutex();
                    this->conditionClose=conditionClose;
                    this->closingLockMutex=closingLockMutex;
                }
                
                void operator()(){
                    
                    
                    //this function implements the behaviour of the garbagin thread
                    while(!interrupted){
                        
                        //sleep n millisecond
                        boost::this_thread::sleep(*timeToSleep);
                        
                        //gain lock on data structure to avoid change to them during garbaging
                        boost::mutex::scoped_lock  lock(*_access);
                        for(int j=0; j<queues.size();j++){
                            
                            //clear queue i
                            queues.at(j)->clearQueue();
                        }
                        
                    }
                    
                    //it's time to end work
                    
                    for(int i=0;i<queues.size();i++){
                        delete queues.at(i);
                        
                    }
                    queues.clear();
                    
                    boost::mutex::scoped_lock  closingLock(*closingLockMutex);
                    
                    // now i notify to tracker thaht my work is finished
                    this->conditionClose->notify_one();
                    
                    
                }
                
                /*!
                 * Interrupt the garbage thread, imposing bool interrupt to true.
                 * In conseguence of this call, GarbageThread will stop and close all his data structure
                 */
                void interrupt(){
                    this->interrupted=true;
                }
                
                
                /*!
                 * Add a queue to purging list of garbageCollector.
                 */
                void putQueueToGarbage(IteratorGarbage<T>* queueToPurge){
                    
                    boost::mutex::scoped_lock  lock(*_access);
                    queues.push_back(queueToPurge);
                    
                    
                }
                
                
                /*!
                 * Removes a queue from the purging list of GarbageCollector. So this queue
                 * will not be cleared anymore.
                 */
                void removeQueueToGarbage(IteratorGarbage<T>* queueToPurge){
                    
                    boost::mutex::scoped_lock  lock(*_access);
                    for(int i=0;i<queues.size();i++){
                        
                        IteratorGarbage<T>* temp=queues.at(i);
                        
                        if(temp->getId()==queueToPurge->getId()){
                            //found
                            queues.erase(queues.begin()+i);
                            //delete temp;
                            return;
                        }
                        
                    }
                    
                    
                }
                
            };
            
            
        }
    }
}
#endif
