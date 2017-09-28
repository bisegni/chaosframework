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

#include "QueueTest.h"
#include <boost/thread.hpp>
QueueTest::QueueTest(){}
QueueTest::~QueueTest() {}

void QueueTest::SetUp() {
    number_of_consumer = 10;
    number_of_producer = 100;
    number_of_production = 100;
    second_to_wait = 5;
    job_to_do = 0;
    job_done = 0;
}
void QueueTest::processBufferElement(TestJobPriorityStruct *job,
                                     chaos::ElementManagingPolicy& p) throw(chaos::CException) {
    assert(job);
    job_done++;
    if((--job->check_concurence) != 0) {
        error_in_job++;
    }
}

void QueueTest::producer() {
    for(int idx = 0;idx < number_of_production; idx++) {
        TestJobPriorityStruct *job = new TestJobPriorityStruct();
        job->check_concurence = 1;
        job_to_do++;
        while(push(job, 0) == false){
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        }
    }
}

TEST_F(QueueTest, QueueTestNormal) {
    job_to_do = 0;
    error_in_job = 0;
    priority_counter = 0;
    //init and start the consumer thread
    init(number_of_consumer);
    
    //init the producer
    for(int idx = 0;idx < number_of_producer; idx++) {
        tg.add_thread(new boost::thread(boost::bind(&QueueTest::producer,
                                                    this)));
    }
    tg.join_all();
    deinit(true);
    //print statistic
    ASSERT_EQ(error_in_job, 0);
    ASSERT_EQ(job_to_do, job_done);
}
