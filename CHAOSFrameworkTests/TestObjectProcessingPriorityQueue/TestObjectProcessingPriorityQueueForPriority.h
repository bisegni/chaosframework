/*
 *	TestObjectProcessingPriorityQueueForPriority.h
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 22/08/16 INFN, National Institute of Nuclear Physics
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
