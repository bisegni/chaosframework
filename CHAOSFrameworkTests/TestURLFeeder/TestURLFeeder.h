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
