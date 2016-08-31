/*
 *	TestTemplateKeyValueHashMap.h
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

#ifndef __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TestTemplateKeyValueHashMap_h
#define __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TestTemplateKeyValueHashMap_h

#include <chaos/common/utility/TemplatedKeyValueHashMap.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
namespace chaos {
    namespace common {
        namespace utility {
            namespace test {
                
                typedef struct TestTemplateKeyValueHashMapElement {
                    int product_id;
                    boost::atomic<int> access_count;
                } TestTemplateKeyValueHashMapElement;
                
                class TestTemplateKeyValueHashMap:
                public TemplatedKeyValueHashMap<TestTemplateKeyValueHashMapElement*> {
                    int number_of_producer;
                    int number_of_production;
                    int number_of_consumer;
                    
                    boost::atomic<int> production_id;
                    boost::atomic<int> product_id_sum;
                    boost::atomic<int> error_count;
                    boost::thread_group producer_thread_group;
                    boost::thread_group consumer_thread_group;
                    
                    std::vector<std::string> production_name_list;
                protected:
                    //! template element deallocation user method (to override)
                    void clearHashTableElement(const void *key,
                                               uint32_t key_len,
                                               TestTemplateKeyValueHashMapElement *element);
                public:
                    void producer();
                    void consumer();
                    int sumOfNumbersUptTo(int num);
                    bool test(int _number_of_producer,
                              int _number_of_producetion,
                              int _number_of_consumer);
                };
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests__1BE9BCC_CC46_46CD_8B1C_6C06C9FC07E0_TestTemplateKeyValueHashMap_h */