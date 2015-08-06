/*
 *	LoadableService.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__LoadableService__
#define __CHAOSFramework__LoadableService__

#include <chaos/common/utility/StartableService.h>

namespace chaos {
	namespace common {
		namespace utility {
			
			namespace service_state_machine {
				//SM Event
				namespace EventType {
					struct load {};
					struct unload {};
				}
				
				// States
				struct Loaded : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct ls_state_machine : public boost::msm::front::state_machine_def<ls_state_machine>  {
					
					
					typedef boost::msm::front::Row <  service_state_machine::Deinitilized  ,  EventType::initialize    , service_state_machine::Initialized   , boost::msm::front::none , boost::msm::front::none > deinit_init_row;
					typedef boost::msm::front::Row <  service_state_machine::Initialized   ,  EventType::deinitialize  , service_state_machine::Deinitilized  , boost::msm::front::none , boost::msm::front::none > init_deinit_row;
					typedef boost::msm::front::Row <  service_state_machine::Initialized   ,  EventType::start         , service_state_machine::Started       , boost::msm::front::none , boost::msm::front::none > init_start_row;
					typedef boost::msm::front::Row <  service_state_machine::Started       ,  EventType::stop          , service_state_machine::Stopped       , boost::msm::front::none , boost::msm::front::none > start_stop_row;
					typedef boost::msm::front::Row <  service_state_machine::Stopped       ,  EventType::start         , service_state_machine::Started       , boost::msm::front::none , boost::msm::front::none > stop_start_row;
					typedef boost::msm::front::Row <  service_state_machine::Stopped       ,  EventType::deinitialize  , service_state_machine::Deinitilized  , boost::msm::front::none , boost::msm::front::none > stop_deinit_row;
					typedef boost::msm::front::Row <  service_state_machine::Loaded		   ,  EventType::initialize	   , service_state_machine::Initialized   , boost::msm::front::none , boost::msm::front::none > load_init_row;
					typedef boost::msm::front::Row <  service_state_machine::Deinitilized  ,  EventType::load		   , service_state_machine::Loaded		  , boost::msm::front::none , boost::msm::front::none >  deinit_unload_row;
					
					// the initial state of the player SM. Must be defined
					typedef Loaded initial_state;
					
					// Transition table for Startable services and his subclass
					struct transition_table : boost::mpl::vector<
					deinit_init_row,
					init_deinit_row,
					init_start_row,
					start_stop_row,
					stop_start_row,
					stop_deinit_row,
					load_init_row,
					deinit_unload_row> {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int ) {}
				};
			}
			
			class LoadableService : public StartableService {
				boost::msm::back::state_machine< service_state_machine::ls_state_machine > state_machine;
			protected:
				
				//! Start the implementation
				virtual void load(void *load_data) throw(chaos::CException);
				
				//! Stop the implementation
				virtual void toLoad() throw(chaos::CException);
				
			public:
				LoadableService();
				virtual ~LoadableService();
				
				static bool loadImplementation(LoadableService& impl, void *load_data, const char * const implName,  const char * const domainString);
				static bool toloadImplementation(LoadableService& impl, const char * const implName,  const char * const domainString);
				
				static bool loadImplementation(LoadableService *impl, void *load_data, const char * const implName,  const char * const domainString);
				static bool toloadImplementation(LoadableService *impl, const char * const implName,  const char * const domainString);
				
				static bool startImplementation(LoadableService& impl, const char * const implName,  const char * const domainString);
				static bool stopImplementation(LoadableService& impl, const char * const implName,  const char * const domainString);
				
				static bool startImplementation(LoadableService *impl, const char * const implName,  const char * const domainString);
				static bool stopImplementation(LoadableService *impl, const char * const implName,  const char * const domainString);
				
				static bool initImplementation(LoadableService& impl, void *initData, const char * const implName,  const char * const domainString);
				static bool deinitImplementation(LoadableService& impl, const char * const implName,  const char * const domainString);
				
				static bool initImplementation(LoadableService *impl, void *initData, const char * const implName,  const char * const domainString);
				static bool deinitImplementation(LoadableService *impl, const char * const implName,  const char * const domainString);
			};
			
		}
	}
}
#endif /* defined(__CHAOSFramework__StartableService__) */

