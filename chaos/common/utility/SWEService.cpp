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

bool SWEService::initImplementation(SWEService& impl, void *initData, const string & impl_name,  const string & domain_string) {
    return initImplementation(&impl, initData, impl_name,  domain_string);
}

bool SWEService::deinitImplementation(SWEService& impl, const string & impl_name,  const string & domain_string) {
    return deinitImplementation(&impl, impl_name,  domain_string);
}

bool SWEService::goInFatalError(SWEService& impl, const std::string & impl_name,  const std::string & domain_string) {
    return goInFatalError(&impl, impl_name, domain_string);
}

bool SWEService::goInRecoverableError(SWEService& impl, const std::string & impl_name,  const std::string & domain_string) {
    return goInRecoverableError(&impl, impl_name, domain_string);
}

bool SWEService::recoverError(SWEService& impl, const std::string & impl_name,  const std::string & domain_string) {
    return recoverError(&impl, impl_name, domain_string);
}

/*!
 */
bool SWEService::initImplementation(SWEService *impl, void *initData, const string & impl_name,  const string & domain_string)  {
    bool result = true;
    try {
        if(impl == NULL) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Implementation of %1% is null", %domain_string)}
        if(impl->serviceState == CUStateKey::RECOVERABLE_ERROR) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -2, "Recoverable error state need to be recovered for %1%", %domain_string)}
        
        SWE_LDBG  << "Initializing " << impl_name;
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::init()) == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0]; //service_state_machine::InizializableServiceType::IS_INITIATED;
            try {
                impl->init(initData);
            } catch (CException& ex) {
                impl->SWEService::state_machine.process_event(service_state_machine::EventType::deinit());
                throw ex;
            }
        } else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -3, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << "Initialized";
    } catch (CException& ex) {
        SWE_LAPP  << "Error initializing " << impl_name;
        throw ex;
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
        throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
    }
    return result;
}

/*!
 */
bool SWEService::deinitImplementation(SWEService *impl, const string & impl_name,  const string & domain_string) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(-1, "Implementation is null", domain_string);
        if(impl->serviceState == CUStateKey::RECOVERABLE_ERROR) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Recoverable error state need to be recovered for %1%", %domain_string)}
        
        SWE_LDBG  << "Deinitializing " << impl_name;
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::init()) == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::InizializableServiceType::IS_DEINTIATED;
            impl->deinit();
        } else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << "Deinitialized";
    } catch (CException& ex) {
        SWE_LERR  << "Error Deinitializing " << impl_name;
        throw ex;
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
        throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
    }
    return result;
}
/*!
 */
bool SWEService::startImplementation(SWEService *impl, const string & impl_name,  const string & domain_string) {
    bool result = true;
    try {
        if(impl == NULL) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Implementation of %1% is null", %domain_string)}
        if(impl->serviceState == CUStateKey::RECOVERABLE_ERROR) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -2, "Recoverable error state need to be recovered for %1%", %domain_string)}
        
        SWE_LDBG  << "Starting " << impl_name;
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::start()) == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0];//service_state_machine::SWEServiceType::SS_STARTED;
            impl->start();
        }else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << " Started";
    } catch (CException& ex) {
        SWE_LAPP  << "Error Starting " << impl_name<<": "<<ex.what();
        throw ex;
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << impl_name<<": "<<ex.what();
        throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
    }
    return result;
}

/*!
 */
bool SWEService::stopImplementation(SWEService *impl, const string & impl_name,  const string & domain_string) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(-1, "Implementation is null", domain_string);
        if(impl->serviceState == CUStateKey::RECOVERABLE_ERROR) {LOG_AND_TROW_FORMATTED(SWE_LDBG, -2, "Recoverable error state need to be recovered for %1%", %domain_string)}
        
        SWE_LDBG  << "Stopping " << impl_name;
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0];
            impl->stop();
        }else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << " Stopped";
    } catch (CException& ex) {
        SWE_LAPP  << "Error Starting " << impl_name;
        throw ex;
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LERR  << "Error Deinitializing " << ex.what();
        throw CException(-1, std::string(ex.what()), std::string(__PRETTY_FUNCTION__));
    }
    return result;
}


bool SWEService::goInFatalError(SWEService *impl, const std::string & impl_name,  const std::string & domain_string) {
    bool result = true;
    
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domain_string);
        if(impl->serviceState != CUStateKey::RECOVERABLE_ERROR &&
           impl->serviceState != CUStateKey::FATAL_ERROR ) {
            //keep track of last state
            impl->last_state = impl->state_machine.current_state()[0];
        }
        SWE_LDBG  << "Going into fatal error state for " << impl_name;
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::fatal_error())  == boost::msm::back::HANDLED_TRUE) {
            //keep track of current state
            impl->serviceState = impl->state_machine.current_state()[0];
            
            switch(impl->last_state) {
                case CUStateKey::INIT:{
                    CHAOS_NOT_THROW(impl->deinit();)
                    break;
                }
                
                case CUStateKey::START:{
                    CHAOS_NOT_THROW(impl->stop();)
                    CHAOS_NOT_THROW(impl->deinit();)
                    break;
                }
                    
                case CUStateKey::STOP:{
                    CHAOS_NOT_THROW(impl->deinit();)
                    break;
                }
                    
                case CUStateKey::DEINIT:{
                    break;
                }
                    
                case CUStateKey::RECOVERABLE_ERROR:{
                    CHAOS_NOT_THROW(impl->stop();)
                    CHAOS_NOT_THROW(impl->deinit();)
                    break;
                }
            }
            
            //call fatal error handler
            impl->fatalErrorFromState(impl->last_state);
        }else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << " into fatal error";
    } catch (CException& ex) {
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with "<< ex.what();
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with "<< ex.what();
    } catch(...) {
        SWE_LAPP  << "Error going into fatal error for " << impl_name << " with general exception";
    }
    return result;
}

bool SWEService::goInRecoverableError(SWEService *impl, const std::string & impl_name,  const std::string & domain_string) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domain_string);
        if(impl->serviceState != CUStateKey::RECOVERABLE_ERROR &&
           impl->serviceState != CUStateKey::FATAL_ERROR ) {
            //keep track of last state
            impl->last_state = impl->state_machine.current_state()[0];
        }
        
        DEBUG_CODE(SWE_LDBG  << "Going into recoverable state for " << impl_name;)
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::fatal_error())  == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0];
            impl->recoverableErrorFromState(impl->last_state);
        }else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        SWE_LDBG  << impl_name << " into recoverable state";
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
    bool result = true;
    bool change_state_result = false;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domain_string);
        DEBUG_CODE(SWE_LDBG  << "Try to recover error " << impl_name;)
        if(impl->SWEService::state_machine.process_event(service_state_machine::EventType::fatal_error())  == boost::msm::back::HANDLED_TRUE) {
            impl->serviceState = impl->state_machine.current_state()[0];
            //call handler for infor the we are going to the last state
            if(impl->beforeRecoverErrorFromState(impl->last_state)) {
                //we can go to the last error
                switch (impl->last_state) {
                    case CUStateKey::INIT : {
                        impl->init(NULL);
                        break;
                    }
                        
                        
                    case CUStateKey::START:{
                        impl->start();
                        break;
                    }
                        
                    case CUStateKey::STOP:{
                        change_state_result = (impl->SWEService::state_machine.process_event(service_state_machine::EventType::stop())  == boost::msm::back::HANDLED_TRUE);
                        break;
                    }
                    default:
                        break;
                }
                
                impl->recoveredToState(impl->last_state);
            }
            
            impl->last_state = -1;
        }else {
            DEBUG_CODE(LOG_AND_TROW_FORMATTED(SWE_LDBG, -1, "Service cant flow to next state and current is -> %1%" , %impl->state_machine.current_state()[0]))
        }
        DEBUG_CODE(SWE_LDBG  << impl_name << " error has been recovered";)
    } catch (CException& ex) {
        SWE_LAPP  << "Error recovering error for " << impl_name << " with "<< ex.what();
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_function_call> >& ex){
        SWE_LAPP  << "Error recovering fatal error for " << impl_name << " with "<< ex.what();
    } catch(...) {
        SWE_LAPP  << "Error recovering fatal error for " << impl_name << " with general exception";
    }
    return result;
}
