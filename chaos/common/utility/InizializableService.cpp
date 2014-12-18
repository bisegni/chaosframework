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
    serviceState = service_state_machine::InizializableServiceType::IS_DEINTIATED;
}

/*!
 */
InizializableService::~InizializableService() {
    //if(state_machine) delete (state_machine);
}

//! Initialize instance
void InizializableService::init(void*) throw(chaos::CException) {
}

//! Deinit the implementation
void InizializableService::deinit() throw(chaos::CException) {
    
}
    //! Return the state
const uint8_t InizializableService::getServiceState() const {
    return serviceState;
}

bool InizializableService::initImplementation(InizializableService& impl, void *initData, const string & implName,  const string & domainString) {
    return initImplementation(&impl, initData, implName,  domainString);
}

bool InizializableService::deinitImplementation(InizializableService& impl, const string & implName,  const string & domainString) {
    return deinitImplementation(&impl, implName,  domainString);
}

/*!
 */
bool InizializableService::initImplementation(InizializableService *impl, void *initData, const string & implName,  const string & domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        IS_LAPP  << "Initializing " << implName;
        if(impl->state_machine.process_event(service_state_machine::EventType::initialize()) == boost::msm::back::HANDLED_TRUE) {
			try {
				impl->init(initData);
			}catch(CException& ex) {
				impl->InizializableService::state_machine.process_event(service_state_machine::EventType::deinitialize());
				throw ex;
			}
            
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_INITIATED;
        } else {
           throw CException(0, "Service cant be initialized", domainString);
        }
        IS_LAPP  << implName << "Initialized";
    } catch (CException& ex) {
        IS_LAPP  << "Error initializing " << implName;
        impl->state_machine.process_event(service_state_machine::EventType::deinitialize());
        throw ex;
    }
    return result;
}

/*!
 */
bool InizializableService::deinitImplementation(InizializableService *impl, const string & implName,  const string & domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        IS_LAPP  << "Deinitializing " << implName;
        if(impl->state_machine.process_event(service_state_machine::EventType::deinitialize()) == boost::msm::back::HANDLED_TRUE) {
            impl->deinit();
        } else {
            throw CException(0, "Service cant be deinitialized", domainString);
        }
        impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_DEINTIATED;
        IS_LAPP  << implName << "Deinitialized";
    } catch (CException& ex) {
        IS_LAPP  << "Error Deinitializing " << implName;
        throw ex;
    }
    return result;
}