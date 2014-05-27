//
//  FeederService.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 28/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "FeederService.h"

using namespace chaos::test::network;

void FeederService::disposeService(void *service_ptr) {
    delete ((ServiceForURL*)service_ptr);
}

void* FeederService::serviceForURL(common::network::URL url) {
    ServiceForURL *result = new ServiceForURL();
    result->url = url.getURL();
    return (void*)result;
}