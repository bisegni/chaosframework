/*
 *	SWEService.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/utility/SWEService.h>

using namespace chaos::common::utility;

using namespace std;
#define SWE_LAPP LAPP_ << "["<<impl_name<<"]- "
#define SWE_LDBG LDBG_ << "["<<impl_name<<"]- "
#define SWE_LERR LERR_ << "["<<impl_name<<"]- "

SWEService::SWEService():
last_state(-1){}

SWEService::~SWEService() {}

bool SWEService::startImplementation(SWEService& impl, const string & impl_name,  const string & domain_string) {
    return startImplementation(&impl, impl_name,  domain_string);
}

bool SWEService::stopImplementation(SWEService& impl, const string & impl_name,  const string & domain_string) {
    return stopImplementation(&impl, impl_name,  domain_string);
}

bool SWEService::initImplementation(SWEService& impl, const string & impl_name,  const string & domain_string) {
    return initImplementation(&impl, impl_name,  domain_string);
}

bool SWEService::deinitImplementation(SWEService& impl, const string & impl_name,  const string & domain_string) {
    return deinitImplementation(&impl, impl_name,  domain_string);
}

bool SWEService::goInFatalError(SWEService& impl, chaos::CException& ex,  const std::string & impl_name,  const std::string & domain_string) {
    return goInFatalError(&impl, ex, impl_name, domain_string);
}

bool SWEService::goInRecoverableError(SWEService& impl, chaos::CException& ex,  const std::string & impl_name,  const std::string & domain_string) {
    return goInRecoverableError(&impl, ex, impl_name, domain_string);
}

bool SWEService::recoverError(SWEService& impl, const std::string & impl_name,  const std::string & domain_string) {
    return recoverError(&impl, impl_name, domain_string);
}

/*!
 */
bool SWEService::initImplementation(SWEService *impl,
                                    const string & impl_name,
                                    const string & domain_string)  {
    bool result = false;
    try {
        if(impl == NULL) {return false;}
        
        SWE_LDBG  << "Initializing " << impl_name;
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::init()) == boost::msm::back::HANDLED_TRUE))) {
            impl->serviceState = impl->state_machine.current_state()[0];
            SWE_LDBG  << impl_name << "Initialized";
            result = true;
        }
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
    }
    return result;
}

/*!
 */
bool SWEService::deinitImplementation(SWEService *impl,
                                      const string & impl_name,
                                      const string & domain_string) {
    bool result = false;
    try {
        if(impl == NULL) return false;
        
        SWE_LDBG  << "Deinitializing " << impl_name;
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::deinit()) == boost::msm::back::HANDLED_TRUE))) {
            impl->serviceState = impl->state_machine.current_state()[0];
            SWE_LDBG  << impl_name << "Deinitialized";
        }
       
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
    }
    return result;
}
/*!
 */
bool SWEService::startImplementation(SWEService *impl,
                                     const string & impl_name,
                                     const string & domain_string) {
    bool result = false;
    try {
        if(impl == NULL) {return false;}
        
        SWE_LDBG  << "Starting " << impl_name;
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::start()) == boost::msm::back::HANDLED_TRUE))) {
            impl->serviceState = impl->state_machine.current_state()[0];
            SWE_LDBG  << impl_name << " Started";
        }

    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << impl_name<<": "<<ex.what();
    }
    return result;
}

/*!
 */
bool SWEService::stopImplementation(SWEService *impl, const string & impl_name,  const string & domain_string) {
    bool result = false;
    try {
        if(impl == NULL) return false;
        
        SWE_LDBG  << "Stopping " << impl_name;
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE))) {
            impl->serviceState = impl->state_machine.current_state()[0];
            SWE_LDBG  << impl_name << " Stopped";
        }
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
    }
    return result;
}


bool SWEService::goInFatalError(SWEService *impl,  chaos::CException& ex, const std::string & impl_name,  const std::string & domain_string) {
    bool result = false;
    
    try {
        if(impl == NULL) return false;
        if(impl->serviceState != CUStateKey::RECOVERABLE_ERROR &&
           impl->serviceState != CUStateKey::FATAL_ERROR ) {
            //keep track of last state
            impl->last_state = impl->state_machine.current_state()[0];
        }
        SWE_LDBG  << "Going into fatal error state for " << impl_name;
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::fatal_error())  == boost::msm::back::HANDLED_TRUE))) {
            //keep track of current state
            impl->serviceState = impl->state_machine.current_state()[0];
            //call fatal error handler
            impl->fatalErrorFromState(impl->last_state, ex);
            SWE_LDBG  << impl_name << " into fatal error";
        }
    } catch (CException& ex) {
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with "<< ex.what();
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with "<< ex.what();
    } catch(...) {
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with general exception";
    }
    return result;
}

bool SWEService::goInRecoverableError(SWEService *impl, chaos::CException& ex, const std::string & impl_name,  const std::string & domain_string) {
    bool result = false;
    try {
        if(impl == NULL) return false;
        if(impl->serviceState != CUStateKey::RECOVERABLE_ERROR &&
           impl->serviceState != CUStateKey::FATAL_ERROR ) {
            //keep track of last state
            impl->last_state = impl->state_machine.current_state()[0];
        }
        
        DEBUG_CODE(SWE_LDBG  << "Going into recoverable state for " << impl_name;)
        if((result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::recoverable_error())  == boost::msm::back::HANDLED_TRUE))) {
            impl->serviceState = impl->state_machine.current_state()[0];
            impl->recoverableErrorFromState(impl->last_state, ex);
            SWE_LDBG  << impl_name << " into recoverable state";
        }
    } catch (CException& ex) {
        SWE_LAPP  << "Error going into recoverable error for " << impl_name << " with "<< ex.what();
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LAPP  << "Error going into recoverable error for " << impl_name << " with "<< ex.what();
    } catch(...) {
        SWE_LAPP  << "Error going into recoverable error for " << impl_name << " with general exception";
    }
    return result;
}

bool SWEService::recoverError(SWEService *impl, const std::string & impl_name,  const std::string & domain_string) {
    bool result = false;
    bool change_state_result = false;
    try {
        if(impl == NULL) return false;
        DEBUG_CODE(SWE_LDBG  << "Try to recover error " << impl_name;)
        //call handler for infor the we are going to the last state
        if((result = impl->beforeRecoverErrorFromState(impl->last_state))) {
            //we can go to the last error
            switch (impl->last_state) {
                case CUStateKey::INIT : {
                    change_state_result = ((impl->SWEService::state_machine.process_event(service_state_machine::EventType::init())  == boost::msm::back::HANDLED_TRUE));
                    break;
                }
                    
                case CUStateKey::START:{
                    change_state_result = ((impl->SWEService::state_machine.process_event(service_state_machine::EventType::start())  == boost::msm::back::HANDLED_TRUE));
                    break;
                }
                    
                case CUStateKey::STOP:{
                    change_state_result = ((impl->SWEService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE));
                    break;
                }
                    
                case CUStateKey::DEINIT:{
                    change_state_result = ((impl->SWEService::state_machine.process_event(service_state_machine::EventType::deinit())  == boost::msm::back::HANDLED_TRUE));
                    break;
                }
                default:
                    break;
            }
            
            impl->serviceState = impl->state_machine.current_state()[0];
            
            impl->recoveredToState(impl->last_state);
            
            impl->last_state = -1;
        }
    } catch (CException& ex) {
        SWE_LAPP  << "Error recovering error for " << impl_name << " with "<< ex.what();
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LAPP  << "Error recovering fatal error for " << impl_name << " with "<< ex.what();
    } catch(...) {
        SWE_LAPP  << "Error recovering fatal error for " << impl_name << " with general exception";
    }
    return result;
}
