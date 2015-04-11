/*
 *	FetcherDeviceTracker.h
 *	!CHAOS
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
#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>


#ifndef CachingSystem_ConcreteDeviceTracker_h
#define CachingSystem_ConcreteDeviceTracker_h
namespace chaos {
    
    namespace caching_system {
        namespace caching_thread{
            template<typename T> class DataFetcherInterface;
            
            template <typename T>
            
            /*!
             *
             */
            class FetcherDeviceTracker : public AbstractDeviceTracker<T> {
            private:
                boost::thread *threadTracker;
                bool interrupted;
                caching_system::DataFetcherInterface<T>* fetcher;
                
            public:
                FetcherDeviceTracker(caching_system::DataFetcherInterface<T>* fetcher,uint64_t msecSampleInterval,uint64_t validity): AbstractDeviceTracker<T>(/* fetcher, */msecSampleInterval, validity){
                    this->interrupted=false;
                    this->fetcher=fetcher;
                    
                    threadTracker = NULL;
                    
                }
                
                
                
                void track() {
                    this->startGarbaging();
                    
                    // std::cout<<"";
                    while(!this->interrupted){
                        boost::posix_time::microseconds workTime(this->timeToFetchData);
                        boost::this_thread::sleep(workTime);
                        sched_yield();
                        
                        
                        boost::mutex::scoped_lock  lock(*this->bufferMapMutex);
                        DataElement<T>* newElement= this->fetcher->_getData();
                        
                        this->doTracking(newElement);
                    }
                    this->shutDownTracking();
                    
                    
                }
                
                bool startTracking() {
                    this->threadTracker = new boost::thread(boost::bind(&FetcherDeviceTracker::track, this));
                    if(threadTracker) threadTracker->detach();
                    return threadTracker != NULL;
                }
                
                bool stopTracking() {
                    if(!this->threadTracker) return false;
                    
                    bool result = this->tryInterruptTracking();
                    if(result) {
                        this->threadTracker->join();
                    }
                    return result;
                }
                
                bool tryInterruptTracking(){
                    if(this->iteratorIdToBufferTracker.size()>0){
                        return false;
                    }else{
                        
                        this->interrupted=true;
                        return true;
                        
                    }
                }
            
            
            
            
        };
        
    }
}
}
#endif
