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

#ifndef __CHAOSFramework__StartableService__
#define __CHAOSFramework__StartableService__

#include <chaos/common/utility/InizializableService.h>

namespace chaos {
	namespace common {
		namespace utility {
			
			namespace service_state_machine {
				//SM Event
				namespace EventType {
					struct start {};
					struct stop {};
				}
				
				// States
				struct Start : public boost::msm::front::state<> {};
				struct Stop : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct ss_state_machine : public boost::msm::front::state_machine_def<ss_state_machine>  {
					
					
					typedef boost::msm::front::Row <  service_state_machine::Deinit ,  EventType::init    , service_state_machine::Init   , boost::msm::front::none , boost::msm::front::none > deinit_init_row;
					typedef boost::msm::front::Row <  service_state_machine::Init   ,  EventType::deinit  , service_state_machine::Deinit , boost::msm::front::none , boost::msm::front::none > init_deinit_row;
					typedef boost::msm::front::Row <  service_state_machine::Init   ,  EventType::start   , service_state_machine::Start  , boost::msm::front::none , boost::msm::front::none > init_start_row;
					typedef boost::msm::front::Row <  service_state_machine::Start  ,  EventType::stop    , service_state_machine::Stop   , boost::msm::front::none , boost::msm::front::none > start_stop_row;
					typedef boost::msm::front::Row <  service_state_machine::Stop   ,  EventType::start   , service_state_machine::Start  , boost::msm::front::none , boost::msm::front::none > stop_start_row;
					typedef boost::msm::front::Row <  service_state_machine::Stop   ,  EventType::deinit  , service_state_machine::Deinit , boost::msm::front::none , boost::msm::front::none > stop_deinit_row;
					
					// the initial state of the player SM. Must be defined
					typedef Deinit initial_state;
					
					// Transition table for Startable services and his subclass
					struct transition_table : boost::mpl::vector< deinit_init_row, init_deinit_row, init_start_row, start_stop_row, stop_start_row, stop_deinit_row > {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int ) {}
				};
			}
			
			class StartableService : public InizializableService {
				boost::msm::back::state_machine< service_state_machine::ss_state_machine > state_machine;
			protected:
				
				//! Start the implementation
				virtual void start() throw(chaos::CException);
				
				//! Stop the implementation
				virtual void stop() throw(chaos::CException);
				
			public:
				StartableService();
				virtual ~StartableService();
				
				
				static bool startImplementation(StartableService& impl, const std::string & implName,  const std::string & domainString);
				static bool stopImplementation(StartableService& impl, const std::string & implName,  const std::string & domainString);
				
				static bool startImplementation(StartableService *impl, const std::string & implName,  const std::string & domainString);
				static bool stopImplementation(StartableService *impl, const std::string & implName,  const std::string & domainString);
				
				static bool initImplementation(StartableService& impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(StartableService& impl, const std::string & implName,  const std::string & domainString);
				
				static bool initImplementation(StartableService *impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(StartableService *impl, const std::string & implName,  const std::string & domainString);
			};
			
			template<typename T>
			class StartableServiceContainer {
				bool delete_on_dispose;
				std::string service_name;
				T *startable_service_instance;
			public:
				StartableServiceContainer():
				delete_on_dispose(true),
				startable_service_instance(NULL) {}
				StartableServiceContainer(bool _delete_on_dispose,
										  const std::string & instance_name):
				delete_on_dispose(_delete_on_dispose),
				startable_service_instance(new T()),
				service_name(instance_name) {}
				StartableServiceContainer(T *instance,
										  bool _delete_on_dispose,
										  const std::string & instance_name):
				startable_service_instance(instance),
				delete_on_dispose(_delete_on_dispose),
				service_name(instance_name) {}
				~StartableServiceContainer() {
					if(delete_on_dispose) delete(startable_service_instance);
				}
				
				bool init(void *init_data, const std::string & domainString) {
					return StartableService::initImplementation(startable_service_instance, init_data, service_name, domainString);
				}
				
				bool start(const std::string & domainString) {
					return StartableService::startImplementation(startable_service_instance, service_name, domainString);
				}
				
				bool stop(const std::string & domainString) {
					return StartableService::stopImplementation(startable_service_instance, service_name, domainString);
				}
				
				bool deinit(const std::string & domainString) {
					return StartableService::deinitImplementation(startable_service_instance, service_name, domainString);
				}
				
				bool isInstantiated() {
					return startable_service_instance != NULL;
				}
				
				T* get() {
					return startable_service_instance;
				}
				
				void reset(T *new_instance, const std::string & instance_name) {
					if(startable_service_instance) {
						delete startable_service_instance;
					}
					startable_service_instance = new_instance;
					service_name = instance_name;
				}
				
				T& operator*() {
					return *startable_service_instance;
				}
				
				T* operator->() {
					return startable_service_instance;
				}
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__StartableService__) */
