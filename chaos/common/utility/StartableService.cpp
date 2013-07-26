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

StartableService::StartableService() {
}

StartableService::~StartableService() {
    //memory for state machine is release by base class
}

//! Start the implementation
void StartableService::start() throw(chaos::CException) {
    
}

//! Start the implementation
void StartableService::stop() throw(chaos::CException) {
    
}

bool StartableService::startImplementation(StartableService& impl, const char * const implName,  const char * const domainString) {
    return startImplementation(&impl, implName,  domainString);
}

bool StartableService::stopImplementation(StartableService& impl, const char * const implName,  const char * const domainString) {
    return stopImplementation(&impl, implName,  domainString);
}

bool StartableService::initImplementation(StartableService& impl, void *initData, const char * const implName,  const char * const domainString) {
    return initImplementation(&impl, initData, implName,  domainString);
}

bool StartableService::deinitImplementation(StartableService& impl, const char * const implName,  const char * const domainString) {
    return deinitImplementation(&impl, implName,  domainString);
}

/*!
 */
bool StartableService::initImplementation(StartableService *impl, void *initData, const char * const implName,  const char * const domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LAPP  << "Initializing " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::initialize()) == boost::msm::back::HANDLED_TRUE) {
            impl->init(initData);
            impl->serviceState = InizializableServiceType::IS_INITIATED;
        }else {
            throw CException(0, "Service cant be initialized", domainString);
        }
        SS_LAPP  << implName << "Initialized";
    } catch (CException ex) {
        SS_LAPP  << "Error initializing " << implName;
        throw ex;
    }
    return result;
}

/*!
 */
bool StartableService::deinitImplementation(StartableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LAPP  << "Deinitializing " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::deinitialize()) == boost::msm::back::HANDLED_TRUE) {
            impl->deinit();
            impl->serviceState = InizializableServiceType::IS_DEINTIATED;
        }else {
            throw CException(0, "Service cant be deinitialize", domainString);
        }
        SS_LAPP  << implName << "Deinitialized";
    } catch (CException ex) {
        SS_LAPP  << "Error Deinitializing " << implName;
        throw ex;
    }
    return result;
}
/*!
 */
bool StartableService::startImplementation(StartableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LAPP  << "Starting " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::start()) == boost::msm::back::HANDLED_TRUE) {
            impl->start();
            impl->serviceState = StartableServiceType::SS_STARTED;
        }else {
            throw CException(0, "Service cant be started", domainString);
        }
        SS_LAPP  << implName << " Started";
    } catch (CException ex) {
        SS_LAPP  << "Error Starting " << implName;
        throw ex;
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
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE) {
            impl->stop();
            impl->serviceState = StartableServiceType::SS_STOPPED;
        }else {
            throw CException(0, "Service cant be stopped", domainString);
        }
        SS_LAPP  << implName << " Stopped";
    } catch (CException ex) {
        SS_LAPP  << "Error Starting " << implName;
        throw ex;
    }
    return result;
}
