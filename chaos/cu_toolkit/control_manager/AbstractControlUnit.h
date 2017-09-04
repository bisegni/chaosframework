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

#ifndef ControlUnit_H
#define ControlUnit_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <json/json.h>

#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/property/property.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/SWEService.h>
#include <chaos/common/alarm/AlarmCatalog.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/alarm/MultiSeverityAlarm.h>
#include <chaos/common/utility/AggregatedCheckList.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/metadata_logging/metadata_logging.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>

#include <chaos/cu_toolkit/control_manager/ControlUnitTypes.h>
#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
#include <chaos/cu_toolkit/control_manager/handler/handler.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>
#include <chaos/cu_toolkit/control_manager/AttributeSharedCacheWrapper.h>

#define CUINFO LAPP_ << "["<<__FUNCTION__<<" - "<< getDeviceID()<<"]"
#define CUDBG LDBG_ << "[- "<<__FUNCTION__<<" - "<< getDeviceID()<<"]"
#define CUERR LERR_ << "["<<__PRETTY_FUNCTION__<<" - "<< getDeviceID()<<"]"



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

namespace chaos{
    namespace cu {
        namespace control_manager {
            
            //forward declaration
            class ControlManager;
            class ProxyControlUnit;
            class ControlUnitApiInterface;
            class WorkUnitManagement;
            class AbstractExecutionUnit;
            
            namespace slow_command {
                class SlowCommand;
                class SlowCommandExecutor;
            }
            
            //!  Base class for control unit !CHAOS node
            /*!
             This is the abstraction of the contorl unit node of CHAOS. This class extends DeclareAction
             class that permit to publish rpc method for control the control unit life cycle. Most of the API
             that needs to be used to create device and his dataset are contained into the DeviceSchemaDB class.
             */
            class AbstractControlUnit:
            public DeclareAction,
            public common::utility::SWEService,
            protected chaos::common::data::DatasetDB,
            public chaos::common::alarm::AlarmHandler,
            public chaos::common::property::PropertyCollector,
            protected chaos::common::async_central::TimerHandler,
            public chaos::cu::driver_manager::DriverErogatorInterface {
                //friendly class declaration
                friend class ControlManager;
                friend class ProxyControlUnit;
                friend class ControlUnitApiInterface;
                friend class WorkUnitManagement;
                friend class DomainActionsScheduler;
                friend class AbstractExecutionUnit;
                friend class SCAbstractControlUnit;
                friend class RTAbstractControlUnit;
                friend class slow_command::SlowCommand;
                friend class slow_command::SlowCommandExecutor;
            public:
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::cu::driver_manager::driver::DrvRequestInfo, ControlUnitDriverList);
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
                
                //! Return the control unit instance
                const std::string& getCUInstance();
                
                //! Return the control unit instance
                const std::string& getCUID();
                
                //! get control unit load parameter
                const std::string& getCUParam();
                
                //!return true if the cu load paramete are in json format
                const bool isCUParamInJson();
                
                //! return the root of the json document
                /*!
                 in case isCUParamInJson return false the root json document
                 will contains NULL value.
                 */
                const Json::Value& getCUParamJsonRootElement();
                
                //! return the type of the control unit
                const std::string& getCUType();
                
                //!push output dataset
                virtual void pushOutputDataset();
                
                //!push system dataset
                virtual void pushInputDataset();
                
                //!push system dataset
                virtual void pushCustomDataset();
                
                //!push system dataset
                virtual void pushSystemDataset();
                
                //!push alarm dataset
                virtual void pushCUAlarmDataset();
                
                //!push alarm dataset
                virtual void pushDevAlarmDataset();
                
                //!copy into a CDataWrapper last received initialization package
                void copyInitConfiguraiton(chaos::common::data::CDataWrapper& copy);
                
                inline const char * const stateVariableEnumToName(chaos::cu::control_manager::StateVariableType type) {
                    switch(type) {
                        case  chaos::cu::control_manager::StateVariableTypeAlarmCU:
                            return chaos::cu::control_manager::StateVariableTypeAlarmCULabel;
                        case   chaos::cu::control_manager::StateVariableTypeAlarmDEV:
                            return chaos::cu::control_manager::StateVariableTypeAlarmDEVLabel;
                    }
                }
                
                inline int stateVariableNameToEnum(const std::string& name) {
                    if(name.compare(chaos::cu::control_manager::StateVariableTypeAlarmCULabel) == 0) {return chaos::cu::control_manager::StateVariableTypeAlarmCU;}
                    if(name.compare(chaos::cu::control_manager::StateVariableTypeAlarmDEVLabel) == 0) {return chaos::cu::control_manager::StateVariableTypeAlarmDEV;}
                    return -1;
                }
                
                chaos::common::data::CDataWrapper *writeCatalogOnCDataWrapper(chaos::common::alarm::AlarmCatalog& catalog,
                                                                              int32_t dataset_type);
            private:
                //enable trace for heap into control unit environment
#ifdef __CHAOS_DEBUG_MEMORY_CU__
                tracey::scope sc;
#endif
                //!load control key
                std::string control_key;
                
                //! contains the description of the type of the control unit
                std::string control_unit_type;
                
                //! is the unique identification code associated to the control unit instance(rand benerated by contructor)
                std::string control_unit_id;
                
                //! control unit load param
                std::string control_unit_param;
                //!decode control unit paramete in json if conversion is applicable
                bool                            is_control_unit_json_param;
                Json::Reader					json_reader;
                Json::Value						json_parameter_document;
                
                //specify the counter updated by the mds on every initilization that will represent the run of work
                int64_t run_id;
                
                //!logging channel
                chaos::common::metadata_logging::StandardLoggingChannel *standard_logging_channel;
                
                //!control unit alarm group
                chaos::common::metadata_logging::AlarmLoggingChannel    *alarm_logging_channel;
                chaos::cu::control_manager::MapStateVariable           map_variable_catalog;
                
                //!these are the startup command list
                /*!
                 The startup command are a set of command that are sent within the load command and
                 are executed after the control unit is completely load. This are enterely managed by ControlManager.
                 */
                chaos::cu::control_manager::ACUStartupCommandList list_startup_command;
                
                //! keep track of how many push has been done for every dataset
                //! 0 - output, 1-input, 2-custom
                uint32_t    push_dataset_counter;
                
                //! identify last timestamp whene the push rate has been acquired;
                uint64_t    last_push_rate_grap_ts;
                
                //! control unit driver information list
                //! definition of the type for the driver list
                ControlUnitDriverList control_unit_drivers;
                
                //! list of the accessor of the driver requested by the unit implementation
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::cu::driver_manager::driver::DriverAccessor*, VInstantitedDriver);
                VInstantitedDriver accessor_instances;
                
                //! attributed value shared cache
                /*!
                 The data in every attribute is published automatically on the chaos data service
                 with somne triggering logic according to the domain one.
                 */
                AbstractSharedDomainCache *attribute_value_shared_cache;
                
                //! the wrapper for the user to isolate the shared cache
                AttributeSharedCacheWrapper *attribute_shared_cache_wrapper;
                
                //! fast access for acquisition timestamp
                bool use_custom_high_resolution_timestamp;
                AttributeValue *timestamp_acq_cached_value;
                AttributeValue *timestamp_hw_acq_cached_value;
                
                //! fast access for thread scheduledaly cached value
                AttributeValue *thread_schedule_daly_cached_value;
                
                //! check list of services for initialization and start state
                chaos::common::utility::AggregatedCheckList check_list_sub_service;
                
                //! dataset handler managment
                handler::DatasetAttributeHandler dataset_attribute_manager;
                
                //! init configuration
                ChaosUniquePtr<chaos::common::data::CDataWrapper> init_configuration;
                void _initDrivers() throw(CException);
                void _initChecklist();
                void _initPropertyGroup();
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
                virtual chaos::common::data::CDataWrapper* _init(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                /*!
                 Deinit the Control Unit
                 */
                virtual chaos::common::data::CDataWrapper* _deinit(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                /*!
                 Starto the  Control Unit scheduling for device
                 */
                virtual chaos::common::data::CDataWrapper* _start(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                /*!
                 Stop the Custom Control Unit scheduling for device
                 */
                virtual chaos::common::data::CDataWrapper* _stop(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                //!Recover from a recoverable error state
                virtual chaos::common::data::CDataWrapper* _recover(chaos::common::data::CDataWrapper *deinitParam, bool& detachParam) throw(CException);
                /*!
                 Restore the control unit to a precise tag
                 */
                virtual chaos::common::data::CDataWrapper* _unitRestoreToSnapshot(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
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
                                                         std::vector<std::string>& attribute_names);
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
                                                            chaos::common::data::CDataWrapper& dataset);
                
                void fillRestoreCacheWithDatasetFromTag(data_manager::KeyDataStorageDomain domain,
                                                        chaos::common::data::CDataWrapper& dataset,
                                                        AbstractSharedDomainCache& restore_cache);
                
                //!logging api
                void metadataLogging(const std::string& subject,
                                     const chaos::common::metadata_logging::StandardLoggingChannel::LogLevel log_level,
                                     const std::string& message);
                //  It's is the dynamically assigned instance of the CU. it will be used
                // as domain for the rpc action.
                string control_unit_instance;
                
                //! Momentary driver for push data into the central memory
                ChaosUniquePtr<data_manager::KeyDataStorage>  key_data_storage;
                
                //! fast cached attribute vector accessor
                std::vector<AttributeValue*> cache_output_attribute_vector;
                std::vector<AttributeValue*> cache_input_attribute_vector;
                std::vector<AttributeValue*> cache_custom_attribute_vector;
                std::vector<AttributeValue*> cache_system_attribute_vector;
                
                //! for security reason subclass needs to modifier the access to this method(overloading it and keep private)
                virtual AbstractSharedDomainCache *_getAttributeCache();
                
                //! initialize system attribute
                virtual void initSystemAttributeOnSharedAttributeCache();
                
                //! Define the dataset information of the device implementeted into the CU
                /*!
                 This method configure the CDataWrapper whit all th einromation for describe the implemented device
                 */
                virtual void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration) throw(CException);
                
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
                chaos::common::data::CDataWrapper* _setDatasetAttribute(chaos::common::data::CDataWrapper*, bool&) throw (CException);
                
                //! Return the state of the control unit
                /*!
                 Return the current control unit state identifyed by ControlUnitState types
                 fitted into the CDatawrapper with the key CUStateKey::CONTROL_UNIT_STATE
                 */
                chaos::common::data::CDataWrapper* _getState(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                //! Return the information about the type of the current instace of control unit
                /*!
                 Return unit fitted into cdata wrapper:
                 CU type: string type associated with the key @CUDefinitionKey::CS_CM_CU_TYPE
                 */
                chaos::common::data::CDataWrapper* _getInfo(chaos::common::data::CDataWrapper*, bool& detachParam) throw(CException);
                
                //! update the timestamp attribute of the output datapack
                void _updateAcquistionTimestamp(uint64_t alternative_ts);
                
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
                
                //! used by subclass to dinamically ad attribute to the dataset
                /*!
                 subclass need to keep private this methdo to
                 stop the access to the user
                 */
                virtual void _completeDatasetAttribute();
                
                void setBypassState(bool bypass_stage,
                                    bool high_priority = false);
            protected:
                void useCustomHigResolutionTimestamp(bool _use_custom_high_resolution_timestamp);
                void setHigResolutionAcquistionTimestamp(uint64_t high_resolution_timestamp);
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
                virtual void unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver);
                
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
                
                //! Abstract Method that need to be used by the sublcass to undefine ihis data
                /*!
                 Subclass, in this method canclear all infromation defined into the deined event. This because
                 after this call can be called only a new define phase
                 */
                virtual void unitUndefineActionAndDataset() throw(CException);
                
                //!handler called for restore a control unit to a determinate point
                /*!
                 On the call of this handler the cache restore part is filled with the dataset
                 that at restore point was pushed by control unit.
                 \param snapshot_cache is the cache, like norma used in control unit development,
                 that contain the four domain filled with the attribute/value faound
                 on saved tag
                 */
                virtual bool unitRestoreToSnapshot(AbstractSharedDomainCache * const snapshot_cache) throw(CException);
                
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
                virtual chaos::common::data::CDataWrapper* setDatasetAttribute(chaos::common::data::CDataWrapper*, bool& detachParam) throw (CException);
                
                // Infrastructure configuration update
                /*!
                 This methdo is called when some configuration is received. All the Control Unit subsystem is
                 checked to control waht is needed to update. Subclass that override this method need first inherited
                 the parent one and the check if the CDataWrapper contains something usefull for it.
                 */
                virtual chaos::common::data::CDataWrapper* updateConfiguration(chaos::common::data::CDataWrapper*, bool&) throw (CException);
                
                //!callback for put a veto on property value change request
                virtual bool propertyChangeHandler(const std::string& group_name,
                                                   const std::string& property_name,
                                                   const chaos::common::data::CDataVariant& property_value);
                
                //!callback ofr updated property value
                virtual void propertyUpdatedHandler(const std::string& group_name,
                                                    const std::string& property_name,
                                                    const chaos::common::data::CDataVariant& old_value,
                                                    const chaos::common::data::CDataVariant& new_value);
                
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
                
                //!check if attribute hase been autorized by handler
                bool isInputAttributeChangeAuthorizedByHandler(const std::string& attr_name);
                
                //---------------alarm api-------------
                //!create a new alarm into the catalog
                void addStateVariable(chaos::cu::control_manager::StateVariableType variable_type,
                                      const std::string& state_variable_name,
                                      const std::string& state_variable_description);
                
                //!set the severity on all state_variable
                void setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                
                //!set the state_variable state
                bool setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
                                              const std::string& state_variable_name,
                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                //!set the StateVariable state
                bool setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
                                              const unsigned int state_variable_ordered_id,
                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                //!get the current StateVariable state
                bool getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
                                              const std::string& state_variable_name,
                                              common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                //!get the current StateVariable state
                bool getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
                                              const unsigned int state_variable_ordered_id,
                                              common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                
                //! set the value on the busy flag
                void setBusyFlag(bool state);
                
                //!return the current value of the busi flag
                const bool getBusyFlag() const;
                
                //!called when an alarm has been modified in his severity
                void alarmChanged(const std::string& state_variable_tag,
                                  const std::string& state_variable_name,
                                  const int8_t state_variable_severity);
                
                //!logging api
                void metadataLogging(const chaos::common::metadata_logging::StandardLoggingChannel::LogLevel log_level,
                                     const std::string& message);
                
                //![API] Api for publish a class method as RPC action
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
                
                template<typename O, typename T>
                bool addHandlerOnInputAttributeName(O* object_reference,
                                                    typename handler::DatasetAttributeHandlerDescription<O,T>::HandlerDescriptionActionPtr handler_ptr,
                                                    const std::string& attribute_name) {
                    return dataset_attribute_manager.addHandlerOnAttributeName<O, T>(object_reference,
                                                                                     attribute_name,
                                                                                     handler_ptr);
                }
                
                template<typename O>
                bool addVariantHandlerOnInputAttributeName(O* object_reference,
                                                           typename handler::DatasetAttributeVariantHandlerDescription<O>::HandlerDescriptionActionPtr handler_ptr,
                                                           const std::string& attribute_name) {
                    return dataset_attribute_manager.addVariantHandlerOnAttributeName<O>(object_reference,
                                                                                         attribute_name,
                                                                                         handler_ptr);
                }
                
                bool removeHandlerOnAttributeName(const std::string& attribute_name) {
                    return dataset_attribute_manager.removeHandlerOnAttributeName(attribute_name);
                }
            };
        }
    }
}
#endif
