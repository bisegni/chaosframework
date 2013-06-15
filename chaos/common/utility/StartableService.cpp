/*
 *	StartableService.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/utility/StartableService.h>

using namespace chaos::utility;


#define SS_LAPP LAPP_ << "[StartableService]- "


/*!
 */
bool StartableService::startImplementation(StartableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LAPP  << "Starting " << implName;
        impl->serviceState = SS_STARTING;
        impl->start();
        impl->serviceState = SS_STARTED;
        SS_LAPP  << implName << "Started";
    } catch (CException ex) {
        SS_LAPP  << "Error Starting " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}

/*!
 */
bool StartableService::stopImplementation(StartableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LAPP  << "Stopping " << implName;
        impl->serviceState = SS_STOPPING;
        impl->stop();
        impl->serviceState = SS_STOPPED;
        SS_LAPP  << implName << "Stopped";
    } catch (CException ex) {
        SS_LAPP  << "Error Starting " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}
