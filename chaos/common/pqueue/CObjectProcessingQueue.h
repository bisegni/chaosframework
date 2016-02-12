/*
 *	CObjectProcessingQueue.h
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
#ifndef CObjectProcessingQueue_H
#define CObjectProcessingQueue_H

#include <chaos/common/pqueue/CObjectProcessingQueueListener.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <queue>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#define LOG_HEAD "[CObjectProcessingQueue] - "
#define COPQUEUE_LAPP_ LAPP_ << LOG_HEAD
#define COPQUEUE_LDBG_ LDBG_ << LOG_HEAD
#define COPQUEUE_LERR_ LERR_ << LOG_HEAD << "(" << __LINE__ << ")"

#define CObjectProcessingQueue_MAX_ELEMENT_IN_QUEUE 1000
namespace chaos {
	
	
	typedef struct {
		bool elementHasBeenDetached;
	} ElementManagingPolicy;
	
	
	/*
	 Base class for the Output Buffer structure
	 */
	template<typename T>
	class CObjectProcessingQueue {
		std::string uid;
		//thread group
		//CThreadGroup threadGroup;
		boost::thread_group t_group;
	protected:
		std::queue<T*> bufferQueue;
		//boost::lockfree::queue<T*> bufferQueue;
		bool in_deinit;
		int output_thread_number;
		mutable boost::mutex qMutex;
		boost::condition_variable liveThreadConditionLock;
		boost::condition_variable emptyQueueConditionLock;
		
		CObjectProcessingQueueListener<T> *eventListener;
		
		/*
		 Thread method that work on buffer item
		 */
		void executeOnThread() {
			//get the oldest element
			while(!in_deinit) {
				T* dataRow = NULL;
				ElementManagingPolicy elementPolicy;
				//retrive the oldest element
				dataRow = waitAndPop();
				if(!dataRow) {
					DEBUG_CODE(COPQUEUE_LDBG_<<"waitAndPop() return NULL object so we return";)
					continue;
				}
				//Process the element
				try {
					if(eventListener &&
					   !(*eventListener).elementWillBeProcessed(tag, dataRow)){
						DELETE_OBJ_POINTER(dataRow);
						continue;
					}
					elementPolicy.elementHasBeenDetached=false;
					processBufferElement(dataRow, elementPolicy);
					if(elementPolicy.elementHasBeenDetached) continue;
				} catch (CException& ex) {
					DECODE_CHAOS_EXCEPTION(ex)
				} catch (...) {
					COPQUEUE_LAPP_ << "Unkown exception";
				}
				
				//if weg got a listener notify it
				if(eventListener && !(*eventListener).elementWillBeDiscarded(tag, dataRow))continue;
				
				DELETE_OBJ_POINTER(dataRow);
			}
			DEBUG_CODE(COPQUEUE_LDBG_<< "executeOnThread ended";)
		}
		
		/*
		 Process the oldest element in buffer
		 */
		virtual void processBufferElement(T*, ElementManagingPolicy&) throw(CException) = 0;
		
	public:
		int tag;
		
		CObjectProcessingQueue():
		in_deinit(false),
		eventListener(NULL),
		uid(common::utility::UUIDUtil::generateUUIDLite()){}
		
		CObjectProcessingQueue(CObjectProcessingQueueListener<T> *_eventListener):
		in_deinit(false),
		eventListener(_eventListener),
		uid(common::utility::UUIDUtil::generateUUIDLite()){}
		
		/*
		 Set the internal thread delay for execute new task
		 
		 void setDelayBeetwenTask(long threadDelay){
		 threadGroup.setDelayBeetwenTask(threadDelay);
		 } */
		
		/*
		 Initialization method for output buffer
		 */
		virtual void init(int threadNumber) throw(CException) {
			in_deinit = false;
			COPQUEUE_LDBG_ << "init";
			//add the n thread on the threadgroup
			COPQUEUE_LDBG_ << "creating and starting" << threadNumber << " thread";
			for (int idx = 0; idx<threadNumber; idx++) {
				t_group.create_thread(boost::bind(&CObjectProcessingQueue<T>::executeOnThread, this));
			}
			
			COPQUEUE_LDBG_ << "Initialized";
			}
			
			/*
			 Deinitialization method for output buffer
			 */
			virtual void deinit(bool waithForEmptyQueue=true) throw(CException) {
				boost::unique_lock<boost::mutex> lock(qMutex);
				COPQUEUE_LDBG_ << "Deinitialization";
				
				if(waithForEmptyQueue){
					COPQUEUE_LDBG_ << " wait until queue is empty";
					while( !bufferQueue.empty()){
						emptyQueueConditionLock.timed_wait(lock,
														   boost::posix_time::milliseconds(500));
						
					}
					COPQUEUE_LDBG_ << "queue is empty";
				}
				COPQUEUE_LDBG_ << "Stopping thread";
				in_deinit = true;
				liveThreadConditionLock.notify_all();
                try {
                   if(lock.owns_lock()) lock.unlock();
                }catch(...) {
                    COPQUEUE_LERR_ << "Error unlocking";
                }
				COPQUEUE_LDBG_ << "join internal thread group";
				t_group.join_all();
				COPQUEUE_LDBG_ << "deinitlized";
			}
			
			/*
			 push the row value into the buffer
			 */
			virtual bool push(T* data) throw(CException) {
				boost::unique_lock<boost::mutex> lock(qMutex);
				if(in_deinit ||
				   bufferQueue.size() > CObjectProcessingQueue_MAX_ELEMENT_IN_QUEUE) return false;
				bufferQueue.push(data);
				liveThreadConditionLock.notify_one();
				return true;
			}
			
			/*
			 get the last insert data
			 */
			T* waitAndPop() {
				DEBUG_CODE(COPQUEUE_LDBG_<< "Entering in waitAndPop";)
				boost::unique_lock<boost::mutex> lock(qMutex);
				//output result poitner
				T *oldestElement = NULL;
				//DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() begin to wait";)
				while(bufferQueue.empty() && !in_deinit) {
					liveThreadConditionLock.wait(lock);
				}
				//DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() wakeup";)
				//get the oldest data ad copy the ahsred_ptr
				if(bufferQueue.empty()) {
					DEBUG_CODE(COPQUEUE_LDBG_<< "bufferQueue.empty() is empty so we go out";)
					return NULL;
				}
				//get the last pointer from the queue
				oldestElement = bufferQueue.front();
				
				//remove the oldest data
				bufferQueue.pop();
				DEBUG_CODE(COPQUEUE_LDBG_<< "Leaving in waitAndPop";)
				return oldestElement;
			}
			
			/*
			 check for empty buffer
			 */
			bool isEmpty() const {
				boost::unique_lock<boost::mutex> lock(qMutex);
				return bufferQueue.empty();
			}
			
			/*
			 check for empty buffer
			 */
			void waitForEmpty() {
				boost::unique_lock<boost::mutex> lock(qMutex);
				while(!bufferQueue.empty()){
					emptyQueueConditionLock.timed_wait(lock,
													   boost::posix_time::milliseconds(500));
				}
				return bufferQueue.empty();
			}
			
			
			/*
			 clear the queue, remove all non processed element
			 */
			void clear() {
				CHAOS_BOOST_LOCK_ERR(boost::unique_lock<boost::mutex> lock(qMutex);, COPQUEUE_LERR_ << "Error on lock";)
				
				//remove all element
				while (!bufferQueue.empty()) {
					T* to_delete = bufferQueue.front();
					bufferQueue.pop();
					CHK_AND_DELETE_OBJ_POINTER(to_delete)
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
#endif
			
