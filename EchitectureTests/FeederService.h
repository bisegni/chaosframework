//
//  FeederService.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__FeederService__
#define __CHAOSFramework__FeederService__

#include <chaos/common/network/URLServiceFeeder.h>
namespace chaos {
    namespace test {
        namespace network {
            
            class ServiceForURL {
            public:
                std::string url;
            };
            
            class FeederService : public common::network::URLServiceFeederHandler {
            protected:
                
                void disposeService(void *service_ptr);
                void* serviceForURL(common::network::URL url);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__FeederService__) */
