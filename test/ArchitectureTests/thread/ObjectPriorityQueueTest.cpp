/*
 *	ObjectPriorityQueueTest.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <stdio.h>
#include <cassert>
#include "ObjectPriorityQueueTest.h"

using namespace chaos::common::pqueue::test;

ObjectPriorityQueueTest::ObjectPriorityQueueTest():
number_of_consumer(0),
number_of_producer(0),
number_of_production(0) {
}

ObjectPriorityQueueTest::~ObjectPriorityQueueTest() {
}

bool ObjectPriorityQueueTest::testPriority(int _max_priority,
                        int _number_of_production) {
    std::srand(std::time(0));
    std::cout << "Generate "<< _number_of_production <<" number of iteration of sequences of data with rando priority of max value "<<_max_priority<<"--------------------------" << std::endl;
    for(int idx = 0; idx < _number_of_production; idx++) {
        command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement(chaos::common::utility::UUIDUtil::generateUUID()), idx, std::rand()%_max_priority+1, true));
    }
    uint64_t last_sequence = 0;
    int last_priority = 100;
    std::cout << "Checking ------------------------------------------------------------------------" << _max_priority << std::endl;
    while (command_submitted_queue.size()) {
        current_element = command_submitted_queue.top();
        command_submitted_queue.pop();
        if(last_priority != current_element->priority) {
            if(last_priority<current_element->priority) {
                std::cerr << "Bad priority sequence"<< std::endl;
                return false;
            }
            last_priority = current_element->priority;
            last_sequence = 0;
        }
        if(last_sequence != current_element->sequence_id) {
            if(last_sequence>current_element->sequence_id) {
                std::cerr << "Bad sequence order"<< std::endl;
                return false;
            }
            last_sequence = current_element->sequence_id;
        }
        std::cout << current_element->element->element_value << "[P:" << current_element->priority << " seq:"<< current_element->sequence_id << "]" << std::endl;
        delete(current_element);
    }
    std::cout << "No error during Checking ------------------------------------------------------------------------" << _max_priority << std::endl;
    return true;
}

bool ObjectPriorityQueueTest::testConcurency(int _number_of_producer,
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
		tg.add_thread(new boost::thread(boost::bind(&ObjectPriorityQueueTest::producer,
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

void ObjectPriorityQueueTest::processBufferElement(TestJobPriorityStruct *job,
												   ElementManagingPolicy& p) throw(CException) {
	assert(job);
	job_to_do--;
	if((--job->check_concurence) != 0) {
		error_in_job++;
	}
}

void ObjectPriorityQueueTest::producer() {
	for(int idx = 0;idx < number_of_production; idx++) {
		TestJobPriorityStruct *job = (TestJobPriorityStruct*)malloc(sizeof(TestJobPriorityStruct));
		job->check_concurence = 1;
		job_to_do++;
		push(job, (job->priority =  priority_counter++));
	}
}
