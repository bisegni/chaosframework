    //
    //  CObjectProcessingQueue.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 17/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef CObjectProcessingQueue_H
#define CObjectProcessingQueue_H

#include "CObjectProcessingQueueListener.h"
#include <chaos/common/thread/CThreadExecutionTask.h>
#include <chaos/common/thread/CThreadGroup.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/global.h>

#include <queue>
#include <boost/lexical_cast.hpp>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    typedef struct {
        bool elementHasBeenDetached;
    } ElementManagingPolicy;
    
    /*
        Base class for the Output Buffer structure
     */
    template<typename T, typename _heapEngine = queue<T*> >
    class CObjectProcessingQueue : public CThreadExecutionTask {
        bool inDeinit;
        int outputThreadNumber;
        mutable boost::mutex qMutex;
        _heapEngine bufferQueue;
        condition_variable liveThreadConditionLock;
        condition_variable emptyQueueConditionLock;
        
            //thread group
        CThreadGroup threadGroup;
        
    protected:
        CObjectProcessingQueueListener<T> *eventListener;
        
        /*
         Thread method that work on buffer item
         */
        void executeOnThread(const string& threadIdentification) throw(CException) {
                //get the oldest element
            T* dataRow = NULL;
            ElementManagingPolicy elementPolicy;
                //retrive the oldest element
            dataRow = waitAndPop();
            if(!dataRow) return;
                //Process the element
            try {
                if(eventListener && !(*eventListener).elementWillBeProcessed(tag, dataRow)){
                    DELETE_OBJ_POINTER(dataRow);
                    return;
                }
                elementPolicy.elementHasBeenDetached=false;
                if(dataRow) processBufferElement(dataRow, elementPolicy);
                if(elementPolicy.elementHasBeenDetached) return;
            } catch (CException& ex) {
                DECODE_CHAOS_EXCEPTION(ex)
            } catch (...) {
                LAPP_ << "[CObjectProcessingQueue] Unkown exception";
            } 
            
                //if weg got a listener notify it
            if(eventListener && !(*eventListener).elementWillBeDiscarded(tag, dataRow))return;
            
            DELETE_OBJ_POINTER(dataRow);
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
          Assign the elaboration Listener
         */
        void setEndElaborationListener(CObjectProcessingQueueListener<T> *objPtr) {
            eventListener = objPtr; 
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

