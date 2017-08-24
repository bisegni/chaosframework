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

#ifndef __CHAOSFrameworkTests__2FFCAB2_CA85_4A0C_9816_68F0ED3F2CA8_TestObjectProcessingPriorityQueueForPriority_h
#define __CHAOSFrameworkTests__2FFCAB2_CA85_4A0C_9816_68F0ED3F2CA8_TestObjectProcessingPriorityQueueForPriority_h


#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace pqueue {
            namespace test {
                
                typedef struct TestPriorityElement {
                    std::string element_value;
                    uint64_t id;
                    int priority;
                    TestPriorityElement(const std::string& _element_value,
                                        uint64_t _id,
                                        int _priority):
                    element_value(_element_value),
                    id(_id),
                    priority(_priority){}
                } TestPriorityElement;
                
                
                //! thest for the chaos::common::pqueue::CObjectProcessingQueue
                class TestObjectProcessingPriorityQueueForPriority:
                protected  CObjectProcessingPriorityQueue<TestPriorityElement> {
                    uint64_t last_sequence;
                    int last_priority;
                    int max_priority;
                    int number_of_production;
                    void processBufferElement(TestPriorityElement *job,
                                              ElementManagingPolicy& p) throw(CException);
                    
                    void init(int threadNumber) throw(CException);
                    
                public:
                    TestObjectProcessingPriorityQueueForPriority();
                    virtual ~TestObjectProcessingPriorityQueueForPriority();
                    bool test(int _max_priority,
                              int _number_of_production);
                };
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests__2FFCAB2_CA85_4A0C_9816_68F0ED3F2CA8_TestObjectProcessingPriorityQueueForPriority_h */
