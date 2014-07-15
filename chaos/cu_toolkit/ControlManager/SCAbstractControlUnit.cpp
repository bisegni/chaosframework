/*
 *	SCAbstractControlUnit.cpp
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

#include <chaos/common/batch_command/BatchCommandConstants.h>

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/command/SetAttributeCommand.h>


using namespace chaos;

using namespace chaos::cu;
using namespace chaos::cu::control_manager::slow_command;

namespace chaos_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;


#define LCCU_ LAPP_ << "[Slow Command Control Unit:" << getCUInstance() <<"] - "

SCAbstractControlUnit::SCAbstractControlUnit(const std::string& _control_unit_id,
											 const std::string& _control_unit_param):
AbstractControlUnit(CUType::SCCU,
					_control_unit_id,
					_control_unit_param) {
    slow_command_executor = new SlowCommandExecutor(control_unit_instance, this);
	slow_command_executor->driverAccessorsErogator = this;
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
					_control_unit_drivers) {
	slow_command_executor = new SlowCommandExecutor(control_unit_instance, this);
	slow_command_executor->driverAccessorsErogator = this;
}

SCAbstractControlUnit::~SCAbstractControlUnit() {
    if(slow_command_executor) {
        delete(slow_command_executor);
    }
}



void SCAbstractControlUnit::defineSharedVariable() {
    
}

void  SCAbstractControlUnit::_getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance) {
    //broadcast to the parent the method call
    AbstractControlUnit::_getDeclareActionInstance(declareActionInstance);
    
    //fill the vecto rwith the local declare action instance
    declareActionInstance.push_back(slow_command_executor);
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void SCAbstractControlUnit::init(void *initData) throw(CException) {
	//call parent impl
	AbstractControlUnit::init(initData);
	
    //associate the data storage
    slow_command_executor->keyDataStoragePtr = AbstractControlUnit::keyDataStorage;
    slow_command_executor->deviceSchemaDbPtr = this; //control unit is it'self the database
    
    LCCU_ << "Install default slow command for device " << DatasetDB::getDeviceID();
    installCommand<command::SetAttributeCommand>(chaos_batch::BatchCommandsKey::ATTRIBUTE_SET_VALUE_CMD_ALIAS);
    
    LCCU_ << "Initializing slow command sandbox for device " << DatasetDB::getDeviceID();
    utility::StartableService::initImplementation(slow_command_executor, (void*)NULL, "Slow Command Executor", "SCAbstractControlUnit::init");
    
    //now we can call funciton for custom definition of the shared variable
    LCCU_ << "Setting up custom shared variable for device " << DatasetDB::getDeviceID();
    defineSharedVariable();
}

/*
 Deinit the Control Unit
 */
void SCAbstractControlUnit::deinit() throw(CException) {
	//call parent impl
	AbstractControlUnit::deinit();
	
    LCCU_ << "Deinitialize sandbox deinitialization for device:" << DatasetDB::getDeviceID();
    utility::StartableService::deinitImplementation(slow_command_executor, "Slow Command Executor", "SCAbstractControlUnit::deinit");
    //deassociate the data storage
    slow_command_executor->keyDataStoragePtr = NULL;
    slow_command_executor->deviceSchemaDbPtr = NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
void SCAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
    LCCU_ << "Start sandbox for device:" << DatasetDB::getDeviceID();
    utility::StartableService::startImplementation(slow_command_executor, "Slow Command Executor", "SCAbstractControlUnit::start");

}

/*
 Stop the Custom Control Unit scheduling for device
 */
void SCAbstractControlUnit::stop() throw(CException) {
	//call parent impl
	AbstractControlUnit::stop();
	
    LCCU_ << "Stop slow command executor for device:" << DatasetDB::getDeviceID();
    utility::StartableService::stopImplementation(slow_command_executor, "Slow Command Executor", "SCAbstractControlUnit::stop");

}

// Perform a command registration
void SCAbstractControlUnit::setDefaultCommand(std::string dafaultCommandName, unsigned int sandbox_instance) {
    slow_command_executor->setDefaultCommand(dafaultCommandName, sandbox_instance);
}

void SCAbstractControlUnit::addExecutionChannels(unsigned int execution_channels) {
    slow_command_executor->addSandboxInstance(execution_channels);
}

/*
 Receive the evento for set the dataset input element
 */
chaos::common::data::CDataWrapper* SCAbstractControlUnit::setDatasetAttribute(chaos::common::data::CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
	uint64_t command_id =0;
    if(!datasetAttributeValues->hasKey(chaos_batch::BatchCommandSubmissionKey::COMMAND_ALIAS_STR)) {
        throw CException(-1, "The alias of the slow command is mandatory", "SlowCommandExecutor::setDatasetAttribute");
    }

    // in slow control cu the CDataWrapper instance received from rpc is internally managed
    //so we need to detach it
    // submit the detacched command to slow controll subsystem
    slow_command_executor->submitCommand(datasetAttributeValues, command_id);
    detachParam = true;

	//construct the result
    chaos_data::CDataWrapper *result = new chaos_data::CDataWrapper();
	result->addInt64Value(chaos_batch::BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    return result;
}

/*
 Event for update some CU configuration
 */
chaos::common::data::CDataWrapper* SCAbstractControlUnit::updateConfiguration(chaos::common::data::CDataWrapper *updatePack, bool& detachParam) throw (CException) {
	chaos::common::data::CDataWrapper *result = AbstractControlUnit::updateConfiguration(updatePack, detachParam);
    if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
        chaos_batch::features::Features features;
		std::memset(&features, 0, sizeof(chaos_batch::features::Features));
		features.featuresFlag &= chaos_batch::features::FeaturesFlagTypes::FF_LOCK_USER_MOD;
		features.featureSchedulerStepsDelay = (uint32_t)updatePack->getUInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
		slow_command_executor->setCommandFeatures(features);
	}
	return result;
}

void SCAbstractControlUnit::addCustomSharedVariable(std::string name, uint32_t max_size, chaos::DataType::DataType type) {
    // add the attribute to the shared setting object
    slow_command_executor->global_attribute_cache.addVariable(chaos_batch::IOCAttributeSharedCache::SVD_CUSTOM, name, max_size, type);
}

void SCAbstractControlUnit::setVariableValue(chaos_batch::IOCAttributeSharedCache::SharedVeriableDomain domain, std::string name, void *value, uint32_t value_size) {
        // add the attribute to the shared setting object
    chaos_batch::VariableIndexType attribute_index = 0;
    if((attribute_index = slow_command_executor->global_attribute_cache.getSharedDomain(domain).getIndexForName(name))) {
        slow_command_executor->global_attribute_cache.getSharedDomain(domain).setDefaultValueForAttribute(attribute_index, value, value_size);
    }
}

chaos_batch::ValueSetting *SCAbstractControlUnit::getVariableValue(chaos_batch::IOCAttributeSharedCache::SharedVeriableDomain domain, const char *variable_name) {
    return slow_command_executor->global_attribute_cache.getVariableValue(domain, variable_name);
}

