/*
 *	AbstractDriverPLugin.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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