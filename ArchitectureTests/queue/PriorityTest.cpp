/*
 *	PriorityTest.cpp
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

#include "PriorityTest.h"

using namespace chaos::common::pqueue::test;

PriorityTest::PriorityTest() {
    
}

PriorityTest::~PriorityTest() {
    
}
bool PriorityTest::test(int _max_priority,
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