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
