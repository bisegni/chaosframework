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

#include <chaos/common/utility/InizializableService.h>

#define IS_LAPP LAPP_ << "[InizializableService]- "
#define IS_LDBG LDBG_ << "[InizializableService]- "
#define IS_LERR LERR_ << "[InizializableService]- "


using namespace chaos::common::utility;

/*!
 */
InizializableService::InizializableService() {
        //set the default value
    serviceState = CUStateKey::DEINIT;
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

bool InizializableService::initImplementation(InizializableService& impl, void *initData, const std::string & implName,  const std::string & domainString) {
    return initImplementation(&impl, initData, implName,  domainString);
}

bool InizializableService::deinitImplementation(InizializableService& impl, const std::string & implName,  const std::string & domainString) {
    return deinitImplementation(&impl, implName,  domainString);
}

/*!
 */
bool InizializableService::initImplementation(InizializableService *impl, void *initData, const std::string & implName,  const std::string & domainString)  {
    bool result = true;
    if(impl == NULL) throw CException(-1, "Implementation is null", domainString);
    try {
        IS_LAPP  << "Initializing " << implName;
        if(impl->state_machine.process_event(service_state_machine::EventType::init()) == boost::msm::back::HANDLED_TRUE) {
			try {
				impl->init(initData);
			}catch(CException& ex) {
				impl->InizializableService::state_machine.process_event(service_state_machine::EventType::deinit());
				throw;
			}
            
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_INITIATED;
        } else {
           throw CException(-2, "Service can't be initialized", domainString);
        }
        IS_LAPP  << implName << "Initialized";
    } catch (CException& ex) {
        IS_LERR  << "Error Initializing";
        DECODE_CHAOS_EXCEPTION_ON_LOG(IS_LERR, ex);
        impl->state_machine.process_event(service_state_machine::EventType::deinit());
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		IS_LERR  << "Error Deinitializing " << ex.what();
		throw CException(-3, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}

/*!
 */
bool InizializableService::deinitImplementation(InizializableService *impl, const std::string & implName,  const std::string & domainString) {
    bool result = true;
    if(impl == NULL) throw CException(-1, "Implementation is null", domainString);
    try {
        IS_LAPP  << "Deinitializing " << implName;
        if(impl->state_machine.process_event(service_state_machine::EventType::deinit()) == boost::msm::back::HANDLED_TRUE) {
            impl->deinit();
        } else {
            throw CException(-2, "Service can't be deinitialized", domainString);
        }
        impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_DEINTIATED;
        IS_LAPP  << implName << "Deinitialized";
    } catch (CException& ex) {
        IS_LERR  << "Error Deinitializing";
        DECODE_CHAOS_EXCEPTION_ON_LOG(IS_LERR, ex);
        throw;
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
		IS_LERR  << "Error Deinitializing " << ex.what();
		throw CException(-3, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
	}
    return result;
}
