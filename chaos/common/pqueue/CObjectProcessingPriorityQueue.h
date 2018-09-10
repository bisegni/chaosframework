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
            public:
                typedef typename ChaosSharedPtr<T> PriorityQueuedElementType;
                uint64_t sequence_id;
                int priority;
                PriorityQueuedElementType element;
                PriorityQueuedElement(PriorityQueuedElementType _element,
                                      int _priority = 50):
                sequence_id(chaos::common::utility::TimingUtil::getTimeStampInMicroseconds()),
                priority(_priority),
                element(MOVE(_element)) {}
                PriorityQueuedElement(T *_element,
                                      uint64_t _sequence_id,
                                      int _priority = 50,
                                      bool _disposeOnDestroy = true):
                sequence_id(_sequence_id),
                priority(_priority),
                element(_element) {}
                ~PriorityQueuedElement(){}
                
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
            
#define PRIORITY_ELEMENT(e) ChaosSharedPtr< chaos::common::pqueue::PriorityQueuedElement< e > >
            
            // pulic class used into the sandbox for use the priority set into the lement that are pointer and not rela reference
            template<typename T>
            struct PriorityElementCompare {
                bool operator() (const PRIORITY_ELEMENT(T)& lhs, const PRIORITY_ELEMENT(T)& rhs) const {
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
                typedef std::priority_queue< PRIORITY_ELEMENT(T),
                std::vector< PRIORITY_ELEMENT(T) >,
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
                /*
                 Thread method that work on buffer item
                 */
                void executeOnThread() {
                    while(!in_deinit) {
                        //get the oldest element
                        PRIORITY_ELEMENT(T) dataRow;
                        //retrive the oldest element
                        dataRow = waitAndPop();
                        if(!dataRow.get()) {
                            DEBUG_CODE(COPPQUEUE_LDBG_<<" waitAndPop() return NULL object so we return";)
                            continue;
                        }
                        //Process the element
                        try {
                            processBufferElement(MOVE(dataRow->element));
                        } catch (CException& ex) {
                            DECODE_CHAOS_EXCEPTION(ex)
                        } catch (...) {
                            COPPQUEUE_LAPP_ << "Unknown exception";
                        }
                    }
                    DEBUG_CODE(COPPQUEUE_LDBG_<< " executeOnThread ended";)
                }
                /*
                 Process the oldest element in buffer
                 */
                virtual void processBufferElement(typename PriorityQueuedElement<T>::PriorityQueuedElementType) = 0;
                
            public:
                CObjectProcessingPriorityQueue():
                in_deinit(false),
                uid(common::utility::UUIDUtil::generateUUIDLite()){}
                
                /*
                 Initialization method for output buffer
                 */
                virtual void init(int threadNumber) {
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
                    virtual void deinit(bool waithForEmptyQueue=true) {
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
                    
                    bool push(typename PriorityQueuedElement<T>::PriorityQueuedElementType elementToPush,
                              int32_t _priority = 0){
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        if(in_deinit ||
                           bufferQueue.size() > CObjectProcessingPriorityQueue_MAX_ELEMENT_IN_QUEUE) return false;
                        //PRIORITY_ELEMENT(T) element(elementToPush);
                        bufferQueue.push(ChaosSharedPtr< PriorityQueuedElement<T> >(new PriorityQueuedElement<T>(elementToPush, _priority)));
                        liveThreadConditionLock.notify_one();
                        return true;
                    }
                    
                    /*
                     get the last insert data
                     */
                    PRIORITY_ELEMENT(T) waitAndPop() {
                        boost::unique_lock<boost::mutex> lock(qMutex);
                        //output result poitner
                        PRIORITY_ELEMENT(T) prioritizedElement;
                        
                        while(bufferQueue.empty() &&
                              !in_deinit) {
                            liveThreadConditionLock.wait(lock);
                        }
                        //get the oldest data ad copy the ahsred_ptr
                        if(bufferQueue.empty()) {
                            DEBUG_CODE(COPPQUEUE_LDBG_<< "bufferQueue.empty() is empty so we go out";)
                            return PRIORITY_ELEMENT(T)();
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
                    }
                    
                    
                    /*
                     clear the queue, remove all non processed element
                     */
                    void clear() {
                        boost::unique_lock<boost::mutex>  lock(qMutex);
                        //remove all element
                        while (!bufferQueue.empty()) {
                            //remove upper element
                            bufferQueue.pop();
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
