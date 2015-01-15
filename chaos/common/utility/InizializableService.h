/*
 *	InizializableService.h
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

#ifndef __CHAOSFramework__InizializableService__
#define __CHAOSFramework__InizializableService__

#include <inttypes.h>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/CException.h>




namespace chaos {
	namespace common {
		namespace utility {
			
			namespace service_state_machine {
				
				//SM Event
				namespace EventType {
					struct initialize {};
					struct deinitialize {};
				}
				
				// The list of FSM states
				struct Deinitilized : public boost::msm::front::state<>{};
				
				struct Initialized : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct id_states_machine : public boost::msm::front::state_machine_def<id_states_machine> {
					
					// the initial state of the player SM. Must be defined
					typedef Deinitilized initial_state;
					
					typedef boost::msm::front::Row <  Deinitilized   ,  EventType::initialize  , Initialized, boost::msm::front::none , boost::msm::front::none > deinit_init_row;
					typedef boost::msm::front::Row <  Initialized   ,  EventType::deinitialize  , Deinitilized, boost::msm::front::none , boost::msm::front::none > init_deinit_row;
					
					// Transition table for initialization services
					struct transition_table : boost::mpl::vector<
					deinit_init_row,
					init_deinit_row > {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int )
					{
					  //	std::cout << "no trnasition";
					}
				};
			}
			
			class InizializableService {
				boost::msm::back::state_machine< service_state_machine::id_states_machine > state_machine;
			protected:
				uint8_t serviceState;
				
				//! Initialize instance
				virtual void init(void*) throw(chaos::CException);
				
				//! Deinit the implementation
				virtual void deinit() throw(chaos::CException);
				
			public:
				InizializableService();
				virtual ~InizializableService();
				//! Return the state
				const uint8_t getServiceState() const;
				
				
				static bool initImplementation(InizializableService& impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(InizializableService& impl, const std::string & implName,  const std::string & domainString);
				
				static bool initImplementation(InizializableService *impl, void *initData, const std::string & implName,  const std::string & domainString);
				static bool deinitImplementation(InizializableService *impl, const std::string & implName,  const std::string & domainString);
			};
			
			template<typename T>
			class InizializableServiceContainer {
				bool delete_on_dispose;
				std::string service_name;
				T *startable_service_instance;
			public:
				InizializableServiceContainer():
				delete_on_dispose(true),
				startable_service_instance(NULL){}
				
				InizializableServiceContainer(bool _delete_on_dispose,
											  const std::string & instance_name):
				delete_on_dispose(_delete_on_dispose),
				startable_service_instance(new T()),
				service_name(instance_name) {}
				
				InizializableServiceContainer(T *instance,
											  bool _delete_on_dispose,
											  const std::string & instance_name):
				startable_service_instance(instance),
				delete_on_dispose(_delete_on_dispose),
				service_name(instance_name) {}
				
				~InizializableServiceContainer() {
					if(delete_on_dispose) delete(startable_service_instance);
				}
				
				bool init(void *init_data, const std::string & domainString) {
					return InizializableService::initImplementation(startable_service_instance, init_data, service_name, domainString);
				}
				
				bool deinit(const std::string & domainString) {
					return InizializableService::deinitImplementation(startable_service_instance, service_name, domainString);
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

#endif /* defined(__CHAOSFramework__InizializableService__) */
