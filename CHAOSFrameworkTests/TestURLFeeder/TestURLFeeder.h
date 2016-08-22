/*
 *	TestURLFeeder.h
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

#ifndef __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h
#define __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h

#include <chaos/common/network/URLServiceFeeder.h>

namespace chaos {
    namespace test {
        namespace network {
            
            class ServiceForURL {
            public:
                std::string url;
            };
            
            class TestURLFeeder : public common::network::URLServiceFeederHandler {
                chaos::common::network::URLServiceFeeder feeder_engine;
                uint32_t number_of_cycle;
            protected:
                
                void disposeService(void *service_ptr);
                void* serviceForURL(const common::network::URL& url,
                                    uint32_t service_index);
                
                void test_feeder_logic();
                void test_feeder_logic_equal_priority();
                void test_feeder_preformance();
            public:
                void test(uint32_t _number_of_cycle = 0);
                TestURLFeeder();
            };
            
        }
    }
}

#endif /* __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h */
