/*
 *	AbstractDeviceTracker.h
 *	!CHOAS
 *	Created by Antonucci Flaminio.
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

#ifndef CachingSystem_DeviceTracker_h
#define CachingSystem_DeviceTracker_h
#include <map>
#include <string>
#include <boost/thread.hpp>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>
#include <chaos/common/caching_system/caching_thread/GarbageThread.h>
#include <chaos/common/caching_system/caching_thread/BufferTracker.h>
#include <chaos/common/caching_system/common_buffer/helper/IdFactory.h>
#include <chaos/common/caching_system/common_buffer/data_element/SmartData.h>

namespace chaos {

namespace caching_system {
    namespace caching_thread{
        template <class T, class U = T>
        class AbstractDeviceTracker{
            
        private:
            DataFetcherInterface<U>* fetcher;
            std::string dev_id;
            
            uint64_t highResValidity;
            uint64_t highResHertz;
            std::map<int,BufferTracker<T>* > iteratorIdToBufferTracker;
           // std::vector<BufferTracker<T>* >* lowResQueuesVector;
            boost::mutex* bufferMapMutex;
            boost::condition_variable closedCondition;
            boost::mutex* closingLockMutex;
            double timeToFetchData;

          //  int idReader;
            GarbageThread<T>* garbageCollector;
            bool interrupted;
            
        protected:
            CommonBuffer<T>* highResQueue;
            std::map<std::string, BufferTracker<T>* > lowResQueues;
            
            void insertInSubBuffers(SmartPointer<T>* elementToAdd ,DataElement<T>* newElement){
                
                // DataElement<T>* newElement=elementToAdd->getElement();
                for(typename std::map<std::string, BufferTracker<T>* >::iterator it = this->lowResQueues.begin(); it != this->lowResQueues.end(); ++it){
                    
                    BufferTracker<T>* subBuffer=it->second;
                    if(subBuffer->needNextData(newElement->getTimestamp())){
                        SmartPointer<T>* localData=new SmartPointer<T>(elementToAdd->getElement());
                        SmartData<T >* smartData=new SmartData<T>(localData,newElement->getTimestamp());
                        subBuffer->getControlledBuffer()->enqueue(smartData,false);
                    }
                    
                }
            }

            
        public:

            AbstractDeviceTracker(DataFetcherInterface<U>* fetcher,uint64_t hertz,uint64_t validity,std::string dev_id){
                this->fetcher=fetcher;
                this->highResHertz=hertz;
                this->highResValidity=validity;
                this->dev_id=dev_id;
                this->highResValidity=validity;
                this->highResHertz=hertz;
                this->highResQueue=new CommonBuffer<T>(validity);
                this->interrupted=false;
                bufferMapMutex=new boost::mutex();
                closingLockMutex=new boost::mutex();
                
                this->timeToFetchData= 1000*((((double)1)/((double)hertz)));

                this->garbageCollector=new GarbageThread<T>(validity,&closedCondition,closingLockMutex);
            
            }

    
            IteratorReader<T>* openMaxResBuffer(){
                return new IteratorReader<T>(highResQueue);
        
            }
    
            IteratorReader<T>* openBuffer(uint64_t validity,uint64_t discretization){
                //first take a mutex lock on this queue
                boost::mutex::scoped_lock  lock(*bufferMapMutex);
                
                
                std::stringstream key;
                key<<validity<<":"<<discretization;
                
                BufferTracker<T>* buffer=lowResQueues[key.str()];
                if(buffer==NULL){
                    
                    //if the key doesn't exists, create it

                    CommonBuffer<T>* newBuffer= new CommonBuffer<T>(validity/**/);
                    buffer=new BufferTracker<T>(newBuffer,discretization);
                    lowResQueues[key.str()]=buffer;
                    IteratorReader<T>* iteratore=buffer->getIterator();
                    iteratorIdToBufferTracker[iteratore->getId()]=buffer;
                    this->garbageCollector->putQueueToGarbage(buffer->getIteratorGarbage());
                    //this->lowResQueuesVector
                    return iteratore;

                }else{
                    IteratorReader<T>* iteratore = buffer->getIterator();
                    iteratorIdToBufferTracker[iteratore->getId()]=buffer;

                    return iteratore;
                    
                }

                
                
    
            }
    
            
            void closeBuffer(IteratorReader<T>* iterator){
                boost::mutex::scoped_lock  lock(*bufferMapMutex);
                uint64_t idIterator=iterator->getId();
                BufferTracker<T>* buffer=iteratorIdToBufferTracker[idIterator];
                std::stringstream key;
                if(buffer==NULL){
                    //it is a max res buffer, i have di delete it explicitally
                    
                    iteratorIdToBufferTracker.erase(iteratorIdToBufferTracker.find(idIterator));

                    delete iterator;
                    return;
                }
                key<<buffer->getValidity()<<":"<<buffer->getDiscretization();
                
               
                
                buffer->deleteIterator(iterator);
                //if a buffer has no readers, you have to delete it.
                if(buffer->getNumberOfReader()==0){
                    //delete unused buffer
                    //i have to notify to garbage collector to remove this queue from his garbage.
                    this->garbageCollector->removeQueueToGarbage(buffer->getIteratorGarbage());
                    
                  //  std::cout<<"deleto il buffer\n";
                    delete buffer;

                    //delete it from the map, so it won't be update
                    lowResQueues.erase(lowResQueues.find(key.str()));
                
                }
             //  std::cout<<"mappa rpima delete\n";
              //  std::cout<<"size: "<<this->iteratorIdToBufferTracker.size()<<"\n";

                iteratorIdToBufferTracker.erase(iteratorIdToBufferTracker.find(idIterator));
          //      std::cout<<"mappa dopo delete\n";

          //      std::cout<<"size: "<<this->iteratorIdToBufferTracker.size()<<"\n";

                
            }
            
            void setTrackingFrequency(uint64_t hertz){
                
            
            }
            
            
            virtual void doTracking(DataElement<U>* newElement)=0;
            
            void operator()(){
                

                boost::thread collector(( boost::ref(*this->garbageCollector) ));
                garbageCollector->putQueueToGarbage(this->highResQueue->getIteratorGarbage());

                
                collector.detach();
               // std::cout<<"";
                while(!interrupted){
                    boost::posix_time::milliseconds workTime(timeToFetchData);
                    boost::this_thread::sleep(workTime);
                    sched_yield();
                  

                    boost::mutex::scoped_lock  lock(*bufferMapMutex);
                    DataElement<U>* newElement= this->fetcher->_getData();

                    doTracking(newElement);
                }
                
                boost::mutex::scoped_lock  closingLock(*closingLockMutex);

                garbageCollector->interrupt();
               
                this->closedCondition.wait(closingLock);

                //TODO before begin to testing
        
                //i have to wait that garbeg completes the shut down
        
        
                // now i have to delete all istanced queues
        
                delete this->highResQueue;
        
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
