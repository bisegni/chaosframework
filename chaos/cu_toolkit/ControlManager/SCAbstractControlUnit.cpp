/*
 *	SCAbstractControlUnit.cpp
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

#include <chaos/common/batch_command/BatchCommand.h>

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/command/SCWaitCommand.h>


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
    slow_command_executor = new SlowCommandExecutor(control_unit_instance, this, this);
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
    slow_command_executor = new SlowCommandExecutor(control_unit_instance, this, this);
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
    //call superclass method
    AbstractControlUnit::_defineActionAndDataset(setup_configuration);
    //add the batch command description to the configuration
    SCACU_LAPP_ << "Install default slow commands for node id:" << DatasetDB::getDeviceID();
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(SCWaitCommand), false);
    
    std::vector< boost::shared_ptr<BatchCommandDescription> > batch_command_description;
    slow_command_executor->getCommandsDescriptions(batch_command_description);
    if(batch_command_description.size()){
        //fill setup with command descirption serialization
        for(std::vector< boost::shared_ptr<BatchCommandDescription> > ::iterator it = batch_command_description.begin();
            it != batch_command_description.end();
            it++) {
            boost::shared_ptr<CDataWrapper> full_description((*it)->getFullDescription());
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
    
    
    //associate the data storage and dataset database..this need to be associated before the executor initialization
    slow_command_executor->key_data_storage = AbstractControlUnit::key_data_storage;
    //control unit is it'self the database
    slow_command_executor->dataset_attribute_db_ptr = this;
    
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
        slow_command_executor->key_data_storage = NULL;
        slow_command_executor->dataset_attribute_db_ptr = NULL;
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
void SCAbstractControlUnit::setDefaultCommand(const std::string& dafaultCommandName, unsigned int sandbox_instance) {
    slow_command_executor->setDefaultCommand(dafaultCommandName, sandbox_instance);
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
    
    for(std::vector<std::string>::iterator it = all_sandbox_ids.begin();
        it != all_sandbox_ids.end();
        it++) {
        //add timestamp for every single sandbox
        domain_attribute_setting.addAttribute((*it)+"_hb", 0, DataType::TYPE_INT64);
    }
}

void SCAbstractControlUnit::completeInputAttribute() {
    SCACU_LAPP_ << "Complete the shared cache input attribute";
    AttributeCache& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
    
    std::vector<std::string> command_alias;
    slow_command_executor->getAllCommandAlias(command_alias);
    
    for(std::vector<std::string>::iterator it = command_alias.begin();
        it != command_alias.end();
        it++) {
        SCACU_LAPP_ << "Add input attribute for slow command:" << *it;
        domain_attribute_setting.addAttribute(*it, 255, DataType::TYPE_STRING);
    }
}

void SCAbstractControlUnit::submitSlowCommand(const std::string command_alias,
                                              CDataWrapper *slow_command_pack,
                                              uint64_t& command_id) {
    CHAOS_ASSERT(slow_command_executor)
    slow_command_executor->submitCommand(command_alias,
                                         slow_command_pack,
                                         command_id);
}

/*
 Receive the event for set the dataset input element
 */
CDataWrapper* SCAbstractControlUnit::setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
    uint64_t command_id =0;
    std::auto_ptr<CDataWrapper> result_for_command;
    if(datasetAttributeValues->hasKey(chaos_batch::BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
        CHAOS_ASSERT(slow_command_executor)
        std::string command_alias = datasetAttributeValues->getStringValue(chaos_batch::BatchCommandAndParameterDescriptionkey::BC_ALIAS);
        // in slow control cu the CDataWrapper instance received from rpc is internally managed
        //so we need to detach it
        // submit the detacched command to slow controll subsystem
        slow_command_executor->submitCommand(command_alias,
                                             datasetAttributeValues,
                                             command_id);
        detachParam = true;
        //in this event the value is the alias of the command
        //publish command value
        AttributeValue *attr_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_INPUT, command_alias);
        if(attr_value) {
            std::string cmd_param = datasetAttributeValues->getJSONString();
            //add new size
            attr_value->setNewSize((uint32_t)cmd_param.size());
            
            //set the value without notify because command value are managed internally only
            attr_value->setValue(cmd_param.c_str(), (uint32_t)cmd_param.size(), true);
            
            //push input dataset change
            pushInputDataset();
        }
        //construct the result
        result_for_command.reset(new CDataWrapper());
        result_for_command->addInt64Value(chaos_batch::BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    }
    CDataWrapper *result = AbstractControlUnit::setDatasetAttribute(datasetAttributeValues, detachParam);
    if(result) {
        result_for_command->copyKeyTo(chaos_batch::BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, *result);
    } else {
        result = result_for_command.release();
    }
    return result;
}

/*
 Event for update some CU configuration
 */
CDataWrapper* SCAbstractControlUnit::updateConfiguration(CDataWrapper *update_pack, bool& detach_param) throw (CException) {
    CDataWrapper *result = AbstractControlUnit::updateConfiguration(update_pack, detach_param);
    std::auto_ptr<CDataWrapper> cu_properties;
    CDataWrapper *cu_property_container = NULL;
    if(update_pack->hasKey(ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY)){
        cu_property_container = update_pack;
    } else  if(update_pack->hasKey("property_abstract_control_unit") &&
               update_pack->isCDataWrapperValue("property_abstract_control_unit")){
        cu_properties.reset(update_pack->getCSDataValue("property_abstract_control_unit"));
        if(cu_properties->hasKey(ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY)) {
            cu_property_container = cu_properties.get();
        }
    }
    
    if(cu_property_container) {
        //we need to configure the delay  from a run() call and the next
        uint64_t new_schedule_daly = cu_property_container->getUInt64Value(ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY);
        chaos_batch::features::Features features;
        std::memset(&features, 0, sizeof(chaos_batch::features::Features));
        //features.featuresFlag &= chaos_batch::features::FeaturesFlagTypes::FF_LOCK_USER_MOD;
        features.featureSchedulerStepsDelay = new_schedule_daly;
        slow_command_executor->setCommandFeatures(features);
        //update cached value
        _updateRunScheduleDelay(new_schedule_daly);
        pushSystemDataset();
    }
    return result;
}

void SCAbstractControlUnit::installCommand(boost::shared_ptr<BatchCommandDescription> command_description,
                                           bool is_default,
                                           unsigned int sandbox) {
    CHAOS_ASSERT(slow_command_executor)
    slow_command_executor->installCommand(command_description);
    if(is_default){
        setDefaultCommand(command_description->getAlias(),
                          sandbox);
    }
}
