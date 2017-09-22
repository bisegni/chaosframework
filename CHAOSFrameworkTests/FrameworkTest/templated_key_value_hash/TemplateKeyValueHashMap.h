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

#ifndef __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TemplateKeyValueHashMap_h
#define __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TemplateKeyValueHashMap_h

#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

#include <gtest/gtest.h>


typedef struct TemplateKeyValueHashMapElement {
    int product_id;
    boost::atomic<int> access_count;
} TemplatTemplateKeyValueHashMapeKeyValueHashMapElement;

class TestTemplateKeyValueHashMap:
public testing::Test,
public chaos::common::utility::TemplatedKeyValueHashMap<TemplateKeyValueHashMapElement*> {
    void clearHashTableElement(const void *key,
                               uint32_t key_len,
                               TemplateKeyValueHashMapElement *element);
protected:
    virtual void SetUp();

    int number_of_producer;
    int number_of_production;
    int number_of_consumer;
    
    boost::atomic<int> production_id;
    boost::atomic<int> product_id_sum;
    boost::atomic<int> error_count;
    boost::thread_group producer_thread_group;
    boost::thread_group consumer_thread_group;

public:
    void producer();
    void consumer();
    TestTemplateKeyValueHashMap();
};
#endif /* __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TemplateKeyValueHashMap_h */
