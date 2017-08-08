/*
 *	CObjectProcessingPriorityQueue.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
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
#ifndef Common_CObjectProcessingPriorityQueue_h
#define Common_CObjectProcessingPriorityQueue_h

#include <queue>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <chaos/common/pqueue/CObjectProcessingQueue.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>

#define COPPQUEUE_LAPP_ LAPP_ << "[CObjectProcessingPriorityQueue] - (" << uid << ") "
#define COPPQUEUE_LDBG_ LDBG_ << "[CObjectProcessingPriorityQueue] - (" << uid << ") "

#define CObjectProcessingPriorityQueue_MAX_ELEMENT_IN_QUEUE 1000

namespace chaos {
    namespace common {
        namespace pqueue {
            template<typename T>
            class CObjectProcessingPriorityQueue;
            /*
             Element for the heap
             */
            template <typename T>
            class PriorityQueuedElement {
                template<typename U>
                friend class CObjectProcessingPriorityQueue;
                
                
                bool disposeOnDestroy;
            public:
                uint64_t sequence_id;
                int priority;
                T *element;
                PriorityQueuedElement(T *_element,
                                      int _priority = 50,
                                      bool _disposeOnDestroy = true):
                disposeOnDestroy(_disposeOnDestroy),
                sequence_id(chaos::common::utility::TimingUtil::getTimeStampInMicroseconds()),
                priority(_priority),
                element(_element) {}
                PriorityQueuedElement(T *_element,
                                      uint64_t _sequence_id,
                                      int _priority = 50,
                                      bool _disposeOnDestroy = true):
                disposeOnDestroy(_disposeOnDestroy),
                sequence_id(_sequence_id),
                priority(_priority),
                element(_element) {}
                ~PriorityQueuedElement(){
                    if (disposeOnDestroy && element) {
                        delete(element);
                        element = NULL;
                    }
                }
                
                /*
                 Operator for heap
                 */
                bool operator < (const PriorityQueuedElement& b) const {
                    return priority < b.priority;
                }
                
                inline int getPriority() {
                    return priority;
                }
            };
            
#define PRIORITY_ELEMENT(e) chaos::common::pqueue::PriorityQueuedElement< e >
            
            // pulic class used into the sandbox for use the priority set into the lement that are pointer and not rela reference
            template<typename T>
            struct PriorityElementCompare {
                bool operator() (const PRIORITY_ELEMENT(T)* lhs, const PRIORITY_ELEMENT(T)* rhs) const {
                    if(lhs->priority < rhs->priority) {
                        return true;
                    } else if(lhs->priority == rhs->priority) {
                        return  lhs->sequence_id >= rhs->sequence_id;
                    } else {
                        return false;
                    }
                }
            };
            
            /*!
             Processing queue implemented with a priority_queue
             */
            template<typename T>
            class CObjectProcessingPriorityQueue {
                typedef std::priority_queue< PRIORITY_ELEMENT(T)*,
                std::vector< PRIORITY_ELEMENT(T)* >,
                PriorityElementCompare<T> > ChaosPriorityQueue;
                
                std::string uid;
                ChaosPriorityQueue bufferQueue;
                bool in_deinit;
                boost::mutex qMutex;
                boost::condition_variable liveThreadConditionLock;
                boost::condition_variable emptyQueueConditionLock;
                
                //thread group
                boost::thread_group t_group;
                
            protected:
                
                CObjectProcessingQueueListener<T> *eventListener;
                
                /*
                 Thread method that work on buffer item
                 */
                void executeOnThread() {
                    while(!in_deinit) {
                        //get the oldest element
                        PRIORITY_ELEMENT(T)* dataRow = NULL;
                        ElementManagingPolicy elementPolicy;
                        //retrive the oldest element
                        dataRow = waitAndPop();
                        if(!dataRow) {
                            DEBUG_CODE(COPPQUEUE_LDBG_<<" waitAndPop() return NULL object so we return";)
                            continue;
                        }
                        //Process the element
                        try {
                            if(eventListener &&
                               !(*eventListener).elementWillBeProcessed(tag, dataRow->element)){
                                DELETE_OBJ_POINTER(dataRow);
                                continue;
                            }
                            elementPolicy.elementHasBeenDetached=false;
                            processBufferElement(dataRow->element, elementPolicy);
                            dataRow->disposeOnDestroy = !elementPolicy.elementHasBeenDetached;
                        } catch (CException& ex) {
                            DECODE_CHAOS_EXCEPTION(ex)
                        } catch (...) {
                            COPPQUEUE_LAPP_ << "Unknown exception";
                        }
                        
                        //if weg got a listener notify it
                        if(eventListener && !(*eventListener).elementWillBeDiscarded(tag, dataRow->element))continue;
                        
                        DELETE_OBJ_POINTER(dataRow);
                    }
                    DEBUG_CODE(COPPQUEUE_LDBG_<< " executeOnThread ended";)
                }
                /*
                 Process the oldest element in buffer
                 */
                virtual void processBufferElement(T*, ElementManagingPolicy&) throw(CException) = 0;
                
            public:
                int tag;
                
                CObjectProcessingPriorityQueue():
                in_deinit(false),
                eventListener(NULL),
                uid(common::utility::UUIDUtil::generateUUIDLite()),
                tag(0){}
                
                CObjectProcessingPriorityQueue(CObjectProcessingQueueListener<T> *_eventListener):
                in_deinit(false),
                eventListener(_eventListener),
                uid(common::utility::UUIDUtil::generateUUIDLite()),
                tag(0){}
                
                /*
                 Initialization method for output buffer
                 */
                virtual void init(int threadNumber) throw(CException) {
                    boost::unique_lock<boost::mutex>  lock(qMutex);
                    in_deinit = false;
                    COPPQUEUE_LAPP_ << "init";
                    //add the n thread on the threadgroup
                    COPPQUEUE_LAPP_ << "creating " << threadNumber << " thread";
                    for (int idx = 0; idx<threadNumber; idx++) {
                        t_group.create_thread(boost::bind(&CObjectProcessingPriorityQueue<T>::executeOnThread, this));
                    }
                    COPPQUEUE_LAPP_ << "Initialized";
                    }
                    
                    /*
                     Deinitialization method for output buffer
                     */
                    virtual void deinit(bool waithForEmptyQueue=true) throw(CException) {
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        COPPQUEUE_LAPP_ << "Deinitialization";
                        
                        if(waithForEmptyQueue){
                            COPPQUEUE_LAPP_ << "wait until queue is empty";
                            while(!bufferQueue.empty()){
                                emptyQueueConditionLock.timed_wait(lock,
                                                                   boost::posix_time::milliseconds(500));
                            }
                            COPPQUEUE_LAPP_ << "queue is empty";
                        }
                        try {
                            if(lock.owns_lock()) lock.unlock();
                        }catch(...) {
                            COPQUEUE_LERR_ << "Error unlocking";
                        }
                        COPPQUEUE_LAPP_ << "Stopping thread";
                        in_deinit = true;
                        liveThreadConditionLock.notify_all();
                        COPPQUEUE_LAPP_ << "join internal thread group";
                        t_group.join_all();
                        COPPQUEUE_LAPP_ << "deinitialized";
                    }
                    
                    bool push(T* elementToPush,
                              int32_t _priority = 0,
                              bool _disposeOnDestroy = true){
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        if(in_deinit ||
                           bufferQueue.size() > CObjectProcessingPriorityQueue_MAX_ELEMENT_IN_QUEUE) return false;
                        PriorityQueuedElement<T> *_element = new PriorityQueuedElement<T>(elementToPush, _priority, _disposeOnDestroy);
                        bufferQueue.push(_element);
                        liveThreadConditionLock.notify_one();
                        return true;
                    }
                    
                    /*
                     get the last insert data
                     */
                    PRIORITY_ELEMENT(T)* waitAndPop() {
                        boost::unique_lock<boost::mutex> lock(qMutex);
                        //output result poitner
                        PriorityQueuedElement<T> *prioritizedElement = NULL;
                        
                        while(bufferQueue.empty() &&
                              !in_deinit) {
                            liveThreadConditionLock.wait(lock);
                        }
                        //get the oldest data ad copy the ahsred_ptr
                        if(bufferQueue.empty()) {
                            DEBUG_CODE(COPPQUEUE_LDBG_<< "bufferQueue.empty() is empty so we go out";)
                            return NULL;
                        }
                        //get the last pointer from the queue
                        prioritizedElement = bufferQueue.top();
                        
                        //remove the element
                        bufferQueue.pop();
                        return prioritizedElement;
                    }
                    
                    /*
                     check for empty buffer
                     */
                    bool isEmpty() const {
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        return bufferQueue.empty();
                    }
                    
                    /*
                     check for empty buffer
                     */
                    void waitForEmpty() {
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        while( bufferQueue.empty()){
                            emptyQueueConditionLock.timed_wait(lock,
                                                               boost::posix_time::milliseconds(500));
                        }
                        //return bufferQueue.empty();
                    }
                    
                    
                    /*
                     clear the queue, remove all non processed element
                     */
                    void clear() {
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        
                        //remove all element
                        while (!bufferQueue.empty()) {
                            //delete the next element
                            PRIORITY_ELEMENT(T) *toDelete =  bufferQueue.top();
                            if(!toDelete) return;
                            //remove upper element
                            bufferQueue.pop();
                            
                            //delete element
                            DELETE_OBJ_POINTER(toDelete);
                        }
                    }
                    
                    /*
                     Return le number of elementi into live data
                     */
                    unsigned long queueSize() {
                        boost::unique_lock<boost::mutex> lock(qMutex);
                        return bufferQueue.size();
                    }
                    
                    /*
                     Assign the elaboration Listener
                     */
                    void setEndElaborationListener(CObjectProcessingQueueListener<T> *objPtr) {
                        eventListener = objPtr;
                    }
                    
                    /*
                     Whait the thread termination
                     */
                    void joinBufferThread() {
                        t_group.join_all();
                    }
                    };
                    }
                    }
                    }
#endif
