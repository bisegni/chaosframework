/*
 *	LoadableService.cpp
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
#include <chaos/common/utility/LoadableService.h>

using namespace chaos::utility;


#define LS_LAPP LAPP_ << "["<<implName<<"]- "
#define LS_LDBG LDBG_ << "["<<implName<<"]- "

LoadableService::LoadableService() {
}

LoadableService::~LoadableService() {
    //memory for state machine is release by base class
}

//! Start the implementation
void LoadableService::load(void *load_data) throw(chaos::CException) {
    
}

//! Start the implementation
void LoadableService::toLoad() throw(chaos::CException) {
    
}

bool LoadableService::loadImplementation(LoadableService& impl, void *load_data, const char * const implName,  const char * const domainString) {
    return loadImplementation(&impl, load_data, implName,  domainString);
}

bool LoadableService::toloadImplementation(LoadableService& impl, const char * const implName,  const char * const domainString) {
    return toloadImplementation(&impl, implName,  domainString);
}

bool LoadableService::initImplementation(LoadableService& impl, void *initData, const char * const implName,  const char * const domainString) {
    return initImplementation(&impl, initData, implName,  domainString);
}

bool LoadableService::deinitImplementation(LoadableService& impl, const char * const implName,  const char * const domainString) {
    return deinitImplementation(&impl, implName,  domainString);
}

bool LoadableService::startImplementation(LoadableService& impl, const char * const implName,  const char * const domainString) {
    return startImplementation(&impl, implName,  domainString);
}

bool LoadableService::stopImplementation(LoadableService& impl, const char * const implName,  const char * const domainString) {
    return stopImplementation(&impl, implName,  domainString);
}
/*!
 */
bool LoadableService::loadImplementation(LoadableService *impl, void *load_data, const char * const implName,  const char * const domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Loading " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::load()) == boost::msm::back::HANDLED_TRUE) {
			try {
				impl->load(load_data);
			} catch (CException& ex) {
				impl->LoadableService::state_machine.process_event(service_state_machine::EventType::unload());
				throw ex;
			}
            
            impl->serviceState = impl->state_machine.current_state()[0];
        } else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be load", domainString);
        }
        LS_LDBG  << implName << "Loaded";
    } catch (CException& ex) {
        LS_LAPP  << "Error Loading " << implName;
        throw ex;
    }
    return result;
}

/*!
 */
bool LoadableService::toloadImplementation(LoadableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Returnning to load state " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::load()) == boost::msm::back::HANDLED_TRUE) {
			impl->toLoad();
            impl->serviceState = impl->state_machine.current_state()[0];
        } else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be set to load", domainString);
        }
        LS_LDBG  << implName << "To load state";
    } catch (CException& ex) {
        LS_LAPP  << "Error Unloading " << implName;
        throw ex;
    }
    return result;
}
/*!
 */
bool LoadableService::initImplementation(LoadableService *impl, void *initData, const char * const implName,  const char * const domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Initializing " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::initialize()) == boost::msm::back::HANDLED_TRUE) {
			try {
				impl->init(initData);
			} catch (CException& ex) {
				impl->LoadableService::state_machine.process_event(service_state_machine::EventType::deinitialize());
				throw ex;
			}
            
            impl->serviceState = impl->state_machine.current_state()[0]; //service_state_machine::InizializableServiceType::IS_INITIATED;
        } else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be initialized", domainString);
        }
        LS_LDBG  << implName << "Initialized";
    } catch (CException& ex) {
        LS_LAPP  << "Error initializing " << implName;
        throw ex;
    }
    return result;
}

/*!
 */
bool LoadableService::deinitImplementation(LoadableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Deinitializing " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::deinitialize()) == boost::msm::back::HANDLED_TRUE) {
			impl->deinit();
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_DEINTIATED;
        } else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be deinitialize", domainString);
        }
        LS_LDBG  << implName << "Deinitialized";
    } catch (CException& ex) {
        LS_LAPP  << "Error Deinitializing " << implName;
        throw ex;
    }
    return result;
}
/*!
 */
bool LoadableService::startImplementation(LoadableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Starting " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::start()) == boost::msm::back::HANDLED_TRUE) {
			try{
				impl->start();
			}catch(CException& ex) {
				impl->LoadableService::state_machine.process_event(service_state_machine::EventType::initialize());
				throw ex;
			}
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::LoadableServiceType::SS_STARTED;
        }else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be started", domainString);
        }
        LS_LDBG  << implName << " Started";
    } catch (CException& ex) {
        LS_LAPP  << "Error Starting " << implName;
        throw ex;
    }
    return result;
}

/*!
 */
bool LoadableService::stopImplementation(LoadableService *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        LS_LDBG  << "Stopping " << implName;
        if(impl->LoadableService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE) {
            try{
				impl->stop();
			}catch(CException& ex) {
				impl->LoadableService::state_machine.process_event(service_state_machine::EventType::start());
				throw ex;
			}
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::LoadableServiceType::SS_STOPPED;
        }else {
			DEBUG_CODE(LS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be stopped", domainString);
        }
        LS_LDBG  << implName << " Stopped";
    } catch (CException& ex) {
        LS_LAPP  << "Error Starting " << implName;
        throw ex;
    }
    return result;
}