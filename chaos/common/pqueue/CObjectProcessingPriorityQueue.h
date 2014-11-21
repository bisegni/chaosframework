/*
 *	CObjectProcessingPriorityQueue.h
 *	!CHOAS
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

#include <boost/thread.hpp>
#include <chaos/common/pqueue/CObjectProcessingQueue.h>
#include <chaos/common/utility/UUIDUtil.h>

#define COPPQUEUE_LAPP_ LAPP_ << uid << "[CObjectProcessingPriorityQueue] - "

namespace chaos {
    using namespace std;
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
        int priority;
        T *element;
        PriorityQueuedElement(T *_element, int _priority = 50, bool _disposeOnDestroy = true):disposeOnDestroy(_disposeOnDestroy), priority(_priority), element(_element) {}
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
            
    template<typename Type, typename Compare = std::less<Type> >
    struct pless : public std::binary_function<Type *, Type *, bool> {
        bool operator()(const Type *x, const Type *y) const {
            return Compare()(*x, *y);
        }
     };
            
#define PRIORITY_ELEMENT(e) PriorityQueuedElement< e >
            
            /*!
                Processing queue implemented with a priority_queue
             */
            template<typename T>
            class CObjectProcessingPriorityQueue {
                string uid;
                priority_queue< PRIORITY_ELEMENT(T)*, std::vector< PRIORITY_ELEMENT(T)* >, pless< PRIORITY_ELEMENT(T) > > bufferQueue;
                bool inDeinit;
                int outputThreadNumber;
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
                    while(!inDeinit) {
                        //get the oldest element
                    PRIORITY_ELEMENT(T)* dataRow = NULL;
                    ElementManagingPolicy elementPolicy;
                        //retrive the oldest element
                    dataRow = waitAndPop();
                        if(!dataRow) {
                            DEBUG_CODE(COPPQUEUE_LAPP_<<" waitAndPop() return NULL object so we return";)
                            continue;
                        }
                        //Process the element
                    try {
                        if(eventListener && !(*eventListener).elementWillBeProcessed(tag, dataRow->element)){
                            DELETE_OBJ_POINTER(dataRow);
                            continue;
                        }
                        elementPolicy.elementHasBeenDetached=false;
                        processBufferElement(dataRow->element, elementPolicy);
                        dataRow->disposeOnDestroy = !elementPolicy.elementHasBeenDetached;
                    } catch (CException& ex) {
                        DECODE_CHAOS_EXCEPTION(ex)
                    } catch (...) {
                        COPPQUEUE_LAPP_ << "Unkown exception";
                    }
                    
                        //if weg got a listener notify it
                    if(eventListener && !(*eventListener).elementWillBeDiscarded(tag, dataRow->element))continue;
                    
                    DELETE_OBJ_POINTER(dataRow);
                }
                    DEBUG_CODE(COPPQUEUE_LAPP_<< " executeOnThread ended";)
                }
                /*
                 Process the oldest element in buffer
                 */
                virtual void processBufferElement(T*, ElementManagingPolicy&) throw(CException) = 0;
                
            public:
                int tag;
                
				CObjectProcessingPriorityQueue():
				inDeinit(false),
				eventListener(NULL),
				uid(chaos::UUIDUtil::generateUUIDLite()) {
					
                }
                
                
                /*
                 Initialization method for output buffer
                 */
                virtual void init(int threadNumber) throw(CException) {
                    boost::unique_lock<boost::mutex>  lock(qMutex);
                    inDeinit = false;
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
                    inDeinit = true;
                    COPPQUEUE_LAPP_ << "Deinitialization";
                        //stopping the group
                    COPPQUEUE_LAPP_ << "Deinitializing Threads";
                    
                    if(waithForEmptyQueue){
                        COPPQUEUE_LAPP_ << "wait until queue is empty";
                        while( !bufferQueue.empty()){
                            emptyQueueConditionLock.wait(lock);
                        }
                        COPPQUEUE_LAPP_ << "queue is empty";
                    }
                    
                    COPPQUEUE_LAPP_ << "Stopping thread";
                    lock.unlock();
                    
                    liveThreadConditionLock.notify_all();
                    COPPQUEUE_LAPP_ << "join internal thread group";
                    t_group.join_all();
                    COPPQUEUE_LAPP_ << "deinitlized";
                }
                
                bool push(T* elementToPush, unsigned int _priority = 50, bool _disposeOnDestroy = true){
                    boost::unique_lock<boost::mutex>  lock(qMutex);
                    if(inDeinit) return false;
                    PriorityQueuedElement<T> *_element = new PriorityQueuedElement<T>(elementToPush, _priority, _disposeOnDestroy);
                    bufferQueue.push(_element);
                    lock.unlock();
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
                    
                    while(bufferQueue.empty() && !inDeinit) {
                        emptyQueueConditionLock.notify_one();
                        liveThreadConditionLock.wait(lock);
                    }
                        //get the oldest data ad copy the ahsred_ptr
                    if(bufferQueue.empty()) {
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
                        emptyQueueConditionLock.wait(lock);
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
                unsigned long elementInQueue() const {
                    boost::unique_lock<boost::mutex>  lock(qMutex);
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
#endif
