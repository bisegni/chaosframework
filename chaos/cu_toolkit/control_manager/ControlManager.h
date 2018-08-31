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

#include <chaos/cu_toolkit/control_manager/ControlUnitApiInterface.h>
#include <chaos/cu_toolkit/control_manager/WorkUnitManagement.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/control_manager/control_manager_constants.h>
#include <chaos/cu_toolkit/control_manager/execution_pool/ExecutionPool.h>

#include <boost/thread.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>


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
                
                //!unit server variable
				std::string				unit_server_alias;
				std::string				unit_server_key;
				boost::shared_mutex		unit_server_sm_mutex;
				UnitServerStateMachine	unit_server_sm;
                
                //!execution pool managment root class
                common::utility::InizializableServiceContainer<chaos::cu::control_manager::execution_pool::ExecutionPoolManager> exectuion_pool_manager;
				
				bool thread_run;
				chaos::WaitSemaphore thread_waith_semaphore;
				boost::scoped_ptr<boost::thread> thread_registration;
				
				//! metadata server channel for control unit registration
				chaos::common::message::MDSMessageChannel *mds_channel;
				
				//!queue for control unit waiting to be published
				mutable boost::shared_mutex		mutex_queue_submitted_cu;
				queue< ChaosSharedPtr<AbstractControlUnit> >	queue_submitted_cu;
				
				//! control unit instance mapped with their unique identifier
				mutable boost::shared_mutex mutex_map_cuid_reg_unreg_instance;
                map<string, ChaosSharedPtr<WorkUnitManagement> > map_cuid_reg_unreg_instance;
				
				//map
				mutable boost::shared_mutex mutex_map_cuid_registered_instance;
				map<string, ChaosSharedPtr<WorkUnitManagement> > map_cuid_registered_instance;
				
				//! association by alias and control unit instancer
				typedef std::map<string, ChaosSharedPtr<CUObjectInstancer> >::iterator MapCUAliasInstancerIterator;
				mutable boost::shared_mutex mutex_map_cu_instancer;
				std::map<string, ChaosSharedPtr<CUObjectInstancer> > map_cu_alias_instancer;
				
                //!hnalder thre proxy creation
                ProxyLoadHandler load_handler;
                
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
                chaos::common::data::CDWUniquePtr unitServerStatus(chaos::common::data::CDWUniquePtr message_data);
				
				/*!
				 Action that show the unit server registration result(success or failure)
				 */
				chaos::common::data::CDWUniquePtr unitServerRegistrationACK(chaos::common::data::CDWUniquePtr message_data);
				
				/*!
				 Action that get the work unit registration(success or failure)
				 */
				chaos::common::data::CDWUniquePtr workUnitRegistrationACK(chaos::common::data::CDWUniquePtr message_data);
				
				/*!
				 Action for loading a control unit
				 */
				chaos::common::data::CDWUniquePtr loadControlUnit(chaos::common::data::CDWUniquePtr message_data);
				
				/*!
				 Action for the unloading of a control unit
				 The unload operation, check that the target control unit is in deinit state
				 */
				chaos::common::data::CDWUniquePtr unloadControlUnit(chaos::common::data::CDWUniquePtr message_data);
				
				/*!
				 Configure the sandbox and all subtree of the CU
				 */
				chaos::common::data::CDWUniquePtr updateConfiguration(chaos::common::data::CDWUniquePtr  message_data);
				
                /*
                 Submit a new Control unit instance
                 */
                void submitControlUnit(ChaosSharedPtr<AbstractControlUnit> control_unit_instance) throw(CException);
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
				
                //! allota a new control unit proxy
                void setProxyCreationHandler(ProxyLoadHandler load_handler);
                
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
															ChaosSharedPtr<CUObjectInstancer>(ALLOCATE_INSTANCER_P3(ControlUnitClass,										//Control unit implementation
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
