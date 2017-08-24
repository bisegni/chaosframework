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

#ifndef __CHAOSFrameworkTests_EEF7605F_524D_404E_B146_DA17DD3C2AAF_TestObjectProcessingQueue_h
#define __CHAOSFrameworkTests_EEF7605F_524D_404E_B146_DA17DD3C2AAF_TestObjectProcessingQueue_h

#include <chaos/common/pqueue/CObjectProcessingQueue.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace pqueue {
            namespace test {
                
                typedef struct TestJobStruct {
                    boost::atomic<int> check_concurence;
                } TestJobStruct;
                
                //! thest for the chaos::common::pqueue::CObjectProcessingQueue
                class TestObjectProcessingQueue:
                protected  chaos::CObjectProcessingQueue<TestJobStruct> {
                    int number_of_consumer;
                    int number_of_producer;
                    int number_of_production;
                    boost::atomic<int> job_to_do;
                    boost::atomic<int> error_in_job;
                    boost::thread_group tg;
                    void processBufferElement(TestJobStruct *job,
                                              ElementManagingPolicy& p) throw(CException);
                    
                    void producer();
                public:
                    TestObjectProcessingQueue();
                    virtual~ TestObjectProcessingQueue();
                    bool test(int _number_of_producer,
                              int _number_of_production,
                              int _number_of_consumer,
                              unsigned int _sec_to_whait = 0,
                              bool _whait_to_finish = true);
                };
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests_EEF7605F_524D_404E_B146_DA17DD3C2AAF_TestObjectProcessingQueue_h */
