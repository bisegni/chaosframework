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
