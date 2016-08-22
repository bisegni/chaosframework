/*
 *	TestObjectProcessingQueue.cpp
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

#include "TestObjectProcessingQueue.h"

#include <stdio.h>
#include <cassert>

using namespace chaos::common::pqueue::test;
TestObjectProcessingQueue::TestObjectProcessingQueue():
number_of_consumer(0),
number_of_producer(0),
number_of_production(0) {
}

TestObjectProcessingQueue::~TestObjectProcessingQueue() {
}

bool TestObjectProcessingQueue::test(int _number_of_producer,
                           int _number_of_production,
                           int _number_of_consumer,
                           unsigned int _sec_to_whait,
                           bool _whait_to_finish) {
    job_to_do = 0;
    error_in_job = 0;
    //init and start the consumer thread
    init(number_of_consumer = _number_of_consumer);
    number_of_production = _number_of_production;
    number_of_producer = _number_of_producer;
    
    //init the producer
    for(int idx = 0;idx < number_of_producer; idx++) {
        tg.add_thread(new boost::thread(boost::bind(&TestObjectProcessingQueue::producer,
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

void TestObjectProcessingQueue::processBufferElement(TestJobStruct *job,
                                           ElementManagingPolicy& p) throw(CException) {
    assert(job);
    job_to_do--;
    if((--job->check_concurence) != 0) {
        error_in_job++;
    }
}

void TestObjectProcessingQueue::producer() {
    for(int idx = 0;idx < number_of_production; idx++) {
        TestJobStruct *job = (TestJobStruct*)malloc(sizeof(TestJobStruct));
        job->check_concurence = 1;
        job_to_do++;
        push(job);
    }
}
