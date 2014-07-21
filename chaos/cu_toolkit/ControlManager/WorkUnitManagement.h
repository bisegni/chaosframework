/*
 *	WorkUnitManagement.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__WorkUnitManagement__
#define __CHAOSFramework__WorkUnitManagement__

#include <chaos/common/data/CDataWrapper.h>

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

#include <boost/thread.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>
#include <chaos/common/message/MDSMessageChannel.h>

using namespace chaos::common::data;

namespace chaos {
	namespace cu {
		namespace control_manager {
			namespace work_unit_state_machine {
				//Work unit event type
				namespace UnitEventType {
					// unit server need to register hiself to the mds
					struct UnitEventTypeUnpublished {};
					// unit server is registering and he's waiting for approval
					struct UnitEventTypePublishing {};
					// unti server is registered
					struct UnitEventTypePublished {};
					// unit server unpublishing event
					struct UnitEventTypeUnpublishing {};
					// unti server is failed to register
					struct UnitEventTypeFailure {};
				}
				
				//! work unit is not published
				struct Unpublished : public boost::msm::front::state<>{};
				//! work unit is publishing to the mds service
				struct Publishing : public boost::msm::front::state<> {};
				//! work unit has been published, and his heartbeat start to pulse
				struct Published : public boost::msm::front::state<> {};
				//! unpublishing event is given and now work unit start te unpublishing process
				struct Unpublishing : public boost::msm::front::state<> {};
				//! there was been some failure during publishing operation
				struct PublishingFailure  : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct wu_state_machine : public boost::msm::front::state_machine_def<wu_state_machine> {
					
					// the initial state of the player SM. Must be defined
					typedef Unpublished initial_state;
					
					typedef boost::msm::front::Row <  Unpublished   ,  UnitEventType::UnitEventTypePublishing  , Publishing, boost::msm::front::none , boost::msm::front::none > unpub_pubing_row;
					typedef boost::msm::front::Row <  Publishing   ,  UnitEventType::UnitEventTypePublished  , Published, boost::msm::front::none , boost::msm::front::none > pubing_pub_row;
					typedef boost::msm::front::Row <  Publishing   ,  UnitEventType::UnitEventTypeFailure  , PublishingFailure, boost::msm::front::none , boost::msm::front::none > pubbing_pubfail_row;
					typedef boost::msm::front::Row <  Published   ,  UnitEventType::UnitEventTypeUnpublishing  , Unpublishing, boost::msm::front::none , boost::msm::front::none > pub_unpubing_row;
					typedef boost::msm::front::Row <  Unpublishing   ,  UnitEventType::UnitEventTypeUnpublished  , Unpublished, boost::msm::front::none , boost::msm::front::none > unpubing_unpub_row;
					
					// Transition table for initialization services
					struct transition_table : boost::mpl::vector<
					unpub_pubing_row,
					pubing_pub_row,
					pubbing_pubfail_row,
					pub_unpubing_row,
					unpubing_unpub_row> {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int ){}
				};
			}
			
			
			//! Unit event state enum definition used on WorkUnitManagment class method
			typedef enum UnitState {
				UnitStateUnpublished,
				UnitStatePublishing,
				UnitStatePublished,
				UnitStatePublishingFailure,
				UnitStateUnpublishing
			} UnitState;
			
			//! type definition for state machine
			typedef boost::msm::back::state_machine< work_unit_state_machine::wu_state_machine >	WorkUnitStateMachine;
			
			//! struct for the managment fo single control unit state machine
			/*!
			 Every control unit has his individual state machine taht permit to regulate the
			 process of publishing and online maintenance
			 */
			class WorkUnitManagement {
				//! state machine for the control unit instance
				WorkUnitStateMachine					wu_instance_sm;
				
				int sendConfPackToMDS(CDataWrapper& dataToSend);
			public:
				MDSMessageChannel						*mds_channel;
				//! abstract contro unit isntalce
				boost::shared_ptr<AbstractControlUnit>	work_unit_instance;

				//! default constructor adn destructor
				WorkUnitManagement(AbstractControlUnit *_work_unit_instance):work_unit_instance(_work_unit_instance){}
				~WorkUnitManagement(){}
				
				//! return the state of the unit state machine
				UnitState getCurrentState();
				
				//! manage the internal state machine
				void scheduleSM() throw (CException);

				void manageACKPack(CDataWrapper ackPack);
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__WorkUnitManagement__) */
