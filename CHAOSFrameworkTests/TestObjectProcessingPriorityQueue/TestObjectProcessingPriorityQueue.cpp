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

#include "TestObjectProcessingPriorityQueue.h"

#include <stdio.h>
#include <cassert>

using namespace chaos::common::pqueue::test;

TestObjectProcessingPriorityQueue::TestObjectProcessingPriorityQueue():
number_of_consumer(0),
number_of_producer(0),
number_of_production(0) {
}

TestObjectProcessingPriorityQueue::~TestObjectProcessingPriorityQueue() {
}

bool TestObjectProcessingPriorityQueue::test(int _number_of_producer,
                                             int _number_of_production,
                                             int _number_of_consumer,
                                             unsigned int _sec_to_whait,
                                             bool _whait_to_finish) {
    job_to_do = 0;
    error_in_job = 0;
    priority_counter = 0;
    //init and start the consumer thread
    init(number_of_consumer = _number_of_consumer);
    number_of_production = _number_of_production;
    number_of_producer = _number_of_producer;
    
    //init the producer
    for(int idx = 0;idx < number_of_producer; idx++) {
        tg.add_thread(new boost::thread(boost::bind(&TestObjectProcessingPriorityQueue::producer,
                                                    this)));
    }
    
    if(_sec_to_whait) {
        sleep(_sec_to_whait);
    }
    //waith the priducer has terminated to generate job
    tg.join_all();
    
    //now deinit the queue
    deinit(_whait_to_finish);
    
    //print statistic
    std::cout << "Error in job = " << error_in_job << std::endl;
    std::cout << "Job not done = " << job_to_do << std::endl;
    std::cout << "end whit element number = " << queueSize() << std::endl;
    return  (job_to_do == 0) &&
    (error_in_job == 0) &&
    (queueSize() == 0);
}

void TestObjectProcessingPriorityQueue::processBufferElement(TestJobPriorityStruct *job,
                                                             ElementManagingPolicy& p) throw(CException) {
    assert(job);
    job_to_do--;
    if((--job->check_concurence) != 0) {
        error_in_job++;
    }
}

void TestObjectProcessingPriorityQueue::producer() {
    for(int idx = 0;idx < number_of_production; idx++) {
        TestJobPriorityStruct *job = (TestJobPriorityStruct*)malloc(sizeof(TestJobPriorityStruct));
        job->check_concurence = 1;
        job_to_do++;
        push(job, (job->priority =  priority_counter++));
    }
}
