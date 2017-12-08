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

#include <chaos/common/exception/CException.h>

#include "TestURLFeeder.h"

#define NUMBER_OF_CYCLE 1000000

#define CHECK_SERVICE(b) \
ASSERT_TRUE((service = static_cast<ServiceForURL *>(feeder_engine.getService()))); \
ASSERT_STREQ(service->url.c_str(), b);

#define CHECK_NO_SERVICE() \
ASSERT_TRUE((service = static_cast<ServiceForURL *>(feeder_engine.getService())) == NULL);

using namespace chaos::common::network;

TestURLFeeder::TestURLFeeder():
feeder_engine("feeder_test", this),
number_of_cycle(0){}

void* TestURLFeeder::serviceForURL(const URL& url,
                                   uint32_t service_index) {
    if(url.getURL().compare("") == 0) return NULL;
    ServiceForURL *result = new ServiceForURL();
    result->url = url.getURL();
    return (void*)result;
}

void TestURLFeeder::disposeService(void *service_ptr) {
    delete ((ServiceForURL*)service_ptr);
}

void TestURLFeeder::SetUp() {
    number_of_cycle = NUMBER_OF_CYCLE;
}

TEST_F(TestURLFeeder, TestURLFeederLogic) {
    feeder_engine.addURL(chaos::common::network::URL("http://test:9091"), 100);
    ASSERT_THROW(feeder_engine.addURL(chaos::common::network::URL(""), 50), chaos::CException);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9092"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9093"), 25);
    ServiceForURL *service = NULL;
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeFailOver);
    CHECK_SERVICE("http://test:9091");
    CHECK_SERVICE("http://test:9091");
    feeder_engine.setURLOffline(0);
    CHECK_SERVICE("http://test:9092");
    CHECK_SERVICE("http://test:9092");
    feeder_engine.setURLOnline(0);
    CHECK_SERVICE("http://test:9091");
    CHECK_SERVICE("http://test:9091");
    
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeRoundRobin);
    CHECK_SERVICE("http://test:9092");
    CHECK_SERVICE("http://test:9093");
    CHECK_SERVICE("http://test:9091");
    
    feeder_engine.setURLOffline(0);
    CHECK_SERVICE("http://test:9092");
    CHECK_SERVICE("http://test:9093");
    CHECK_SERVICE("http://test:9092");
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOffline(1);
    feeder_engine.setURLOffline(2);
    CHECK_SERVICE("http://test:9091");
    CHECK_SERVICE("http://test:9091");
    
    feeder_engine.setURLOffline(0);
    CHECK_NO_SERVICE()
    CHECK_NO_SERVICE()
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOnline(1);
    feeder_engine.setURLOnline(2);
    CHECK_SERVICE("http://test:9091");
    CHECK_SERVICE("http://test:9092");
    CHECK_SERVICE("http://test:9093");
    CHECK_SERVICE("http://test:9091");
    CHECK_SERVICE("http://test:9092");
    CHECK_SERVICE("http://test:9093");
}

TEST_F(TestURLFeeder, TestURLFeederEqualPriority) {
    feeder_engine.addURL(chaos::common::network::URL("http://test:9091"), 100);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9092"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:9093"), 25);
    feeder_engine.addURL(chaos::common::network::URL("http://test:8000"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:8001"), 50);
    feeder_engine.addURL(chaos::common::network::URL("http://test:8002"), 50);
    
    feeder_engine.setURLOffline(0);
    feeder_engine.setURLOffline(1);
    feeder_engine.setURLOffline(2);
    
    ServiceForURL *service = NULL;
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeFailOver);
    CHECK_SERVICE("http://test:8000");
    CHECK_SERVICE("http://test:8000");
    
    feeder_engine.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeRoundRobin);
    CHECK_SERVICE("http://test:8000");
    CHECK_SERVICE("http://test:8001");
    CHECK_SERVICE("http://test:8002");
    
    feeder_engine.setURLOffline(3);
    CHECK_SERVICE("http://test:8001");
    CHECK_SERVICE("http://test:8002");
    CHECK_SERVICE("http://test:8001");
    
    feeder_engine.setURLOnline(3);
    feeder_engine.setURLOffline(4);
    feeder_engine.setURLOffline(5);
    CHECK_SERVICE("http://test:8000");
    CHECK_SERVICE("http://test:8000");
    
    feeder_engine.setURLOffline(3);
    CHECK_NO_SERVICE();
    CHECK_NO_SERVICE();
    
    feeder_engine.setURLOnline(3);
    feeder_engine.setURLOnline(4);
    feeder_engine.setURLOnline(5);
    CHECK_SERVICE("http://test:8000");
    CHECK_SERVICE("http://test:8001");
    CHECK_SERVICE("http://test:8002");
    CHECK_SERVICE("http://test:8000");
    CHECK_SERVICE("http://test:8001");
    CHECK_SERVICE("http://test:8002");
    
    feeder_engine.setURLOnline(0);
    feeder_engine.setURLOnline(1);
    feeder_engine.setURLOnline(2);
}

TEST_F(TestURLFeeder, TestURLFeederPerformance) {
    for (int idx = 0; idx < number_of_cycle; idx++) {
        feeder_engine.getService();
    }
}
