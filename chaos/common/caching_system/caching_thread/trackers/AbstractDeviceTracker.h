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
#include <vector>
#include <boost/thread.hpp>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
#include <chaos/common/caching_system/caching_thread/GarbageThread.h>
#include <chaos/common/caching_system/caching_thread/BufferTracker.h>
#include <chaos/common/caching_system/common_buffer/helper/IdFactory.h>
#include <chaos/common/caching_system/common_buffer/data_element/SmartData.h>
#include <chaos/common/caching_system/caching_thread/trackers/TrackerListener.h>
#include <chaos/common/caching_system/caching_thread/tracker_interface/EmbeddedDataTransform.h>

namespace chaos {
    
    namespace caching_system {
        namespace caching_thread{
            template <typename T, typename D>
            
            class TransformTracker;
            
            template <class T, class U = T>
            
            /*!
             * This class represents an abstraction of Tracker. It has responsability to mantain sub buffer information
             * and the logic needed for management, including garbaging of controlled buffer.
             */
            class AbstractDeviceTracker{
                
            private:
                
                //!< Validity for an element inserted in the queue
                uint64_t highResValidity;
                //!< 
                uint64_t highResHertz;
                boost::condition_variable closedCondition;
                boost::mutex closingLockMutex;
                //!<this vector is used to account listeners for this tracker
                std::vector<TrackerListener<T>* > * trackerListeners;
                //!< reference to current istance of garbage collector
                GarbageThread<T>* garbageCollector;
                
                //!< reference to high res queue, that is the principal queue
                CommonBuffer<T>* highResQueue;

                
                /*!
                 * Utility function for initializing this data structure, called by costructor
                 */
                void init( uint64_t usecSampleInterval, uint64_t validity){
                    this->highResHertz=usecSampleInterval;
                    this->highResValidity=validity;
                    this->highResQueue=new CommonBuffer<T>(validity);
                    bufferMapMutex=new boost::mutex();
                    // closingLockMutex=new boost::mutex();
                    highResIterator=new std::vector< IteratorReader<T>* >();
                    setSleepTimeInUS(usecSampleInterval);
                    this->garbageCollector=new GarbageThread<T>(validity,&closedCondition,&closingLockMutex);
                    trackerListeners=new std::vector< TrackerListener<T>* >();
                    
                }
                
                
            protected:

                int64_t timeToFetchData;
                std::map<int,BufferTracker<T>* > iteratorIdToBufferTracker;
                std::vector<IteratorReader<T>*  >* highResIterator;
                boost::mutex* bufferMapMutex;
                std::map<std::string, BufferTracker<T>* > lowResQueues;
                
                void insertInSubBuffers(SmartPointer<T>* elementToAdd ,DataElement<T>* newElement){
                    
                    for(typename std::map<std::string, BufferTracker<T>* >::iterator it = this->lowResQueues.begin(); it != this->lowResQueues.end(); ++it){
                        
                        BufferTracker<T>* subBuffer=it->second;
                        if(subBuffer->needNextData(newElement->getTimestamp())){
                            SmartPointer<T>* localData=new SmartPointer<T>(elementToAdd->getElement());
                            SmartData<T >* smartData=new SmartData<T>(localData,newElement->getTimestamp());
                            subBuffer->getControlledBuffer()->enqueue(smartData,false);
                        }
                        
                    }
                }
                
                /*!
                 * Utility Function used to give new data to all listener registered in this tracker
                 */
                void insertInListener(SmartPointer<T>* elementToAdd,DataElement<T>* newElement){
                    for(int i=0;i<this->trackerListeners->size();i++){
                        
                        TrackerListener<T>* current=this->trackerListeners->at(i);
                        
                        current->addedNewelement(elementToAdd,newElement->getTimestamp());
                    }
                    
                }
                
                void inline addListener(TrackerListener<T>* listener){
                    
                    //push it in listeners
                    this->trackerListeners->push_back(listener);
                    
                }
                
                
            public:
                
                
                
                AbstractDeviceTracker(uint64_t usecSampleInterval, uint64_t validity){
                    init(usecSampleInterval, validity);
                    
                }
                
           
                template<typename D>
                TransformTracker<T,D>* getNewTranformTracker(EmbeddedDataTransform<T,D>* filter,uint64_t usecSampleInterval,uint64_t validity ) {
                    
                    //caching_system::DataFetcherInterface<T>* fetcher,uint64_t hertz,uint64_t validity
                    
                    //create a new tracker with basic transformation
                    TransformTracker<T,D>* transformTracker = new TransformTracker<T,D>(filter, usecSampleInterval, validity);
                    
                    //add it on listners vector
                    addListener(transformTracker);
                    
                    transformTracker->startGarbaging();
                    return transformTracker;
                    
                }
                
                void setSleepTimeInHZ(double hertz) {
                    this->timeToFetchData = 1000000*((((double)1)/((double)hertz)));
                }
                
                void setSleepTimeInUS(uint64_t ms) {
                    this->timeToFetchData = ms;
                }
                
                
                IteratorReader<T>* openMaxResBuffer(){
                    
                    int id=caching_system::helper::getNewId();
                    
                    IteratorReader<T>* newIterator= new IteratorReader<T>(highResQueue);
                    
                    this->highResIterator->push_back(newIterator);
                    
                    newIterator->setId(id);
                    return newIterator;
                    
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
                        
                        //removes from map element accidentally created
                        iteratorIdToBufferTracker.erase(iteratorIdToBufferTracker.find(idIterator));
                        
                        //look for id, and remove it from vector, then delete iterator.
                        for(int i = 0;i<highResIterator->size();i++){
                            
                            if(iterator->getId()==highResIterator->at(i)->getId()){
                                
                                this->highResIterator->erase(highResIterator->begin()+i);
                                delete iterator;
                                
                                
                            }
                        }
                        
                        
                        return;
                    }
                    
                    //if it is a low resolution iterator, i have to remove from BufferTracker and,
                    //if BufferTracker is not related to any others iterator, i have to destroy it
                    // destroyng BufferTracker implies to put away the subqueue also from garbagign
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
                    
                    iteratorIdToBufferTracker.erase(iteratorIdToBufferTracker.find(idIterator));
                }
                
                
                virtual void doTracking(DataElement<U>* newElement){
                    
                    //enqueue new element in high res queue
                    SmartPointer<T>* mySmartData=this->highResQueue->enqueue(newElement,true);
                    
                    //then, insert it in subBuffer
                    this->insertInSubBuffers(mySmartData,newElement);
                    
                    //finally, give new data to listeners objects.
                    
                    this->insertInListener(mySmartData,newElement);
                    
                    delete mySmartData;
                    
                }
                
                
                               
                void shutDownTracking(){
                    
                    
                    //this shutdown works, but it is very slow... it must be revisited...
                    
                    
                    boost::mutex::scoped_lock  closingLock(closingLockMutex);
                    
                    
                    //stopping garbaging and his iterators
                    stopGarbaging();
                    
                    
                    this->closedCondition.wait(closingLock);
                    
                    //delete all listener
                    for(int i=0;i<this->trackerListeners->size();i++){
                        delete this->trackerListeners->at(i);
                        
                    }
                    this->trackerListeners->clear();
                    delete this->trackerListeners;
                    
                    //delete all high res iterator
                    
                    for(int i=0;i<highResIterator->size();i++){
                        closeBuffer(highResIterator->at(i));
                        
                    }
                    //delete all buffer trackers
                    
                    // iteratorIdToBufferTracker
                    
                    //now i take from map all keys
                    std::map<int, BufferTracker<T>* > m;
                    typename std::map<int,BufferTracker<T>* >::iterator it;
                    for( it = m.begin(); it != m.end(); ++it) {
                        //  v.push_back(it->first);
                        delete it->second;
                    }
                    
                    //then i will close all Buffer
                    delete highResQueue;
                    
                    //delete all stuff, including garbage, buffertracker (if any) and other references
                    
                    
                    //delete closingLockMutex;
                    
                    
                    
                    delete garbageCollector;
                    
                    
                    
                    //delete last high resolution queue
                    //  delete this->highResQueue;
                    
                }
                
                
                
                
                
                
                
                                
                
                void startGarbaging(){
                    
                    boost::thread collector(( boost::ref(*this->garbageCollector) ));
                    this->garbageCollector->putQueueToGarbage(this->highResQueue->getIteratorGarbage());
                    collector.detach();
                }

                
             
                
                void stopGarbaging(){
                    garbageCollector->interrupt();
                    
                }
                
                
            };
        }
    }
}

#include <chaos/common/caching_system/caching_thread/trackers/TransformTracker.h>

#endif
