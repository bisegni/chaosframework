/*
 *	ControlManager.h
 *	!CHAOS
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
#ifndef ChaosFramework_ControlManager_h
#define ChaosFramework_ControlManager_h

#include <queue>
#include <vector>
#include <map>

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos/cu_toolkit/control_manager/WorkUnitManagement.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/control_manager/execution_pool/ExecutionPool.h>

#include <boost/thread.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>

//startup parameter for unit server
#define CONTROL_MANAGER_UNIT_SERVER_ENABLE							"unit-server-enable"
#define CONTROL_MANAGER_UNIT_SERVER_ENABLE_desc						"Enable the unit server on this instance of process"

#define CONTROL_MANAGER_UNIT_SERVER_ALIAS							"unit-server-alias"
#define CONTROL_MANAGER_UNIT_SERVER_ALIAS_desc						"Alias used to publish the unit server"
#define CONTROL_MANAGER_UNIT_SERVER_KEY								"unit-server-file-key"
#define CONTROL_MANAGER_UNIT_SERVER_KEY_desc						"the path to the file that contains the rsa public key for the unit server alias"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC			"unit-server-retry-ms"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_desc	"Delay in milliseconds for the registration retry"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_DEFAULT	2000


//define the type for the Control Unit instancer

namespace chaos {
    namespace cu {
		namespace control_manager {


#define REGISTER_CU(_cu_)						\
		  chaos::cu::ChaosCUToolkit::getInstance()->registerControlUnit< _cu_ >();

			typedef chaos::common::utility::ObjectInstancerP3<AbstractControlUnit,
			const std::string&,
			const std::string&,
			const AbstractControlUnit::ControlUnitDriverList&> CUObjectInstancer;
			
			
			namespace unit_server_state_machine {
				
				//Unit server state machine
				namespace UnitServerEventType {
					// unit server need to register hiself to the mds
					struct UnitServerEventTypeUnpublished {};
					// unit server is registering and he's waiting for approval
					struct UnitServerEventTypePublishing {};
					// unti server is registered
					struct UnitServerEventTypePublished {};
					// unit server is registering and he's waiting for approval
					struct UnitServerEventTypeFailure {};
				}
				
				// Unit Server state
				struct Unpublished : public boost::msm::front::state<>{};
				
				struct Publishing : public boost::msm::front::state<> {};
				
				struct Published : public boost::msm::front::state<> {};
				
				struct PublishingFailure  : public boost::msm::front::state<> {};
				
				// front-end: define the FSM structure
				struct us_state_machine : public boost::msm::front::state_machine_def<us_state_machine> {
					
					// the initial state of the player SM. Must be defined
					typedef Unpublished initial_state;
					
					typedef boost::msm::front::Row <  Unpublished   ,  UnitServerEventType::UnitServerEventTypePublishing  , Publishing, boost::msm::front::none , boost::msm::front::none > unpub_pubing_row;
					typedef boost::msm::front::Row <  Publishing   ,  UnitServerEventType::UnitServerEventTypePublished  , Published, boost::msm::front::none , boost::msm::front::none > pubing_pub_row;
					typedef boost::msm::front::Row <  Publishing   ,  UnitServerEventType::UnitServerEventTypeFailure  , PublishingFailure, boost::msm::front::none , boost::msm::front::none > pubbing_pubfail_row;
					typedef boost::msm::front::Row <  Published   ,  UnitServerEventType::UnitServerEventTypeUnpublished  , Unpublished, boost::msm::front::none , boost::msm::front::none > pub_unpub_row;
					
					// Transition table for initialization services
					struct transition_table : boost::mpl::vector<
					unpub_pubing_row,
					pubing_pub_row,
					pubbing_pubfail_row,
					pub_unpub_row> {};
					
					template <class FSM,class Event>
					void no_transition(Event const& ,FSM&, int ){}
				};
			}
			//type definition of the state machine
			typedef boost::msm::back::state_machine< unit_server_state_machine::us_state_machine >	UnitServerStateMachine;
			
			typedef boost::upgrade_lock<boost::shared_mutex>			UpgradeableLock;
			typedef boost::upgrade_to_unique_lock<boost::shared_mutex>	UpgradeReadToWriteLock;
			typedef boost::shared_lock<boost::shared_mutex>				ReadLock;
			typedef boost::unique_lock<boost::shared_mutex>				WriteLock;

			
			//!Manager for the Control Unit managment
			/*!
			 This class is the central point qhere control unit are managed. Two api are exposed
			 via RPC to load and unload the control unit giving some parameter.
			 */
			class ControlManager :
			public DeclareAction,
			public common::utility::StartableService,
			protected chaos::common::async_central::TimerHandler,
			public common::utility::Singleton<ControlManager> {
			  friend class common::utility::Singleton<ControlManager>;
				//mutable boost::shared_mutex mutex_registration;
				
				//unit server state machine
				bool					use_unit_server;
                bool                    use_execution_pools;
                unsigned int            publishing_counter_delay;
				std::string				unit_server_alias;
				std::string				unit_server_key;
				boost::shared_mutex		unit_server_sm_mutex;
				UnitServerStateMachine	unit_server_sm;
				
				bool thread_run;
				chaos::WaitSemaphore thread_waith_semaphore;
				boost::scoped_ptr<boost::thread> thread_registration;
				
				//! metadata server channel for control unit registration
				chaos::common::message::MDSMessageChannel *mds_channel;
				
				//!queue for control unit waiting to be published
				mutable boost::shared_mutex		mutex_queue_submitted_cu;
				queue< AbstractControlUnit* >	queue_submitted_cu;
				
				//! control unit instance mapped with their unique identifier
				mutable boost::shared_mutex mutex_map_cuid_reg_unreg_instance;
                map<string, boost::shared_ptr<WorkUnitManagement> > map_cuid_reg_unreg_instance;
				
				//map
				mutable boost::shared_mutex mutex_map_cuid_registered_instance;
				map<string, boost::shared_ptr<WorkUnitManagement> > map_cuid_registered_instance;
				
				//! association by alias and control unit instancer
				typedef std::map<string, boost::shared_ptr<CUObjectInstancer> >::iterator MapCUAliasInstancerIterator;
				mutable boost::shared_mutex mutex_map_cu_instancer;
				std::map<string, boost::shared_ptr<CUObjectInstancer> > map_cu_alias_instancer;
				
				/*
				 Constructor
				 */
				ControlManager();
				
				/*
				 Desctructor
				 */
				~ControlManager();
				
				void _loadWorkUnit();
				void _unloadControlUnit();
				//!prepare and send registration pack to the metadata server
				void sendUnitServerRegistration();
				
				//! manage transistion betwheen map_cuid_registering_instance and map_cuid_registered_instance
				/*!
				 checn in the registering map al the instance that has state machine in a stable state
				 */
				inline void migrateStableAndUnstableSMCUInstance();
				
				//! Make one steps in SM for all registring state machine
				inline void makeSMSteps();
			protected:
				//! timer fire method
				void manageControlUnit();
				
				//TimerHandler callback
				void timeout();
				
				//! start control units state machine thread
				void startControlUnitSMThread();
				
				//! stop control unit state machien thread
				void stopControlUnitSMThread(bool whait = true);
				
				
                /*!
				 Action to poll status of a UnitServer and all CUs
				 */
				CDataWrapper* unitServerStatus(CDataWrapper *message_data, bool &detach) throw (CException);
				
				/*!
				 Action that show the unit server registration result(success or failure)
				 */
				CDataWrapper* unitServerRegistrationACK(CDataWrapper *message_data, bool &detach) throw (CException);
				
				/*!
				 Action that get the work unit registration(success or failure)
				 */
				CDataWrapper* workUnitRegistrationACK(CDataWrapper *message_data, bool &detach) throw (CException);
				
				/*!
				 Action for loading a control unit
				 */
				CDataWrapper* loadControlUnit(CDataWrapper *message_data, bool &detach) throw (CException);
				
				/*!
				 Action for the unloading of a control unit
				 The unload operation, check that the target control unit is in deinit state
				 */
				CDataWrapper* unloadControlUnit(CDataWrapper *message_data, bool &detach) throw (CException);
				
				/*!
				 Configure the sandbox and all subtree of the CU
				 */
				CDataWrapper* updateConfiguration(CDataWrapper  *message_data, bool& detach);
				
			public:
				
				/*
				 Initialize the Control Manager
				 */
				void init(void *initParameter) throw(CException);
				
				/*
				 Start the Control Manager
				 */
				void start() throw(CException);
				
				
				/*!
				 Stop the control manager
				 */
				void stop() throw(CException);
				
				/*
				 Deinitialize the Control Manager
				 */
				void deinit() throw(CException);
				
				/*
				 Submit a new Control unit instance
				 */
				void submitControlUnit(AbstractControlUnit *control_unit_instance) throw(CException);
				
				//! control unit registration
				/*!
				 Register a control unit instancer associating it to an alias
				 \param control_unit_alias the alias associated to the template class identification
				 */
				template<typename ControlUnitClass>
				void registerControlUnit() {
					//lock the map with the instancer
					WriteLock write_instancer_lock(mutex_map_cu_instancer);
					
					map_cu_alias_instancer.insert(make_pair(CONTROL_UNIT_PUBLISH_NAME(ControlUnitClass),
															boost::shared_ptr<CUObjectInstancer>(ALLOCATE_INSTANCER_P3(ControlUnitClass,										//Control unit implementation
																													   AbstractControlUnit,										//Control unit base class
																													   const std::string&,										//Control Unit unique id param
																													   const std::string&,										//control unit load param
																													   const AbstractControlUnit::ControlUnitDriverList&))));	//Control unit driver list
				}
			};
		}
	}
}
#endif
