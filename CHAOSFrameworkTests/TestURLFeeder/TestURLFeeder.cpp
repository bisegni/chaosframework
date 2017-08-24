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

#include "TestURLFeeder.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#define NUMBER_OF_CYCLE 1000000

#define STR_ASSERT(a,b) \
assert(std::string(a).compare(std::string(b)) == 0);

#define PRINT_SERVICE \
service = static_cast<chaos::test::network::ServiceForURL *>(feeder_engine.getService()); \
if(service) \
std::cout << service->url <<std::endl; \
else \
std::cout << "no service" <<std::endl; \

#define CHECK_SERVICE(b) \
assert(service); \
if(service) STR_ASSERT(service->url, b)

#define NO_SERVICE \
assert(service == NULL);
using namespace chaos::test::network;

TestURLFeeder::TestURLFeeder():
feeder_engine("feeder_test", this),
number_of_cycle(NUMBER_OF_CYCLE) {
}

void TestURLFeeder::disposeService(void *service_ptr) {
    delete ((ServiceForURL*)service_ptr);
}

void* TestURLFeeder::serviceForURL(const common::network::URL& url,
                                uint32_t service_index) {
    ServiceForURL *result = new ServiceForURL();
    result->url = url.getURL();
    return (void*)result;
}

void TestURLFeeder::test_feeder_logic() {
    std::cout << "----------------logic feeder test-----------" << std::endl;
    feeder_engine.addURL(chaos::common::network::URL("http://test:9091"), 100);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9092"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9093"), 25);
    chaos::test::network::ServiceForURL *service = NULL;
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeFailOver);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    feeder_engine.setURLOffline(0);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    feeder_engine.setURLOnline(0);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeRoundRobin);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9093")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    
    feeder_engine.setURLOffline(0);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9093")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOffline(1);
    feeder_engine.setURLOffline(2);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    
    feeder_engine.setURLOffline(0);
    PRINT_SERVICE
    NO_SERVICE
    PRINT_SERVICE
    NO_SERVICE
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOnline(1);
    feeder_engine.setURLOnline(2);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9093")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9091")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9092")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:9093")
    std::cout << "----------------logic feeder test-----------" << std::endl;
}

void TestURLFeeder::test_feeder_logic_equal_priority() {
    std::cout << "----------------logic feeder test equal priority-----------" << std::endl;
    
    feeder_engine.addURL(chaos::common::network::URL("http://test:8000"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:8001"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:8002"), 50);
    
    feeder_engine.setURLOffline(0);
    feeder_engine.setURLOffline(1);
    feeder_engine.setURLOffline(2);
    
    chaos::test::network::ServiceForURL *service = NULL;
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeFailOver);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeRoundRobin);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8001")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8002")
    
    feeder_engine.setURLOffline(3);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8001")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8002")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8001")
    
    feeder_engine.setURLOnline(3);
    feeder_engine.setURLOffline(4);
    feeder_engine.setURLOffline(5);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    
    feeder_engine.setURLOffline(3);
    PRINT_SERVICE
    NO_SERVICE
    PRINT_SERVICE
    NO_SERVICE
    
    feeder_engine.setURLOnline(3);
    feeder_engine.setURLOnline(4);
    feeder_engine.setURLOnline(5);
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8001")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8002")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8000")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8001")
    PRINT_SERVICE
    CHECK_SERVICE("http://test:8002")
    std::cout << "----------------logic feeder test equal priority-----------" << std::endl;
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOnline(1);
    feeder_engine.setURLOnline(2);
}

void TestURLFeeder::test_feeder_preformance() {
    std::cout << "----------------performance feeder test-----------" << std::endl;
    boost::posix_time::ptime end;
    boost::posix_time::ptime start;
    
    start  = boost::posix_time::microsec_clock::universal_time();
    for (int idx = 0; idx < number_of_cycle; idx++) {
        feeder_engine.getService();
    }
    end  = boost::posix_time::microsec_clock::universal_time();
    uint64_t time_for_test = (end-start).total_microseconds();
    std::cout << "total time for "<< number_of_cycle << " cycle of getService() in round robin->" << time_for_test << std::endl;
    std::cout << "every cycle is ->" << ((double)time_for_test/(double)number_of_cycle) << " microseconds "<<std::endl;
    std::cout << "----------------performance feeder test-----------" << std::endl;
    std::cout << std::endl;
    
}

void TestURLFeeder::test(uint32_t _number_of_cycle) {
    if(_number_of_cycle>0)number_of_cycle = _number_of_cycle;
    test_feeder_logic();
    test_feeder_preformance();
    test_feeder_logic_equal_priority();
    test_feeder_preformance();
    
}
