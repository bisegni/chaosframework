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

#include <chaos/common/batch_command/BatchCommand.h>

#include <chaos/cu_toolkit/control_manager/SCAbstractControlUnit.h>

#include <chaos/cu_toolkit/control_manager/slow_command/command/SCWaitCommand.h>


using namespace chaos;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::control_manager::slow_command::command;
using namespace chaos::common::data;
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;


#define SCACU_LAPP_ LAPP_ << "[SCAbstractControlUnit:" << getCUInstance() <<"] - "
#define SCACU_LDBG_ LDBG_ << "[SCAbstractControlUnit:" << getCUInstance() <<"] - "
#define SCACU_LERR_ LERR_ << "[SCAbstractControlUnit:" << getCUInstance() <<" ("<< __LINE__ <<")] - "

SCAbstractControlUnit::SCAbstractControlUnit(const std::string& _control_unit_id,
                                             const std::string& _control_unit_param):
AbstractControlUnit(CUType::SCCU,
                    _control_unit_id,
                    _control_unit_param),
slow_command_executor(NULL) {
    //allocate the SlowCommandExecutor, custom implementation of batch command executor engine
    slow_command_executor = new SlowCommandExecutor(control_unit_instance, this);
    //set the driver delegator to the executor
    slow_command_executor->driverAccessorsErogator = this;
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = slow_command_executor->getAttributeSharedCache();
}

/*!
 Parametrized constructor
 \param _control_unit_id unique id for the control unit
 \param _control_unit_drivers driver information
 */
SCAbstractControlUnit::SCAbstractControlUnit(const std::string& _control_unit_id,
                                             const std::string& _control_unit_param,
                                             const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(CUType::SCCU,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers),
slow_command_executor(NULL){
    //allocate the SlowCommandExecutor, custom implementation of batch command executor engine
    slow_command_executor = new SlowCommandExecutor(control_unit_instance, this);
    //set the driver delegator to the executor
    slow_command_executor->driverAccessorsErogator = this;
    //associate the shared cache of the executor to the asbtract control unit one
    attribute_value_shared_cache = slow_command_executor->getAttributeSharedCache();
}

SCAbstractControlUnit::~SCAbstractControlUnit() {
    if(slow_command_executor) {
        delete(slow_command_executor);
        slow_command_executor = NULL;
    }
}

void  SCAbstractControlUnit::_getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance) {
    //broadcast to the parent the method call
    AbstractControlUnit::_getDeclareActionInstance(declareActionInstance);
    
    //fill the vecto rwith the local declare action instance
    declareActionInstance.push_back(slow_command_executor);
}

//! called whr the infrastructure need to know how is composed the control unit
void SCAbstractControlUnit::_defineActionAndDataset(CDataWrapper& setup_configuration)  throw(CException) {
    //add the batch command description to the configuration
    SCACU_LAPP_ << "Install default slow commands for node id:" << DatasetDB::getDeviceID();
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(SCWaitCommand), false);
    
    //call superclass method
    AbstractControlUnit::_defineActionAndDataset(setup_configuration);
    
    std::vector< ChaosSharedPtr<BatchCommandDescription> > batch_command_description;
    slow_command_executor->getCommandsDescriptions(batch_command_description);
    if(batch_command_description.size()){
        //fill setup with command descirption serialization
        for(std::vector< ChaosSharedPtr<BatchCommandDescription> > ::iterator it = batch_command_description.begin();
            it != batch_command_description.end();
            it++) {
            ChaosSharedPtr<CDataWrapper> full_description((*it)->getFullDescription());
            setup_configuration.appendCDataWrapperToArray(*full_description);
        }
        setup_configuration.finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION);
    }
}

AbstractSharedDomainCache *SCAbstractControlUnit::_getAttributeCache() {
    return AbstractControlUnit::_getAttributeCache();
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void SCAbstractControlUnit::init(void *initData) throw(CException) {
    //this need to be made first of AbstractControlUnit::init(initData); because
    //SlowCommandExecutor has his own instance of cache and this control unit need to
    //use that
    
    //call parent impl
    AbstractControlUnit::init(initData);
    
    //control unit is it'self the database
    slow_command_executor->control_unit_instance = this;
    
    //init executor
    StartableService::initImplementation(slow_command_executor, (void*)NULL, "Slow Command Executor", __PRETTY_FUNCTION__);
    
    SCACU_LAPP_ << "Initializing slow command sandbox for node id " << DatasetDB::getDeviceID();
    
    
    //now we can call funciton for custom definition of the shared variable
    SCACU_LAPP_ << "Setting up custom shared variable for node id " << DatasetDB::getDeviceID();
}

/*
 Deinit the Control Unit
 */
void SCAbstractControlUnit::deinit() throw(CException) {
    //call parent impl
    AbstractControlUnit::deinit();
    
    if(slow_command_executor) {
        SCACU_LAPP_ << "Deinitialize the command executor for " << DatasetDB::getDeviceID();
        StartableService::deinitImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);
        //deassociate the data storage
        slow_command_executor->control_unit_instance = NULL;
    } else {
        SCACU_LAPP_ << "No command executor allocated for " << DatasetDB::getDeviceID();
    }
}

/*
 Starto the  Control Unit scheduling for node
 */
void SCAbstractControlUnit::start() throw(CException) {
    //call parent impl
    AbstractControlUnit::start();
    
    SCACU_LAPP_ << "Start sandbox for node id:" << DatasetDB::getDeviceID();
    StartableService::startImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);
}

/*
 Stop the Custom Control Unit scheduling for node
 */
void SCAbstractControlUnit::stop() throw(CException) {
    //call parent impl
    AbstractControlUnit::stop();
    
    SCACU_LAPP_ << "Stop slow command executor for node id:" << DatasetDB::getDeviceID();
    StartableService::stopImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);
}

// Perform a command registration
void SCAbstractControlUnit::setDefaultCommand(const std::string& dafaultCommandName,
                                              bool sticky,
                                              unsigned int sandbox_instance) {
    slow_command_executor->setDefaultCommand(dafaultCommandName,
                                             sandbox_instance);
}

void SCAbstractControlUnit::addExecutionChannels(unsigned int execution_channels) {
    slow_command_executor->addSandboxInstance(execution_channels);
}

//! system dataset configuraiton overload
void SCAbstractControlUnit::initSystemAttributeOnSharedAttributeCache() {
    //first call root system dataset initialization method
    AbstractControlUnit::initSystemAttributeOnSharedAttributeCache();
    
    //add slow control system prorperty
    AttributeCache& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM);
    //add heart beat attribute
    SCACU_LAPP_ << "Adding addiodiotnal system attribute for control unit numbers";
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_RUN_UNIT_AVAILABLE, 0, DataType::TYPE_INT32);
    
    uint32_t run_unit = slow_command_executor->getNumberOfSandboxInstance();
    domain_attribute_setting.setValueForAttribute(domain_attribute_setting.getNumberOfAttributes()-1, &run_unit, sizeof(uint32_t));
    
    std::vector<std::string> all_sandbox_ids;
    slow_command_executor->getSandboxID(all_sandbox_ids);
    
    //add sand box identifier for the last system event
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_RUN_UNIT_ID, 127, DataType::TYPE_STRING);
    
    //command status
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_QUEUED_CMD, 0, DataType::TYPE_INT32);
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_STACK_CMD, 0, DataType::TYPE_INT32);
}

void SCAbstractControlUnit::completeInputAttribute() {
    
}

void SCAbstractControlUnit::submitSlowCommand(const std::string command_alias,
                                              CDataWrapper *slow_command_pack,
                                              uint64_t& command_id) {
    CHAOS_ASSERT(slow_command_executor)
    slow_command_executor->submitCommand(command_alias,
                                         slow_command_pack,
                                         command_id);
}

void SCAbstractControlUnit::submitBatchCommand(const std::string& batch_command_alias,
                                               chaos_data::CDataWrapper *command_data,
                                               uint64_t& command_id,
                                               uint32_t execution_channel,
                                               uint32_t priority,
                                               uint32_t submission_rule,
                                               uint32_t submission_retry_delay,
                                               uint64_t scheduler_step_delay)  throw (CException) {
    CHAOS_ASSERT(slow_command_executor)
    slow_command_executor->submitCommand(batch_command_alias,
                                         command_data,
                                         command_id,
                                         execution_channel,
                                         priority,
                                         submission_rule,
                                         submission_retry_delay,
                                         scheduler_step_delay);
}

ChaosUniquePtr<CommandState> SCAbstractControlUnit::getStateForCommandID(uint64_t command_id) {
    return slow_command_executor->getStateForCommandID(command_id);
}

/*
 Receive the event for set the dataset input element
 */
CDataWrapper* SCAbstractControlUnit::setDatasetAttribute(CDataWrapper *dataset_attribute_values, bool& detachParam) throw (CException) {
    uint64_t command_id =0;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result_for_command;
    
    //cal first the superclass method because the dataset_attribute_values is not detached
    CDataWrapper *result = AbstractControlUnit::setDatasetAttribute(dataset_attribute_values, detachParam);
    
    //check if we have a command
    if(dataset_attribute_values->hasKey(chaos_batch::BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
        CHAOS_ASSERT(slow_command_executor)
        std::string command_alias = dataset_attribute_values->getStringValue(chaos_batch::BatchCommandAndParameterDescriptionkey::BC_ALIAS);
        // in slow control cu the CDataWrapper instance received from rpc is internally managed
        //so we need to detach it
        // submit the detacched command to slow controll subsystem
        slow_command_executor->submitCommand(command_alias,
                                             dataset_attribute_values,
                                             command_id);
        detachParam = true;
        //construct the result if we don't already have it
        if(!result) {
            result = new CDataWrapper();
        }
        //add command id into the result
        result->addInt64Value(chaos_batch::BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    }
    return result;
}

void SCAbstractControlUnit::propertyUpdatedHandler(const std::string& group_name,
                                                   const std::string& property_name,
                                                   const CDataVariant& old_value,
                                                   const CDataVariant& new_value) {
    if(group_name.compare("property_abstract_control_unit") == 0) {
        key_data_storage->updateConfiguration(property_name, new_value);
        //is my group
        if(property_name.compare(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY) == 0) {
            chaos_batch::features::Features features;
            std::memset(&features, 0, sizeof(chaos_batch::features::Features));
            features.featureSchedulerStepsDelay = new_value.asUInt64();
            slow_command_executor->setCommandFeatures(features);
            _updateRunScheduleDelay(new_value.asUInt64());
            //pushSystemDataset();
        }
    }
    //call superclass
    AbstractControlUnit::propertyUpdatedHandler(group_name,
                                                property_name,
                                                old_value,
                                                new_value);
}

void SCAbstractControlUnit::installCommand(ChaosSharedPtr<BatchCommandDescription> command_description,
                                           bool is_default,
                                           bool sticky,
                                           unsigned int sandbox) {
    CHAOS_ASSERT(slow_command_executor)
    slow_command_executor->installCommand(command_description);
    if(is_default){
        setDefaultCommand(command_description->getAlias(),
                          sticky,
                          sandbox);
    }
}
bool SCAbstractControlUnit::waitOnCommandID(uint64_t& cmd_id) {
    ChaosUniquePtr<CommandState> cmd_state;
    do {
        cmd_state = getStateForCommandID(cmd_id);
        if (!cmd_state.get()) break;
        
        switch (cmd_state->last_event) {
            case BatchCommandEventType::EVT_QUEUED:
                SCACU_LDBG_ << cmd_id << " -> QUEUED";
                break;
            case BatchCommandEventType::EVT_RUNNING:
                SCACU_LDBG_ << cmd_id << " -> RUNNING";
                break;
            case BatchCommandEventType::EVT_WAITING:
                SCACU_LDBG_ << cmd_id << " -> WAITING";
                break;
            case BatchCommandEventType::EVT_PAUSED:
                SCACU_LDBG_ << cmd_id << " -> PAUSED";
                break;
            case BatchCommandEventType::EVT_KILLED:
                SCACU_LDBG_ << cmd_id << " -> KILLED";
                break;
            case BatchCommandEventType::EVT_COMPLETED:
                SCACU_LDBG_ << cmd_id << " -> COMPLETED";
                break;
            case BatchCommandEventType::EVT_FAULT:
                SCACU_LDBG_ << cmd_id << " -> FAULT";
                break;
        }
        //whait some times
        usleep(500000);
    } while (cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED &&
             cmd_state->last_event != BatchCommandEventType::EVT_FAULT &&
             cmd_state->last_event != BatchCommandEventType::EVT_KILLED);
    return (cmd_state.get() &&
            cmd_state->last_event == BatchCommandEventType::EVT_COMPLETED);
}
