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
                virtual ~AsyncRunnable();
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
                virtual void processBufferElement(ChaosSharedPtr<AsyncRunnable> next_job) ;
            public:
                AsyncPoolRunner(unsigned int _thread_number);
                ~AsyncPoolRunner();
                
                void init(void *init_data) ;
                void deinit() ;
                
                //! submit a new runnable
                /*!
                 the ownership on runnable is kept by the runner that
                 will provi to delete the object
                 */
                void submit(ChaosUniquePtr<AsyncRunnable> runnable);
            };
        }
    }
}

#endif /* __CHAOSFramework_ECDB2C62_2009_4CA5_A8A7_887D35BCA27A_AsyncPoolRunner_h */
