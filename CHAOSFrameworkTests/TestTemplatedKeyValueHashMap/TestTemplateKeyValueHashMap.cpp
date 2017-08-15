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

#include "TestTemplateKeyValueHashMap.h"

#include <cassert>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::utility::test;

void TestTemplateKeyValueHashMap::producer() {
    for(int idx = 0;
        idx < number_of_production;
        idx++) {
        TestTemplateKeyValueHashMapElement *element = (TestTemplateKeyValueHashMapElement *)malloc(sizeof(TestTemplateKeyValueHashMapElement));
        element->product_id = production_id++;
        
        std::string element_key = std::string("element_") + boost::lexical_cast<std::string>(element->product_id);
        addElement(element_key.c_str(), (uint32_t)element_key.size(), element);
    }
}

void TestTemplateKeyValueHashMap::consumer() {
    TestTemplateKeyValueHashMapElement *element_found = NULL;
    for(int            idx            = 0;
        idx < production_id;
        idx++) {
        
        std::string element_key = std::string("element_") + boost::lexical_cast<std::string>(idx);
        
        if(getElement(element_key.c_str(),
                      (uint32_t)element_key.size(),
                      &element_found) == 0){
            //ok
            assert(element_found);
            assert(element_found->product_id == idx);
            
            //sum the product id
            product_id_sum += element_found->product_id;
        }
        else {
            error_count++;
        }
    }
}

void TestTemplateKeyValueHashMap::clearHashTableElement(const void *key,
                                                        uint32_t key_len,
                                                        TestTemplateKeyValueHashMapElement *element) {
    assert(element);
    free(element);
}

int TestTemplateKeyValueHashMap::sumOfNumbersUptTo(int num) {
    unsigned int result = 0;
    
    for(int idx = 0;
        idx < num;
        idx++) {
        result += idx;
    }
    return result;
}

bool TestTemplateKeyValueHashMap::test(int _number_of_producer,
                                       int _number_of_production,
                                       int _number_of_consumer) {
    number_of_producer   = _number_of_producer;
    number_of_production = _number_of_production;
    number_of_consumer   = _number_of_consumer;
    production_id        = 0;
    product_id_sum       = 0;
    
    //init the producer
    for(int idx = 0;
        idx < number_of_producer;
        idx++) {
        producer_thread_group.add_thread(new boost::thread(boost::bind(&TestTemplateKeyValueHashMap::producer, this)));
    }
    
    //waith that all insert has been done
    producer_thread_group.join_all();
    assert(production_id == (_number_of_producer * _number_of_production));
    
    //init the consumer
    for(int idx = 0;
        idx < number_of_consumer;
        idx++) {
        consumer_thread_group.add_thread(new boost::thread(boost::bind(&TestTemplateKeyValueHashMap::consumer, this)));
    }
    
    //waith that all insert has been done
    consumer_thread_group.join_all();
    
    assert((number_of_consumer * sumOfNumbersUptTo(_number_of_producer * _number_of_production)) == product_id_sum);
    
    clear();
    
    return true;
}
