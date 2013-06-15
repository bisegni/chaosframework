/*
 *	InizializableService.cpp
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

#include <chaos/common/utility/InizializableService.h>

#define IS_LAPP LAPP_ << "[InizializableService]- "


using namespace chaos::utility;
/*!
 */
InizializableService::InizializableService() {
        //set the default value
    serviceState = InizializableServiceType::IS_DEINTIATED;
}

/*!
 */
InizializableService::~InizializableService() {
    
}

    //! Return the state
const uint8_t InizializableService::getServiceState() const {
    return serviceState;
}

/*!
 */
bool InizializableService::initImplementation(InizializableService *impl, void *initData, const char * const implName,  const char * const domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        IS_LAPP  << "Initializing " << implName;
        impl->serviceState = InizializableServiceType::IS_INITING;
        impl->init(initData);
        impl->serviceState = InizializableServiceType::IS_INITIATED;
        IS_LAPP  << implName << "Initialized";
    } catch (CException ex) {
        IS_LAPP  << "Error initializing " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}

/*!
 */
bool InizializableService::deinitImplementation(InizializableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        IS_LAPP  << "Deinitializing " << implName;
        impl->serviceState = InizializableServiceType::IS_DEINITING;
        impl->deinit();
        impl->serviceState = InizializableServiceType::IS_DEINTIATED;
        IS_LAPP  << implName << "Deinitialized";
    } catch (CException ex) {
        IS_LAPP  << "Error Deinitializing " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}