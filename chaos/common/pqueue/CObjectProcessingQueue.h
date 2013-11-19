/*
 *	CObjectProcessingQueue.h
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
#ifndef CObjectProcessingQueue_H
#define CObjectProcessingQueue_H

#include "CObjectProcessingQueueListener.h"
#include <chaos/common/thread/CThreadExecutionTask.h>
#include <chaos/common/thread/CThreadGroup.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <queue>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#define LOG_HEAD uid << "[CObjectProcessingQueue] - "
#define COPQUEUE_LAPP_ LAPP_ << LOG_HEAD
#define COPQUEUE_LDBG_ LDBG_ << LOG_HEAD


namespace chaos {
    using namespace std;
    using namespace boost;
    
    typedef struct {
        bool elementHasBeenDetached;
    } ElementManagingPolicy;
    
	
    /*
	 Base class for the Output Buffer structure
     */
    template<typename T>
    class CObjectProcessingQueue {
        string uid;
		//thread group
        //CThreadGroup threadGroup;
		boost::thread_group t_group;
    protected:
        queue<T*> bufferQueue;
        bool inDeinit;
        int outputThreadNumber;
        mutable boost::mutex qMutex;
        boost::condition_variable liveThreadConditionLock;
        boost::condition_variable emptyQueueConditionLock;
		
        CObjectProcessingQueueListener<T> *eventListener;
        
        /*
         Thread method that work on buffer item
         */
        void executeOnThread() {
			//get the oldest element
			while(!inDeinit) {
				T* dataRow = NULL;
				ElementManagingPolicy elementPolicy;
                //retrive the oldest element
				dataRow = waitAndPop();
				if(!dataRow) {
					DEBUG_CODE(COPQUEUE_LDBG_<<" waitAndPop() return NULL object so we return";)
					continue;
				}
                //Process the element
				try {
					if(eventListener && !(*eventListener).elementWillBeProcessed(tag, dataRow)){
						DELETE_OBJ_POINTER(dataRow);
						continue;
					}
					elementPolicy.elementHasBeenDetached=false;
					processBufferElement(dataRow, elementPolicy);
					if(elementPolicy.elementHasBeenDetached) continue;
				} catch (CException& ex) {
					DECODE_CHAOS_EXCEPTION(ex)
				} catch (...) {
					COPQUEUE_LAPP_ << " Unkown exception";
				}
				
                //if weg got a listener notify it
				if(eventListener && !(*eventListener).elementWillBeDiscarded(tag, dataRow))return;
				
				DELETE_OBJ_POINTER(dataRow);
			}
			DEBUG_CODE(COPQUEUE_LDBG_<< " executeOnThread ended";)
        }
        
        /*
		 Process the oldest element in buffer
         */
        virtual void processBufferElement(T*, ElementManagingPolicy&) throw(CException) = 0;
        
    public:
        int tag;
        
        CObjectProcessingQueue(){
            inDeinit = false;
            eventListener=0L;
			uid = chaos::UUIDUtil::generateUUIDLite();
        }
		
        /*
         Set the internal thread delay for execute new task
		 
		 void setDelayBeetwenTask(long threadDelay){
		 threadGroup.setDelayBeetwenTask(threadDelay);
		 } */
        
        /*
		 Initialization method for output buffer
         */
        virtual void init(int threadNumber) throw(CException) {
            inDeinit = false;
            COPQUEUE_LAPP_ << " init";
			//add the n thread on the threadgroup
            COPQUEUE_LAPP_ << " creating and starting" << threadNumber << " thread";
            for (int idx = 0; idx<threadNumber; idx++) {
                t_group.create_thread(boost::bind(&CObjectProcessingQueue<T>::executeOnThread, this));
            }
            
            //COPQUEUE_LAPP_ << "Starting all thread";
            //threadGroup.startGroup();
            COPQUEUE_LAPP_ << " Initialized";
			}
			
			/*
			 Deinitialization method for output buffer
			 */
			virtual void deinit(bool waithForEmptyQueue=true) throw(CException) {
				boost::mutex::scoped_lock lock(qMutex);
				inDeinit = true;
				COPQUEUE_LAPP_ << " Deinitialization";
                //stopping the group
				COPQUEUE_LAPP_ << " Deinitializing Threads";
				
				if(waithForEmptyQueue){
					COPQUEUE_LAPP_ << " wait until queue is empty";
					while( !bufferQueue.empty()){
						emptyQueueConditionLock.wait(lock);
					}
					COPQUEUE_LAPP_ << " queue is empty";
				}
				
				COPQUEUE_LAPP_ << " Stopping thread";
				//threadGroup.stopGroup(false);
				lock.unlock();
				
				liveThreadConditionLock.notify_all();
				COPQUEUE_LAPP_ << " join internal thread group";
				t_group.join_all();
				COPQUEUE_LAPP_ << " deinitlized";
			}
			
			/*
			 push the row value into the buffer
			 */
			virtual bool push(T* data) throw(CException) {
				boost::mutex::scoped_lock lock(qMutex);
				if(inDeinit) return false;
				bufferQueue.push(data);
				lock.unlock();
				liveThreadConditionLock.notify_all();
				return true;
			}
			
			/*
			 get the last insert data
			 */
			T* waitAndPop() {
				boost::mutex::scoped_lock lock(qMutex);
                //output result poitner
				T *oldestElement = NULL;
				DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() begin to wait";)
				while(bufferQueue.empty() && !inDeinit) {
					emptyQueueConditionLock.notify_one();
					liveThreadConditionLock.wait(lock);
				}
				DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() wakeup";)
                //get the oldest data ad copy the ahsred_ptr
				if(bufferQueue.empty()) {
					DEBUG_CODE(COPQUEUE_LDBG_<< " bufferQueue.empty() is empy so we go out";)
					return NULL;
				}
                //get the last pointer from the queue
				oldestElement = bufferQueue.front();
				
                //remove the oldest data
				bufferQueue.pop();
				
				return oldestElement;
			}
			
			/*
			 check for empty buffer
			 */
			bool isEmpty() const {
				boost::mutex::scoped_lock lock(qMutex);
				return bufferQueue.empty();
			}
			
			/*
			 check for empty buffer
			 */
			void waitForEmpty() {
				boost::mutex::scoped_lock lock(qMutex);
				while( bufferQueue.empty()){
					emptyQueueConditionLock.wait(lock);
				}
				return bufferQueue.empty();
			}
			
			
			/*
			 clear the queue, remove all non processed element
			 */
			void clear() {
				boost::mutex::scoped_lock lock(qMutex);
				
                //remove all element
				while (!bufferQueue.empty()) {
					bufferQueue.pop();
				}
			}
			
			/*
			 Return le number of elementi into live data
			 */
			unsigned long elementInLiveBuffer() const {
				boost::mutex::scoped_lock lock(qMutex);
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
			
