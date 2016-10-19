/*
 *	AsyncPoolRunner.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/10/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_ECDB2C62_2009_4CA5_A8A7_887D35BCA27A_AsyncPoolRunner_h
#define __CHAOSFramework_ECDB2C62_2009_4CA5_A8A7_887D35BCA27A_AsyncPoolRunner_h

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

namespace chaos {
    namespace common {
        namespace async_central {
            
            class AsyncPoolRunner;
            
            class AsyncRunnable {
                friend class AsyncPoolRunner;
                //!description of the job to be executed
                const std::string job_desc;
            protected:
                virtual void run() = 0;
            public:
                AsyncRunnable(const std::string& _job_desc);
                ~AsyncRunnable();
            };
            
            typedef chaos::common::pqueue::CObjectProcessingPriorityQueue<AsyncRunnable> PriorityQueueRunnable;
            
            //!runnable root class
            /*!
             contain a queue of runnable class and a poll of thread that
             execute that instance
             */
            class AsyncPoolRunner:
            protected PriorityQueueRunnable,
            public utility::InizializableService {
            protected:
                unsigned int thread_number;
                //!inherited by CObjectProcessingPriorityQueue
                virtual void processBufferElement(AsyncRunnable *next_job,
                                                  ElementManagingPolicy& element_policy) throw(chaos::CException);
            public:
                AsyncPoolRunner(unsigned int _thread_number);
                ~AsyncPoolRunner();
                
                void init(void *init_data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                
                //! submit a new runnable
                /*!
                 the ownership on runnable is kept by the runner that
                 will provi to delete the object
                 */
                void submit(AsyncRunnable *runnable);
            };
        }
    }
}

#endif /* __CHAOSFramework_ECDB2C62_2009_4CA5_A8A7_887D35BCA27A_AsyncPoolRunner_h */
