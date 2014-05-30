//
//  main.cpp
//  EchitectureTests
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/network/URLServiceFeeder.h>
#include "FeederService.h"

#define PRINT_SERVICE \
service = static_cast<chaos::test::network::ServiceForURL *>(feeder_test.getService()); \
if(service) \
std::cout << service->url <<std::endl; \
else \
std::cout << "no service" <<std::endl; \


int main(int argc, const char * argv[])
{
    chaos::test::network::FeederService fd;
    chaos::common::network::URLServiceFeeder feeder_test("feeder_test", &fd);
    int idx[2];
    idx[0] = feeder_test.addURL(chaos::common::network::URL("http://test:9091"),100);
    idx[1] = feeder_test.addURL(chaos::common::network::URL("http://test:9092"));
    
    chaos::test::network::ServiceForURL *service = NULL;
    feeder_test.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeFailOver);
    PRINT_SERVICE
    PRINT_SERVICE
    
    feeder_test.setFeedMode(chaos::common::network::URLServiceFeeder::URLServiceFeedModeRoundRobin);
    PRINT_SERVICE
    PRINT_SERVICE
    
    feeder_test.setURLOffline(0);
    PRINT_SERVICE
    PRINT_SERVICE

    feeder_test.setURLOnline(0);
    feeder_test.setURLOffline(1);
    PRINT_SERVICE
    PRINT_SERVICE

    feeder_test.setURLOffline(0);
    PRINT_SERVICE
    PRINT_SERVICE
    feeder_test.setURLOnline(0);
    feeder_test.setURLOnline(1);
    PRINT_SERVICE
    PRINT_SERVICE

    return 0;
}

