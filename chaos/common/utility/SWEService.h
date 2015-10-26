/*
 *	SWEService.h
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

#ifndef __CHAOSFramework__SWEService_h
#define __CHAOSFramework__SWEService_h

#include <chaos/common/utility/StartableService.h>

namespace chaos {
    namespace common {
        namespace utility {
            namespace service_state_machine {
                //SM Event
                namespace EventType {
                    struct recoverable_error {};
                    struct fatal_error {};
                }
                
                // States
                struct RecoverableError : public boost::msm::front::state<> {};
                struct FatalError : public boost::msm::front::state<> {};
                
                // front-end: define the FSM structure
                struct swe_state_machine:
                public boost::msm::front::state_machine_def<swe_state_machine>  {
                    
                    typedef boost::msm::front::Row <  service_state_machine::Deinit ,  EventType::init    , service_state_machine::Init   , boost::msm::front::none , boost::msm::front::none > deinit_init_row;
                    typedef boost::msm::front::Row <  service_state_machine::Init   ,  EventType::deinit  , service_state_machine::Deinit , boost::msm::front::none , boost::msm::front::none > init_deinit_row;
                    typedef boost::msm::front::Row <  service_state_machine::Init   ,  EventType::start   , service_state_machine::Start  , boost::msm::front::none , boost::msm::front::none > init_start_row;
                    typedef boost::msm::front::Row <  service_state_machine::Start  ,  EventType::stop    , service_state_machine::Stop   , boost::msm::front::none , boost::msm::front::none > start_stop_row;
                    typedef boost::msm::front::Row <  service_state_machine::Stop   ,  EventType::start   , service_state_machine::Start  , boost::msm::front::none , boost::msm::front::none > stop_start_row;
                    typedef boost::msm::front::Row <  service_state_machine::Stop   ,  EventType::deinit  , service_state_machine::Deinit , boost::msm::front::none , boost::msm::front::none > stop_deinit_row;
 
                    //-------------recoverable error states--------------
                    typedef boost::msm::front::Row <  service_state_machine::Init   ,  EventType::recoverable_error  , service_state_machine::RecoverableError  , boost::msm::front::none , boost::msm::front::none > init_re_row;
                    typedef boost::msm::front::Row <  service_state_machine::Start  ,  EventType::recoverable_error  , service_state_machine::RecoverableError  , boost::msm::front::none , boost::msm::front::none > start_re_row;
                    typedef boost::msm::front::Row <  service_state_machine::Stop   ,  EventType::recoverable_error  , service_state_machine::RecoverableError  , boost::msm::front::none , boost::msm::front::none > stop_re_row;
                    typedef boost::msm::front::Row <  service_state_machine::Deinit ,  EventType::recoverable_error  , service_state_machine::RecoverableError  , boost::msm::front::none , boost::msm::front::none > deinit_re_row;
                    
                    typedef boost::msm::front::Row <  service_state_machine::RecoverableError ,  EventType::init  , service_state_machine::Init  , boost::msm::front::none , boost::msm::front::none > re_init_row;
                    typedef boost::msm::front::Row <  service_state_machine::RecoverableError ,  EventType::start  , service_state_machine::Start  , boost::msm::front::none , boost::msm::front::none > re_start_row;
                    typedef boost::msm::front::Row <  service_state_machine::RecoverableError ,  EventType::stop  , service_state_machine::Stop  , boost::msm::front::none , boost::msm::front::none > re_stop_row;
                    typedef boost::msm::front::Row <  service_state_machine::RecoverableError ,  EventType::deinit  , service_state_machine::Deinit  , boost::msm::front::none , boost::msm::front::none > re_deinit_row;
                   
                    //----------------fatal error states-----------------
                    typedef boost::msm::front::Row <  service_state_machine::Init       ,  EventType::fatal_error   , service_state_machine::FatalError    , boost::msm::front::none , boost::msm::front::none > init_fe_row;
                    typedef boost::msm::front::Row <  service_state_machine::Start      ,  EventType::fatal_error   , service_state_machine::FatalError    , boost::msm::front::none , boost::msm::front::none > start_fe_row;
                    typedef boost::msm::front::Row <  service_state_machine::Stop       ,  EventType::fatal_error   , service_state_machine::FatalError    , boost::msm::front::none , boost::msm::front::none > stop_fe_row;
                    typedef boost::msm::front::Row <  service_state_machine::Deinit     ,  EventType::fatal_error   , service_state_machine::FatalError    , boost::msm::front::none , boost::msm::front::none > deinit_fe_row;
                    typedef boost::msm::front::Row <  service_state_machine::FatalError ,  EventType::init          , service_state_machine::Init          , boost::msm::front::none , boost::msm::front::none > fe_init_row;
                    // the initial state of the player SM. Must be defined
                    typedef Deinit initial_state;
                    
                    // Transition table for Startable services and his subclass
                    struct transition_table : boost::mpl::vector<
                    deinit_init_row,
                    init_deinit_row,
                    init_start_row,
                    start_stop_row,
                    stop_start_row,
                    stop_deinit_row,
                    init_re_row,
                    start_re_row,
                    stop_re_row,
                    deinit_re_row,
                    re_init_row,
                    re_start_row,
                    re_stop_row,
                    re_deinit_row,
                    init_fe_row,
                    start_fe_row,
                    stop_fe_row,
                    deinit_fe_row,
                    fe_init_row> {};
                    
                    template <class FSM,class Event>
                    void no_transition(Event const& ,FSM&, int ) {}
                };
            }
            
            class SWEService : public StartableService {
                int last_state;
                boost::msm::back::state_machine< service_state_machine::swe_state_machine > state_machine;
            protected:
                
                //! State machine is gone into recoverable error
                virtual void recoverableErrorFromState(int last_state, chaos::CException& ex) = 0;
                
                //! State machine is gone into recoverable error
                /*!
                 Retunr true if we can rool back to previous state
                 */
                virtual bool beforeRecoverErrorFromState(int last_state) = 0;
                
                //! State machine is gone into recoverable error
                /*!
                 Retunr true if we can rool back to previous state
                 */
                virtual void recoveredToState(int last_state) = 0;
                
                //! State machine is gone into an unrecoverable error
                virtual void fatalErrorFromState(int last_state, chaos::CException& ex ) = 0;
                
            public:
                SWEService();
                virtual ~SWEService();
                
                
                static bool startImplementation(SWEService& impl, const std::string & impl_name,  const std::string & domain_string);
                static bool startImplementation(SWEService *impl, const std::string & impl_name,  const std::string & domain_string);
                
                static bool stopImplementation(SWEService& impl, const std::string & impl_name,  const std::string & domain_string);
                static bool stopImplementation(SWEService *impl, const std::string & impl_name,  const std::string & domain_string);
                
                static bool initImplementation(SWEService& impl, void *initData, const std::string & impl_name,  const std::string & domain_string);
                static bool initImplementation(SWEService *impl, void *initData, const std::string & impl_name,  const std::string & domain_string);
                
                static bool deinitImplementation(SWEService& impl, const std::string & impl_name,  const std::string & domain_string);
                static bool deinitImplementation(SWEService *impl, const std::string & impl_name,  const std::string & domain_string);
                
                static bool goInFatalError(SWEService& impl, chaos::CException& ex, const std::string & impl_name,  const std::string & domain_string);
                static bool goInFatalError(SWEService *impl, chaos::CException& ex,  const std::string & impl_name,  const std::string & domain_string);
                
                static bool goInRecoverableError(SWEService& impl, chaos::CException& ex, const std::string & impl_name,  const std::string & domain_string);
                static bool goInRecoverableError(SWEService *impl, chaos::CException& ex,  const std::string & impl_name,  const std::string & domain_string);
                
                static bool recoverError(SWEService& impl, const std::string & impl_name,  const std::string & domain_string);
                static bool recoverError(SWEService *impl, const std::string & impl_name,  const std::string & domain_string);
            };
            
        }
    }
}

#endif /* SWEService_h */
