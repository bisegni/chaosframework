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

#include "QueuePriorityTest.h"

QueuePriorityTest::QueuePriorityTest():
last_sequence(0),
last_priority(0),
max_priority(0),
number_of_production(0){}
QueuePriorityTest::~QueuePriorityTest(){}

void QueuePriorityTest::processBufferElement(ChaosSharedPtr<TestPriorityElement> element) throw(chaos::CException) {
    if(last_priority != element->priority) {
        ASSERT_EQ(last_priority>=element->priority, true);
        last_priority = element->priority;
        last_sequence = 0;
    }
    if(last_sequence != element->id) {
        ASSERT_EQ(last_sequence<element->id, true);
        last_sequence = element->id;
    }
}

void QueuePriorityTest::SetUp() {
    last_sequence = 0;
    last_priority = 100;
    max_priority = 50;
    number_of_production = 1000;
}

TEST_F(QueuePriorityTest, QueueTestPriority) {
    //fill queue initlize to test priority for all element
    int priority = 0;
    for(int idx = 0; idx < number_of_production; idx++) {
        //generating random priority
        priority = std::rand()%max_priority+1;
        ChaosSharedPtr<TestPriorityElement> element = ChaosMakeSharedPtr<TestPriorityElement>(chaos::common::utility::UUIDUtil::generateUUID(), idx, priority);
        push(element, priority);
    }
    init(1);
    deinit(true);
}
