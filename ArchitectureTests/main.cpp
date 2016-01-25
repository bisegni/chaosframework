//
//  main.cpp
//  EchitectureTests
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "utility/HashMapTest.h"
#include "network/FeederTest.h"
#include "thread/ObjectQueueTest.h"
#include "thread/ObjectPriorityQueueTest.h"
#include <cassert>
#include <queue>
#include <vector>
#include <string>
#include <chaos/common/property/PropertyCollector.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

using namespace chaos;

typedef struct TestElement {
    std::string element_value;
    TestElement(const std::string& _element_value):
    element_value(_element_value){}
}TestElement;

// pulic class used into the sandbox for use the priority set into the lement that are pointer and not rela reference
struct TestElementCompare {
    bool operator() (const PRIORITY_ELEMENT(TestElement)* lhs, const PRIORITY_ELEMENT(TestElement)* rhs) const {
        if(lhs->priority < rhs->priority) {
            return true;
        } else if(lhs->priority == rhs->priority) {
            return  lhs->sequence_id >= rhs->sequence_id;
        } else {
            return false;
        }
    }
};

std::priority_queue<PRIORITY_ELEMENT(TestElement)*,
std::vector<PRIORITY_ELEMENT(TestElement)*>,
TestElementCompare > command_submitted_queue;

PRIORITY_ELEMENT(TestElement) *current_element = NULL;

class TestProp:
public chaos::common::property::PropertyCollector {
    
    void setPeropertyOne(int _new_property_one) {
        property_one = _new_property_one;
    }
    int getPeropertyOne() {
        return property_one;
    }
public:
    DECLARE_CHAOS_PROPERTY(TestProp, int, CHAOS_PROPERTY_READ_WRITE, property_one)

    TestProp() {
        REGISTER_CHAOS_PROPERTY("TestProp", property_one, &TestProp::setPeropertyOne, &TestProp::getPeropertyOne, "Get the property one")
    }
};

int main(int argc, const char * argv[]) {
    
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("a"), 50, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("b"), 50, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("c"), 50, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("_a"), 20, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("_b"), 20, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("_c"), 20, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("_cdfasdf"), 20, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("_zzzzzzz"), 20, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("cdfasdf"), 50, true));
    command_submitted_queue.push(new PriorityQueuedElement<TestElement>(new TestElement("zzzzzzz"), 50, true));
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);

    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    current_element = command_submitted_queue.top(); command_submitted_queue.pop();
    std::cout << current_element->element->element_value << "[" << current_element->sequence_id << "]" << std::endl;
    delete(current_element);
    
    TestProp tp;
    tp.property_one = 34;
    int i = tp.property_one;
    std::string i_str =  tp.property_one;
    i_str = tp.getSectionPropertyStrValue("TestProp", "property_one");
    tp.setSectionPropertyStrValue("TestProp",
                          "property_one",
                          "100");
    i = tp.property_one;
    i_str = tp.getSectionPropertyStrValue("TestProp", "property_one");

    tp.property_one = "1500";
    i = tp.property_one;
    
    chaos::test::network::FeederTest fd;
	fd.test(100000);
    
    chaos::common::pqueue::test::ObjectQueueTest oqt;
    assert((oqt.test(10, 100, 100, 0, true) == true));
	
	chaos::common::pqueue::test::ObjectPriorityQueueTest opqt;
	assert((oqt.test(10, 100, 100, 0, true) == true));

	chaos::common::utility::test::HashMapTest hmt;
	assert(hmt.test(10, 10, 10));
    
    return 0;
}

