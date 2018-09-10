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

#include <chaos/common/global.h>
#include <chaos/common/utility/StartableService.h>

using namespace chaos::common::utility;

using namespace std;
#define SS_LAPP LAPP_ << "["<<implName<<"]- "
#define SS_LDBG LDBG_ << "["<<implName<<"]- "
#define SS_LERR LERR_ << "["<<implName<<"]- "

StartableService::StartableService() {
}

StartableService::~StartableService() {
    //memory for state machine is release by base class
}

//! Start the implementation
void StartableService::start()  {
    
}

//! Start the implementation
void StartableService::stop()  {
    
}

bool StartableService::startImplementation(StartableService& impl, const string & implName,  const string & domainString) {
    return startImplementation(&impl, implName,  domainString);
}

bool StartableService::stopImplementation(StartableService& impl, const string & implName,  const string & domainString) {
    return stopImplementation(&impl, implName,  domainString);
}

bool StartableService::initImplementation(StartableService& impl, void *initData, const string & implName,  const string & domainString) {
    return initImplementation(&impl, initData, implName,  domainString);
}

bool StartableService::deinitImplementation(StartableService& impl, const string & implName,  const string & domainString) {
    return deinitImplementation(&impl, implName,  domainString);
}

/*!
 */
bool StartableService::initImplementation(StartableService *impl, void *initData, const string & implName,  const string & domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LDBG  << "Initializing " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::init()) == boost::msm::back::HANDLED_TRUE) {
			try {
				impl->init(initData);
			} catch (CException& ex) {
				impl->StartableService::state_machine.process_event(service_state_machine::EventType::deinit());
				throw;
			}
            
            impl->serviceState = impl->state_machine.current_state()[0]; //service_state_machine::InizializableServiceType::IS_INITIATED;
        } else {
			DEBUG_CODE(SS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be initialized", domainString);
        }
        SS_LDBG  << implName << "Initialized";
    } catch (CException& ex) {
        SS_LAPP  << "Error initializing " << implName;
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		SS_LERR  << "Error initializing " << ex.what();
		throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}

/*!
 */
bool StartableService::deinitImplementation(StartableService *impl, const string & implName,  const string & domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LDBG  << "Deinitializing " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::deinit()) == boost::msm::back::HANDLED_TRUE) {
			impl->deinit();
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_DEINTIATED;
        } else {
			DEBUG_CODE(SS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be deinitialize", domainString);
        }
        SS_LDBG  << implName << "Deinitialized";
    } catch (CException& ex) {
        SS_LERR  << "Error Deinitializing " << implName;
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		SS_LERR  << "Error Deinitializing " << ex.what();
		throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}
/*!
 */
bool StartableService::startImplementation(StartableService *impl, const string & implName,  const string & domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LDBG  << "Starting " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::start()) == boost::msm::back::HANDLED_TRUE) {
			try{
				impl->start();
			}catch(CException& ex) {
				impl->StartableService::state_machine.process_event(service_state_machine::EventType::init());
				throw;
			}
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::StartableServiceType::SS_STARTED;
        }else {
			DEBUG_CODE(SS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be started", domainString);
        }
        SS_LDBG  << implName << " Started";
    } catch (CException& ex) {
      SS_LAPP  << "Error Starting " << implName<<": "<<ex.what();
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		SS_LERR  << "Error Starting " << implName<<": "<<ex.what();
		throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}

/*!
 */
bool StartableService::stopImplementation(StartableService *impl, const string & implName,  const string & domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        SS_LDBG  << "Stopping " << implName;
        if(impl->StartableService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE) {
            try{
				impl->stop();
			}catch(CException& ex) {
				impl->StartableService::state_machine.process_event(service_state_machine::EventType::start());
				throw;
			}
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::StartableServiceType::SS_STOPPED;
        }else {
			DEBUG_CODE(SS_LDBG << "Service cant flow to next state and current is ->" << impl->state_machine.current_state()[0];)
            throw CException(0, "Service cant be stopped", domainString);
        }
        SS_LDBG  << implName << " Stopped";
    } catch (CException& ex) {
        SS_LAPP  << "Error Stopping " << implName;
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		SS_LERR  << "Error Stopping " << ex.what();
		throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}
