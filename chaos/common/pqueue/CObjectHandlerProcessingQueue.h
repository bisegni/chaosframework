/*	
 *	CObjectHandlerProcessingQueue.h
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
#ifndef CHAOSFramework_CObjectHandlerProcessingQueue_h
#define CHAOSFramework_CObjectHandlerProcessingQueue_h



#include "CObjectProcessingQueueListener.h"
#include <chaos/common/thread/CThreadExecutionTask.h>
#include <chaos/common/thread/CThreadGroup.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/global.h>
#include <chaos/common/action/ActionDescriptor.h>

#include <queue>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    
    /*!
     Base class for the Output Buffer structure
     */
    template<typename T, typename _heapEngine = queue<T*> >
    class CObjectHandlerProcessingQueue : public CThreadExecutionTask {
        bool inDeinit;
        int outputThreadNumber;
        mutable boost::mutex qMutex;
        _heapEngine bufferQueue;
        boost::condition_variable liveThreadConditionLock;
        boost::condition_variable emptyQueueConditionLock;
        boost::function<CDataWrapper*(CDataWrapper*)> handler;
            //thread group
        CThreadGroup threadGroup;
        
    protected:
        
        /*!
         
         */
        void setHandler(boost::function<CDataWrapper*(CDataWrapper*, bool)> _handler){
            handler = _handler;
        }
        
        /*!
         Thread method that work on buffer item
         */
        void executeOnThread() throw(CException) {
            bool detachData = false;
                //get the oldest element
            T* dataRow = NULL;
                //retrive the oldest element
            dataRow = waitAndPop();
            if(!dataRow) return;
                //Process the element
            try {
                

                if(dataRow && !handler.empty()) handler(dataRow, detachData);
                if(detachData) return;
            } catch (CException& ex) {
                DECODE_CHAOS_EXCEPTION(ex)
            } catch (...) {
                LAPP_ << "[CObjectProcessingQueue] Unkown exception";
            } 
            
            
            DELETE_OBJ_POINTER(dataRow);
        }
        
    public:
        int tag;
        
        CObjectHandlerProcessingQueue(){
            inDeinit = false;
        } 
        
        
        /*
         Set the internal thread delay for execute new task
         */
        void setDelayBeetwenTask(long threadDelay){
            threadGroup.setDelayBeetwenTask(threadDelay);
        }
        
        /*
         Initialization method for output buffer
         */
        virtual void init(int threadNumber) throw(CException) {
            inDeinit = false;
            LAPP_ << "CObjectProcessingQueue init";        
                //add the n thread on the threadgroup
            LAPP_ << "CObjectProcessingQueue creating " << threadNumber << " thread";
            for (int idx = 0; idx<threadNumber; idx++) {
                threadGroup.addThread(new CThread(this));
            }      
            
            LAPP_ << "CObjectProcessingQueue Starting all thread";
            threadGroup.startGroup();
            LAPP_ << "CObjectProcessingQueue Initialized";
        }
        
        /*
         Deinitialization method for output buffer
         */
        virtual void deinit(bool waithForEmptyQueue=true) throw(CException) {
            boost::mutex::scoped_lock lock(qMutex);
            inDeinit = true;
            LAPP_ << "CObjectProcessingQueue Deinitialization";
                //stopping the group
            LAPP_ << "CObjectProcessingQueue Deinitializing Threads";
            
            if(waithForEmptyQueue){
                LAPP_ << "CObjectProcessingQueue wait until queue is empty";
                while( !bufferQueue.empty()){
                    emptyQueueConditionLock.wait(lock);
                }
                LAPP_ << "CObjectProcessingQueue queue is empty";
            }
            
            LAPP_ << "CObjectProcessingQueue Stopping thread";
            threadGroup.stopGroup(false);
            lock.unlock();
            
            liveThreadConditionLock.notify_all();
            LAPP_ << "CObjectProcessingQueue join internal thread group";
            threadGroup.joinGroup();
            LAPP_ << "CObjectProcessingQueue deinitlized";
        }
        
        /*
         push the row value into the buffer
         */
        virtual bool push(T* data) throw(CException) {
            boost::mutex::scoped_lock lock(qMutex);
            if(inDeinit) return true;
            bufferQueue.push(data);
            lock.unlock();
            liveThreadConditionLock.notify_one();
            return false;
        }
        
        /*
         get the last insert data
         */
        T* waitAndPop() {
            boost::mutex::scoped_lock lock(qMutex);
                //output result poitner
            T *oldestElement = NULL;
            
            while(bufferQueue.empty() && !threadGroup.isStopped()) {
                emptyQueueConditionLock.notify_one();
                liveThreadConditionLock.wait(lock);
            }
                //get the oldest data ad copy the ahsred_ptr
            if(bufferQueue.empty()) {
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
         Whait the thread termination
         */
        void joinBufferThread() {
            threadGroup.joinGroup();
        }
    };
}
#endif
