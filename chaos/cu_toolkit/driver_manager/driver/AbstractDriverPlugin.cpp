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

#include <chaos/common/plugin/AbstractPlugin.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

using namespace chaos::cu::driver_manager::driver;

AbstractDriverPlugin::AbstractDriverPlugin(AbstractDriver *_abstractDriver):abstractDriver(_abstractDriver) {
    
}

AbstractDriverPlugin::~AbstractDriverPlugin() {
}

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
