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
typedef struct TestJobPriorityStruct {
    int priority;
    boost::atomic<int> check_concurence;
} TestJobPriorityStruct;

//! thest for the chaos::common::pqueue::CObjectProcessingQueue
class QueueTest:
public testing::Test,
protected  chaos::common::pqueue::CObjectProcessingPriorityQueue<TestJobPriorityStruct> {
protected:
    int number_of_consumer;
    int number_of_producer;
    int number_of_production;
    int second_to_wait;
    boost::atomic<int> job_to_do;
    boost::atomic<int> job_done;
    boost::atomic<int> error_in_job;
    boost::atomic<int> priority_counter;
    boost::thread_group tg;
    void processBufferElement(ChaosSharedPtr<TestJobPriorityStruct> job) throw(chaos::CException);
public:
    QueueTest();
    virtual ~QueueTest();
    virtual void SetUp();
    void producer();
};
