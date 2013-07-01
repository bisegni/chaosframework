//
//  DriverWrapperPlugin.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

using namespace chaos::cu::dm::driver;

AbstractDriverPlugin::AbstractDriverPlugin(AbstractDriver *_abstractDriver):abstractDriver(_abstractDriver) {
    
}

AbstractDriverPlugin::~AbstractDriverPlugin() {
};

//! Proxy for create a new accessor to the driver

DriverAccessor * const AbstractDriverPlugin::getNewAccessor() {
    DriverAccessor *newAccessor;
    if(abstractDriver->getNewAccessor(&newAccessor)) {
        return newAccessor;
    } else {
        return NULL;
    }
}

//! Proxy for dispose an accessor
void AbstractDriverPlugin::releaseAccessor(DriverAccessor *accessor) {
    abstractDriver->releaseAccessor(accessor);
}