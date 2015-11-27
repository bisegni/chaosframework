/*
 *	ControlUnit.h
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
#ifndef ControlUnit_H
#define ControlUnit_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <chaos/common/exception/CException.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/utility/SWEService.h>
#include <chaos/common/utility/AggregatedCheckList.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/chaos_types.h>

#include <chaos/cu_toolkit/ControlManager/AttributeSharedCacheWrapper.h>
#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>

#ifdef __CHAOS_DEBUG_MEMORY_CU__
#include <chaos/common/debug/tracey.hpp>
#endif

#define CU_OPT_IN_MEMORY_DATABASE "cu-db-in-memory"

#define CONTROL_UNIT_PUBLISH_NAME(impl)\
impl::PublishName

#define PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(impl)\
const char * const  impl::PublishName = #impl;\

#define PUBLISHABLE_CONTROL_UNIT_INTERFACE(impl)\
public:\
static const char * const PublishName;\
private:\
//class impl : public subclass

namespace chaos{
    namespace cu {
        namespace control_manager {
			using namespace chaos::common::data;
			using namespace chaos::common::data::cache;
			using namespace chaos::cu::driver_manager;
			using namespace chaos::cu::driver_manager::driver;
			
            //forward declaration
			class ControlManager;
			class WorkUnitManagement;
            namespace slow_command {
                class SlowCommandExecutor;
            }
            
            
            typedef enum {
                INIT_RPC_PHASE_CALL_INIT_STATE = 0,
                INIT_RPC_PHASE_INIT_SHARED_CACHE,
                INIT_RPC_PHASE_COMPLETE_OUTPUT_ATTRIBUTE,
                INIT_RPC_PHASE_COMPLETE_INPUT_ATTRIBUTE,
                INIT_RPC_PHASE_INIT_SYSTEM_CACHE,
                INIT_RPC_PHASE_CALL_UNIT_DEFINE_ATTRIBUTE,
                INIT_RPC_PHASE_CREATE_FAST_ACCESS_CASCHE_VECTOR,
                INIT_RPC_PHASE_CALL_UNIT_INIT,
                INIT_RPC_PHASE_UPDATE_CONFIGURATION,
                INIT_RPC_PHASE_PUSH_DATASET
            }InitRPCPhase;
            
            typedef enum {
                INIT_SM_PHASE_INIT_DB = 0,
                INIT_SM_PHASE_CREATE_DATA_STORAGE,
            }InitSMPhase;
            
            
            typedef enum {
                START_RPC_PHASE_IMPLEMENTATION = 0,
                START_RPC_PHASE_UNIT
            }StartRPCPhase;
            
            typedef enum {
                START_SM_PHASE_STAT_TIMER = 0
            }StartSMPhase;
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<chaos::common::data::CDataWrapper>, ACUStartupCommandList)
            
            //!  Base class for control unit !CHAOS node
			/*!
			 This is the abstraction of the contorl unit node of CHAOS. This class extends DeclareAction
			 class that permit to publish rpc method for control the control unit life cycle. Most of the API
			 that needs to be used to create device and his dataset are contained into the DeviceSchemaDB class.
			 */
			class AbstractControlUnit :
			public DriverErogatorInterface,
			public DeclareAction,
			protected DatasetDB,
            protected chaos::common::async_central::TimerHandler,
			public common::utility::SWEService {
				//friendly class declaration
				friend class ControlManager;
				friend class WorkUnitManagement;
				friend class DomainActionsScheduler;
				friend class SCAbstractControlUnit;
				friend class RTAbstractControlUnit;
                friend class slow_command::SlowCommandExecutor;
                //enable trace for heap into control unit environment
#ifdef __CHAOS_DEBUG_MEMORY_CU__
                tracey::scope sc;
#endif
			public:
				//! definition of the type for the driver list
				typedef std::vector<DrvRequestInfo>				ControlUnitDriverList;
				typedef std::vector<DrvRequestInfo>::iterator	ControlUnitDriverListIterator;
			private:
				std::string  control_key;
				//! contains the description of the type of the control unit
				std::string control_unit_type;
				
				//! is the unique identification code associated to the control unit instance(rand benerated by contructor)
				std::string control_unit_id;
				
				//! control unit load param
				std::string control_unit_param;
				
                //!these are the startup command list
                /*!
                 The startup command are a set of command that are sent within the load command and
                 are executed after the control unit is completely load. This are enterely managed by ControlManager.
                 */
                ACUStartupCommandList list_startup_command;
                
                //! keep track of how many push has been done for every dataset
                //! 0 - output, 1-input, 2-custom
                uint32_t    push_dataset_counter;
                
                //! identify last timestamp whene the push rate has been acquired;
                uint64_t    last_push_rate_grap_ts;
                
				//! control unit driver information list
				ControlUnitDriverList control_unit_drivers;
				
				//! list of the accessor of the driver requested by the unit implementation
				std::vector< DriverAccessor *> accessorInstances;
				
				//! attributed value shared cache
				/*!
				 The data in every attribute is published automatically on the chaos data service
				 with somne triggering logic according to the domain one.
				 */
				AbstractSharedDomainCache *attribute_value_shared_cache;
				
				//! the wrapper for the user to isolate the shared cache
				AttributeSharedCacheWrapper *attribute_shared_cache_wrapper;
                
                //! fast access for acquisition timestamp
                AttributeValue *timestamp_acq_cached_value;
                
                //! fast access for thread scheduledaly cached value
                AttributeValue *thread_schedule_daly_cached_value;
                
                //! check list of services for initialization and start state
                chaos::common::utility::AggregatedCheckList check_list_sub_service;
                
                //! init configuration
                std::auto_ptr<CDataWrapper> init_configuration;
                
                void _initChecklist();
				
                void doInitRpCheckList() throw(CException);
                void doInitSMCheckList() throw(CException);
                void doStartRpCheckList() throw(CException);
                void doStartSMCheckList() throw(CException);
                void redoInitRpCheckList(bool throw_exception = true) throw(CException);
                void redoInitSMCheckList(bool throw_exception = true) throw(CException);
                void redoStartRpCheckList(bool throw_exception = true) throw(CException);
                void redoStartSMCheckList(bool throw_exception = true) throw(CException);
                
				/*!
				 Initialize the Custom Contro Unit and return the configuration
				 */
				virtual CDataWrapper* _init(CDataWrapper*, bool& detachParam) throw(CException);
				
				/*!
				 Deinit the Control Unit
				 */
				virtual CDataWrapper* _deinit(CDataWrapper*, bool& detachParam) throw(CException);
				
				/*!
				 Starto the  Control Unit scheduling for device
				 */
				virtual CDataWrapper* _start(CDataWrapper*, bool& detachParam) throw(CException);
				
				/*!
				 Stop the Custom Control Unit scheduling for device
				 */
				virtual CDataWrapper* _stop(CDataWrapper*, bool& detachParam) throw(CException);
				
                //!Recover from a recoverable error state
                virtual CDataWrapper* _recover(CDataWrapper *deinitParam, bool& detachParam) throw(CException);
				/*!
				 Restore the control unit to a precise tag
				 */
				virtual CDataWrapper* _unitRestoreToSnapshot(CDataWrapper*, bool& detachParam) throw(CException);
				
				/*!
				 Define the control unit DataSet and Action into
				 a CDataWrapper
				 */
				void _undefineActionAndDataset() throw(CException);
				
				// Startable Service method
				void init(void *initData) throw(CException);
				
				// Startable Service method
				void start() throw(CException);
				
				// Startable Service method
				void stop() throw(CException);
				
				// Startable Service method
				void deinit() throw(CException);
				
                //! State machine is gone into recoverable error
                void recoverableErrorFromState(int last_state, chaos::CException& ex);
                
                //! State machine is gone into recoverable error
                bool beforeRecoverErrorFromState(int last_state);
                
                //! State machine is gone into recoverable error
                void recoveredToState(int last_state);
                
                //! State machine is gone into an unrecoverable error
                void fatalErrorFromState(int last_state, chaos::CException& ex);
                
				//! initialize the dataset attributes (input and output)
				void initAttributeOnSharedAttributeCache(SharedCacheDomain domain,
														std::vector<string>& attribute_names);
				//! complete the output dataset cached with mandatory attribute
				/*!
				 The mandatory attribute like timestamp and triggered id are added after the user defined output aattribute
				 */
				virtual void completeOutputAttribute();

				//! complete the input attribute
				/*!
				 this method can be overloaded by subclass to add 
				 automatically generated input attribute
				 */
				virtual void completeInputAttribute();
				
				//! fill the array with the cached value, the array is used for the fast access of cached data
				void fillCachedValueVector(AttributeCache& attribute_cache,
										   std::vector<AttributeValue*>& cached_value);
				
				//! filel the dataset packet for the cached attribute in the array
				inline void fillCDatawrapperWithCachedValue(std::vector<AttributeValue*>& cached_attributes,
															CDataWrapper& dataset);
				
				void fillRestoreCacheWithDatasetFromTag(data_manager::KeyDataStorageDomain domain,
														CDataWrapper& dataset,
														AbstractSharedDomainCache& restore_cache);
			protected:
				//  It's is the dynamically assigned instance of the CU. it will be used
				// as domain for the rpc action.
				string control_unit_instance;
                
				//! Momentary driver for push data into the central memory
                std::auto_ptr<data_manager::KeyDataStorage>  key_data_storage;
				
				//! fast cached attribute vector accessor
				std::vector<AttributeValue*> cache_output_attribute_vector;
				std::vector<AttributeValue*> cache_input_attribute_vector;
				std::vector<AttributeValue*> cache_custom_attribute_vector;
				std::vector<AttributeValue*> cache_system_attribute_vector;

                //! ofr security reason subclass needs to modifier the access to this method(overloading it and keep private)
                virtual AbstractSharedDomainCache *_getAttributeCache();
                
				//! initialize system attribute
				virtual void initSystemAttributeOnSharedAttributeCache();
				
				//! Define the dataset information of the device implementeted into the CU
				/*!
				 This method configure the CDataWrapper whit all th einromation for describe the implemented device
				 */
				virtual void _defineActionAndDataset(CDataWrapper &setup_configuration) throw(CException);
				
				//! Get all managed declare action instance
				/*!
				 Return all the instance that implements actions to be published into network broker.
				 Subclass can override this method to inclued their action instancer
				 */
				virtual void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
				
                //! Set the dataset input element values
				/*!
				 This method is called when the input attribute of the dataset need to be valorized,
				 subclass need to perform all the appropiate action to set these attribute
				 */
				CDataWrapper* _setDatasetAttribute(CDataWrapper*, bool&) throw (CException);
				
                //! Return the state of the control unit
				/*!
				 Return the current control unit state identifyed by ControlUnitState types
				 fitted into the CDatawrapper with the key CUStateKey::CONTROL_UNIT_STATE
				 */
				CDataWrapper* _getState(CDataWrapper*, bool& detachParam) throw(CException);
				
				//! Return the information about the type of the current instace of control unit
				/*!
				 Return unit fitted into cdata wrapper:
				 CU type: string type associated with the key @CUDefinitionKey::CS_CM_CU_TYPE
				 */
				CDataWrapper* _getInfo(CDataWrapper*, bool& detachParam) throw(CException);
                
                //! update the timestamp attribute of the output datapack
                void _updateAcquistionTimestamp(uint64_t alternative_ts = 0);
                
                void _updateRunScheduleDelay(uint64_t new_scehdule_delay);
                
                //!timer for update push metric
                void _updatePushRateMetric();

                //!put abstract control unit state machine in recoverable error
                /*!
                 This method set the control unit status to "rerror" and valorize the health
                 error keys with the exception values
                 */
                void _goInRecoverableError(chaos::CException recoverable_exception);
                
                //!put abstract control unit state machine in fatal error
                /*!
                 This method set the control unit status to "ferror" and valorize the health
                 error keys with the exception values
                 */
                void _goInFatalError(chaos::CException recoverable_exception);
                
                //! Abstract Method that need to be used by the sublcass to define the dataset
				/*!
				 Subclass, in this method can call the api to create the dataset, after this method
				 this class will collet all the information and send all to the MDS server.
				 */
				virtual void unitDefineActionAndDataset() throw(CException) = 0;
				
				//! Abstract method for the definition of the driver
				/*!
				 This method permit to the control unit implementation to define the necessary drivers it needs
				 \param neededDriver need to be filled with the structure DrvRequestInfo filled with the information
				 about the needed driver [name, version and initialization param if preset statically]
				 */
				virtual void unitDefineDriver(std::vector<DrvRequestInfo>& neededDriver);
				
				//! abstract method to permit to the control unit to define custom attribute
				/*!
				 Custom attirbute, into the shared cache can be accessed globally into the control unit environment
				 they are publishe on every value change commit
				 */
				virtual void unitDefineCustomAttribute();
				
                //! Abstract method for the initialization of the control unit
				/*!
				 This is where the subclass need to be inizialize their environment, usually the hardware initialization. An exception
				 will stop the Control Unit live.
				 */
				virtual void unitInit() throw(CException) = 0;
				
                //! Abstract method for the start of the control unit
				/*!
				 This is where the subclass need to be start all the staff needed by normal control process. An exception
				 will stop the Control Unit live and perform the deinitialization of the control unit.
				 */
				virtual void unitStart() throw(CException) = 0;
				
				//! Abstract method for the stop of the control unit
				/*!
				 This is where the subclass need to be stop all the staff needed for pause the control process. An exception
				 will stop the Control Unit live and perform the deinitialization of the control unit.
				 */
				virtual void unitStop() throw(CException) = 0;
				
				//! Abstract method for the deinit of the control unit
				/*!
				 This is where the subclass need to be deinit all the staff that has been allocatate into the init method.
				 Usually the hardware deallocation etc..
				 */
				virtual void unitDeinit() throw(CException) = 0;
				
				//!handler calledfor restor a control unit to a determinate point
				/*!
				 On the call of this handler the cache restore part is filled with the dataset
				 that at restore point was pushed by control unit.
				 \param snapshot_cache is the cache, like norma used in control unit development,
				 that contain the four domain filled with the attribute/value faound
				 on saved tag
				 */
				virtual void unitRestoreToSnapshot(AbstractSharedDomainCache * const snapshot_cache) throw(CException);
				
				//! this andler is called befor the input attribute will be updated
				virtual void unitInputAttributePreChangeHandler() throw(CException);
				
				//! attribute change handler
				/*!
				 the handle is fired after the input attribute cache as been update triggere
				 by the rpc request for attribute change.
				 */
				virtual void unitInputAttributeChangedHandler() throw(CException);
				
                //Abstract method used to sublcass to set theri needs
				/*!
				 Receive the event for set the dataset input element, this virtual method
				 is empty because can be used by controlunit implementation
				 */
				virtual CDataWrapper* setDatasetAttribute(CDataWrapper*, bool& detachParam) throw (CException);
				
                // Infrastructure configuration update
				/*!
				 This methdo is called when some configuration is received. All the Control Unit subsystem is
				 checked to control waht is needed to update. Subclass that override this method need first inherited
				 the parent one and the check if the CDataWrapper contains something usefull for it.
				 */
				virtual CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);
				
				//! return the accessor by an index
				/*
				 The index parameter correspond to the order that the driver infromation are
				 added by the unit implementation into the function AbstractControlUnit::unitDefineDriver.
				 */
				driver_manager::driver::DriverAccessor * getAccessoInstanceByIndex(int idx);
				
				//!return the accessor for the sared attribute cache
				inline
				AttributeSharedCacheWrapper * const getAttributeCache() {
					return attribute_shared_cache_wrapper;
				}
				
                //!timer for update push metric
                void timeout();
			public:
				
				//! Default Contructor
				/*!
				 \param _control_unit_type the superclass need to set the control unit type for his implementation
				 \param _control_unit_id unique id for the control unit
				 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
				 */
				AbstractControlUnit(const std::string& _control_unit_type,
									const std::string& _control_unit_id,
									const std::string& _control_unit_param);
				
				//! Default Contructor
				/*!
				 \param _control_unit_type the superclass need to set the control unit type for his implementation
				 \param _control_unit_id unique id for the control unit
				 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
				 \param _control_unit_drivers driver information
				 */
				AbstractControlUnit(const std::string& _control_unit_type,
									const std::string& _control_unit_id,
									const std::string& _control_unit_param,
									const ControlUnitDriverList& _control_unit_drivers);
				
				//! default destructor
				virtual ~AbstractControlUnit();
				
				
                //![API] Api for publish a lass method as RPC action
				/*!
				 This is a convenient method to register actiona associated to this instance of the class, class pointer
				 is associated directly to the current instance of this class and domain is fixed to the Control Unit instance.
				 Developer can use this method or the more flexible DeclareAction::addActionDescritionInstance.
				 */
				template<typename T>
				AbstActionDescShrPtr addActionDescritionInstance(T* objectReference,
																 typename ActionDescriptor<T>::ActionPointerDef actionHandler,
																 const char*const actionAliasName,
																 const char*const actionDescription) {
					//call the DeclareAction action register method, the domain will be associated to the control unit isntance
					return DeclareAction::addActionDescritionInstance(objectReference, actionHandler, control_unit_instance.c_str(), actionAliasName, actionDescription);
				}
				
				//! Return the contro unit instance
				const char * getCUInstance();
				
				//! Return the contro unit instance
				const char * getCUID();
				
				//! getControlUnitParam
				const string& getCUParam();
				
				//push output dataset
                virtual void pushOutputDataset(bool ts_already_set = false);
				
				//push system dataset
				virtual void pushInputDataset();
				
				//push system dataset
				virtual void pushCustomDataset();
				
				//push system dataset
				virtual void pushSystemDataset();
			};
		}
    }
}
#endif
