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

namespace chaos {
    
    
    namespace caching_system {
        
        namespace caching_thread{
            
            template<typename T>
            
            class GarbageThread{
                
                
                
            private:
                // CommonBuffer<T>* highResQueue;
                // long hertz;
                std::vector<IteratorGarbage<T>* > queues;
                boost::mutex* _access;
                bool interrupted;
                boost::posix_time::milliseconds* timeToSleep;
                boost::condition_variable* conditionClose;
                boost::mutex* closingLockMutex;
                
                
            protected:
                
                
            public:
                
                GarbageThread(uint64_t millisToSleep,boost::condition_variable* conditionClose, boost::mutex* closingLockMutex){
                    // this->queues=new std::vector<IteratorGarbage<T>* >();
                    interrupted=false;
                    timeToSleep=new boost::posix_time::milliseconds(millisToSleep);
                    // timeToSleep(millisToSleep);
                    //aggiungi la creazione del lock
                    _access=new boost::mutex();
                    this->conditionClose=conditionClose;
                    this->closingLockMutex=closingLockMutex;
                }
                
                void operator()(){
                    // std::cout<<"parto il garbage\n";
                    
                    while(!interrupted){
                        //  std::cout<<"cominicio sleep\n";
                        
                        boost::this_thread::sleep(*timeToSleep);
                        
                        boost::mutex::scoped_lock  lock(*_access);
                        
                        
                        for(int j=0; j<queues.size();j++){
                            //   std::cout<<"cancello\n";
                            queues.at(j)->clearQueue();
                            
                            //  std::cout<<"elementi "<<j<<": "<<queues.at(j)->getValidityWindowSize()<<"\n";
                            
                            
                        }
                        
                        //  std::cout<<"finisco lavoro e mi riaddormo\n";
                    }
                    
                    for(int i=0;i<queues.size();i++){
                        delete queues.at(i);
                        
                    }
                    queues.clear();
                    
                    boost::mutex::scoped_lock  closingLock(*closingLockMutex);
                    
                    // std::cout<<"invio la notify\n";;
                    this->conditionClose->notify_one();
                    
                    
                }
                
                // make this call blocking
                void interrupt(){
                    
                    
                    
                    this->interrupted=true;
                    
                }
                
                void putQueueToGarbage(IteratorGarbage<T>* queueToPurge){
                    
                    boost::mutex::scoped_lock  lock(*_access);
                    queues.push_back(queueToPurge);
                    
                    
                }
                
                
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
