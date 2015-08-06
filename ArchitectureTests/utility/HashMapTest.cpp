/*
 *	HashMapTest.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "HashMapTest.h"

#include <cassert>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::utility::test;

void HashMapTest::producer() {
	for(int idx = 0;
		idx < number_of_production;
		idx++){
		HashMapTestElement *element = (HashMapTestElement*)malloc(sizeof(HashMapTestElement));
		element->product_id = production_id++;
		
		std::string element_key = std::string("element_") + boost::lexical_cast<std::string>(element->product_id);
		addElement(element_key.c_str(), (uint32_t)element_key.size(), element);
	}
}

void HashMapTest::consumer() {
	HashMapTestElement *element_found = NULL;
	for(int idx = 0;
		idx < production_id;
		idx++){
		
		std::string element_key = std::string("element_") + boost::lexical_cast<std::string>(idx);
		
		if(getElement(element_key.c_str(),
					  (uint32_t)element_key.size(),
					  &element_found)==0) {
			//ok
			assert(element_found);
			assert(element_found->product_id == idx);
			
			//sum the product id
			product_id_sum += element_found->product_id;
		} else {
			error_count++;
		}
	}
}

void HashMapTest::clearHashTableElement(const void *key,
										uint32_t key_len,
										HashMapTestElement *element) {
	assert(element);
	free(element);
}

int HashMapTest::sumOfNumbersUptTo(int num) {
	unsigned int result = 0;
	for(int idx = 0;
		idx < num;
		idx++) {
		result += idx;
	}
	return result;
}

bool HashMapTest::test(int _number_of_producer,
					   int _number_of_production,
					   int _number_of_consumer) {
	number_of_producer		= _number_of_producer;
	number_of_production	= _number_of_production;
	number_of_consumer		= _number_of_consumer;
	production_id = 0;
	product_id_sum = 0;
	//init the producer
	for(int idx = 0;
		idx < number_of_producer;
		idx++) {
		producer_thread_group.add_thread(new boost::thread(boost::bind(&HashMapTest::producer,
																	   this)));
	}
	
	//waith that all insert has been done
	producer_thread_group.join_all();
	assert(production_id == (_number_of_producer * _number_of_production));
	
	//init the consumer
	for(int idx = 0;
		idx < number_of_consumer;
		idx++) {
		consumer_thread_group.add_thread(new boost::thread(boost::bind(&HashMapTest::consumer,
																	   this)));
	}
	
	//waith that all insert has been done
	consumer_thread_group.join_all();

	assert((number_of_consumer * sumOfNumbersUptTo(_number_of_producer * _number_of_production)) == product_id_sum);
	
	clear();
	
	return true;
}