//
//  FeederTest.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__FeederTest__
#define __CHAOSFramework__FeederTest__

#include <chaos/common/network/URLServiceFeeder.h>

namespace chaos {
    namespace test {
        namespace network {
            
            class ServiceForURL {
            public:
                std::string url;
            };
            
            class FeederTest : public common::network::URLServiceFeederHandler {
				chaos::common::network::URLServiceFeeder feeder_engine;
				uint32_t number_of_cycle;
            protected:
                
                void disposeService(void *service_ptr);
                void* serviceForURL(const common::network::URL& url, uint32_t service_index);
				
				void test_feeder_logic();
				void test_feeder_logic_equal_priority();
				void test_feeder_preformance();
			public:
				void test(uint32_t _number_of_cycle = 0);
				FeederTest();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__FeederTest__) */
