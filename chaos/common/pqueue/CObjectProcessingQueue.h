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
#ifndef CObjectProcessingQueue_H
#define CObjectProcessingQueue_H

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/exception/exception.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <queue>

#define LOG_HEAD "[CObjectProcessingQueue] - "
#define COPQUEUE_LAPP_ LAPP_ << LOG_HEAD
#define COPQUEUE_LDBG_ LDBG_ << LOG_HEAD
#define COPQUEUE_LERR_ LERR_ << LOG_HEAD << "(" << __LINE__ << ")"

#define CObjectProcessingQueue_MAX_ELEMENT_IN_QUEUE 1000
namespace chaos {
    
    typedef struct ElementManagingPolicy {
        bool elementHasBeenDetached;
    } ElementManagingPolicy;
    
    /*
     Base class for the Output Buffer structure
     */
    template<typename T>
    class CObjectProcessingQueue {
    public:
        typedef ChaosSharedPtr<T> QueueElementShrdPtr;
    private:
        const std::string uuid;
        //thread group
        //CThreadGroup threadGroup;
        boost::thread_group t_group;
    protected:
        bool in_deinit;
        mutable boost::mutex qMutex;
        std::queue< QueueElementShrdPtr > buffer_queue;
        boost::condition_variable liveThreadConditionLock;
        boost::condition_variable emptyQueueConditionLock;
        
        /*
         Thread method that work on buffer item
         */
        void executeOnThread() {
            //get the oldest element
            while(!in_deinit) {
                //Process the element
                try {
                    QueueElementShrdPtr element = MOVE(waitAndPop());
                    if(!element.get()) {
                        continue;
                    }
                    processBufferElement(MOVE(element));
                } catch (CException& ex) {
                    DECODE_CHAOS_EXCEPTION(ex)
                } catch (...) {
                    COPQUEUE_LAPP_ << "Unkown exception";
                }
            }
            DEBUG_CODE(COPQUEUE_LDBG_<< "executeOnThread ended";)
        }
        
        /*
         Process the oldest element in buffer
         */
        virtual void processBufferElement(QueueElementShrdPtr element)  = 0;
        
    public:
        CObjectProcessingQueue():
        in_deinit(false),
        uuid(common::utility::UUIDUtil::generateUUIDLite()){}
        
        const std::string& getUUID() const{
            return uuid;
        }
        /*
         Initialization method for output buffer
         */
        virtual void init(int threadNumber) {
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
            virtual void deinit(bool waithForEmptyQueue=true) {
                boost::unique_lock<boost::mutex> lock(qMutex);
                COPQUEUE_LDBG_ << "Deinitialization";
                
                if(waithForEmptyQueue){
                    COPQUEUE_LDBG_ << " wait until queue is empty";
                    while(!buffer_queue.empty()){
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
                COPQUEUE_LDBG_ << "deinitialized";
            }
            
            /*
             push the row value into the buffer
             */
            virtual bool push(QueueElementShrdPtr data) {
                boost::unique_lock<boost::mutex> lock(qMutex);
                if(in_deinit ||
                   buffer_queue.size() > CObjectProcessingQueue_MAX_ELEMENT_IN_QUEUE) return false;
                buffer_queue.push(MOVE(data));
                liveThreadConditionLock.notify_one();
                return true;
            }
            
            /*
             get the last insert data
             */
            QueueElementShrdPtr waitAndPop() {
                boost::unique_lock<boost::mutex> lock(qMutex);
                //output result poitner
                QueueElementShrdPtr element;
                //DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() begin to wait";)
                while(buffer_queue.empty() && !in_deinit) {
                    liveThreadConditionLock.wait(lock);
                }
                //DEBUG_CODE(COPQUEUE_LDBG_<< " waitAndPop() wakeup";)
                //get the oldest data ad copy the ahsred_ptr
                if(buffer_queue.empty()) {
                    DEBUG_CODE(COPQUEUE_LDBG_<< "bufferQueue.empty() is empty so we go out";)
                    return QueueElementShrdPtr();
                }
                //get the last pointer from the queue
                element = buffer_queue.front();
                
                //remove the oldest data
                buffer_queue.pop();
                return element;
            }
            
            /*
             check for empty buffer
             */
            bool isEmpty() const {
                boost::unique_lock<boost::mutex> lock(qMutex);
                return buffer_queue.empty();
            }
            
            /*
             check for empty buffer
             */
            void waitForEmpty() {
                boost::unique_lock<boost::mutex> lock(qMutex);
                while(!buffer_queue.empty()){
                    emptyQueueConditionLock.timed_wait(lock,
                                                       boost::posix_time::milliseconds(500));
                }
                return buffer_queue.empty();
            }
            
            
            /*
             clear the queue, remove all non processed element
             */
            void clear() {
                CHAOS_BOOST_LOCK_ERR(boost::unique_lock<boost::mutex> lock(qMutex);, COPQUEUE_LERR_ << "Error on lock";)
                //remove all element
                while (!buffer_queue.empty()) {
                    //QueueElementShrdPtr tmp = MOVE(bufferQueue.front());
                    buffer_queue.pop();
                }
            }
            
            /*
             Return le number of elementi into live data
             */
            unsigned long queueSize() {
                boost::unique_lock<boost::mutex> lock(qMutex);
                return buffer_queue.size();
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
            
