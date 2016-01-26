/*
 *	PriorityTest.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 25/01/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__PriorityTest_h
#define __CHAOSFramework__PriorityTest_h
#include <queue>
#include <vector>
#include <string>

#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

namespace chaos {
    namespace common {
        namespace pqueue {
            namespace test {
                
                typedef struct TestElement {
                    std::string element_value;
                    TestElement(const std::string& _element_value):
                    element_value(_element_value){}
                }TestElement;
                
                // pulic class used into the sandbox for use the priority set into the lement that are pointer and not rela reference
                struct TestElementCompare {
                    bool operator() (const PRIORITY_ELEMENT(TestElement)* lhs, const PRIORITY_ELEMENT(TestElement)* rhs) const {
                        if(lhs->priority < rhs->priority) {
                            return true;
                        } else if(lhs->priority == rhs->priority) {
                            return  lhs->sequence_id >= rhs->sequence_id;
                        } else {
                            return false;
                        }
                    }
                };
                
                class PriorityTest {
                    std::priority_queue<PRIORITY_ELEMENT(TestElement)*,
                    std::vector<PRIORITY_ELEMENT(TestElement)*>,
                    TestElementCompare > command_submitted_queue;
                    
                    PRIORITY_ELEMENT(TestElement) *current_element = NULL;
                public:
                    PriorityTest();
                    virtual ~PriorityTest();
                    bool test(int _max_priority,
                              int _number_of_production);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__PriorityTest_h */
