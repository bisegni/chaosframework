/*
 *	TestObjectProcessingPriorityQueueForPriority.cpp
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

#include "TestObjectProcessingPriorityQueueForPriority.h"

using namespace chaos::common::pqueue::test;

TestObjectProcessingPriorityQueueForPriority::TestObjectProcessingPriorityQueueForPriority():
last_sequence(0),
last_priority(100),
max_priority(0),
number_of_production(0){}

TestObjectProcessingPriorityQueueForPriority::~TestObjectProcessingPriorityQueueForPriority(){}

void TestObjectProcessingPriorityQueueForPriority::processBufferElement(TestPriorityElement *element,
                                                                        ElementManagingPolicy& p) throw(CException) {
    if(last_priority != element->priority) {
        assert(last_priority>=element->priority);
        last_priority = element->priority;
        last_sequence = 0;
    }
    if(last_sequence != element->id) {
        assert(last_sequence<element->id);
        last_sequence = element->id;
    }
}

void TestObjectProcessingPriorityQueueForPriority::init(int threadNumber) throw(CException) {
    
    //fill queue initlize to test priority for all element
    int priority = 0;
    for(int idx = 0; idx < number_of_production; idx++) {
        priority = std::rand()%max_priority+1;
        push(new TestPriorityElement(chaos::common::utility::UUIDUtil::generateUUID(), idx, priority), priority, true);
    }
    
    CObjectProcessingPriorityQueue<TestPriorityElement>::init(threadNumber);
}

bool TestObjectProcessingPriorityQueueForPriority::test(int _max_priority,
                                                        int _number_of_production) {
    max_priority = _max_priority;
    number_of_production =_number_of_production;
    init(1);
    deinit(true);
    return true;
}
