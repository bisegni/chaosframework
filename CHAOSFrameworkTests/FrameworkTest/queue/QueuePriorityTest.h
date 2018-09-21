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

#include <gtest/gtest.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
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

class QueuePriorityTest:
public testing::Test,
public  chaos::common::pqueue::CObjectProcessingPriorityQueue<TestPriorityElement> {
    void processBufferElement(ChaosSharedPtr<TestPriorityElement> job) throw(chaos::CException);
protected:
    uint64_t last_sequence;
    int last_priority;
    int max_priority;
    int number_of_production;
public:
    QueuePriorityTest();
    virtual ~QueuePriorityTest();
    
    virtual void SetUp();
};
