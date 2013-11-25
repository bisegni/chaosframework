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

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/command/SetAttributeCommand.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandConstants.h>

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cu;
using namespace chaos::cu::control_manager::slow_command;

#define LCCU_ LAPP_ << "[Slow Command Control Unit:" << getCUInstance() <<"] - "

SCAbstractControlUnit::SCAbstractControlUnit():AbstractControlUnit(CUType::SCCU) {
    slowCommandExecutor = new SlowCommandExecutor(cuInstance, this);
	slowCommandExecutor->driverAccessorsErogator = this;
}

SCAbstractControlUnit::~SCAbstractControlUnit() {
    if(slowCommandExecutor) {
        delete(slowCommandExecutor);
    }
}



void SCAbstractControlUnit::defineSharedVariable() {
    
}

void  SCAbstractControlUnit::_getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance) {
    //broadcast to the parent the method call
    AbstractControlUnit::_getDeclareActionInstance(declareActionInstance);
    
    //fill the vecto rwith the local declare action instance
    declareActionInstance.push_back(slowCommandExecutor);
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void SCAbstractControlUnit::init(void *initData) throw(CException) {
	//call parent impl
	AbstractControlUnit::init(initData);
	
    //associate the data storage
    slowCommandExecutor->commandSandbox.keyDataStoragePtr = AbstractControlUnit::keyDataStorage;
    slowCommandExecutor->commandSandbox.deviceSchemaDbPtr = this; //control unit is it'self the database
    
    LCCU_ << "Install default slow command for device " << DatasetDB::getDeviceID();
    installCommand<command::SetAttributeCommand>(control_manager::slow_command::SlowCommandsKey::ATTRIBUTE_SET_VALUE_CMD_ALIAS);
    
    LCCU_ << "Initializing slow command sandbox for device " << DatasetDB::getDeviceID();
    utility::StartableService::initImplementation(slowCommandExecutor, (void*)NULL, "Slow Command Executor", "SCAbstractControlUnit::init");
    
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
    utility::StartableService::deinitImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::deinit");
    //deassociate the data storage
    slowCommandExecutor->commandSandbox.keyDataStoragePtr = NULL;
    slowCommandExecutor->commandSandbox.deviceSchemaDbPtr = NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
void SCAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
    LCCU_ << "Start sandbox for device:" << DatasetDB::getDeviceID();
    utility::StartableService::startImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::start");

}

/*
 Stop the Custom Control Unit scheduling for device
 */
void SCAbstractControlUnit::stop() throw(CException) {
	//call parent impl
	AbstractControlUnit::stop();
	
    LCCU_ << "Stop slow command executor for device:" << DatasetDB::getDeviceID();
    utility::StartableService::stopImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::stop");

}

// Perform a command registration
void SCAbstractControlUnit::setDefaultCommand(const char * dafaultCommandName) {
    CHAOS_ASSERT(slowCommandExecutor);
    slowCommandExecutor->setDefaultCommand(dafaultCommandName);
}
/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* SCAbstractControlUnit::setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
	uint64_t command_id =0;
    if(!datasetAttributeValues->hasKey(SlowCommandSubmissionKey::COMMAND_ALIAS_STR)) {
        throw CException(-1, "The alias of the slow command is mandatory", "SlowCommandExecutor::setDatasetAttribute");
    }

    // in slow control cu the CDataWrapper instance received from rpc is internally managed
    //so we need to detach it
    // submit the detacched command to slow controll subsystem
    detachParam = slowCommandExecutor->submitCommand(datasetAttributeValues, command_id);
	if(!detachParam) {
        throw CException(-2, "Error submitting command", "SlowCommandExecutor::setDatasetAttribute");
    }
	//construct the result
	CDataWrapper *result = new CDataWrapper();
	result->addInt64Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    return result;
}

/*
 Event for update some CU configuration
 */
CDataWrapper* SCAbstractControlUnit::updateConfiguration(chaos_data::CDataWrapper *updatePack, bool& detachParam) throw (CException) {
	CDataWrapper *result = AbstractControlUnit::updateConfiguration(updatePack, detachParam);
    if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
		features::Features features;
		std::memset(&features, 0, sizeof(features::Features));
		features.featuresFlag &= features::FeaturesFlagTypes::FF_LOCK_USER_MOD;
		features.featureSchedulerStepsDelay = (uint32_t)updatePack->getUInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
		slowCommandExecutor->setCommandFeatures(features);
	}
	return result;
}

void SCAbstractControlUnit::addSharedVariable(std::string name, uint32_t max_size, chaos::DataType::DataType type) {
    // add the attribute to the shared setting object
    slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(IOCAttributeShareCache::SVD_CUSTOM).addAttribute(name, max_size, type);
}

void SCAbstractControlUnit::setSharedVariableValue(std::string name, void *value, uint32_t value_size) {
    // add the attribute to the shared setting object
	setVariableValue(IOCAttributeShareCache::SVD_CUSTOM, name, value, value_size);
    //VariableIndexType attribute_index = 0;
    //if((attribute_index = slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(IOCAttributeShareCache::SVD_CUSTOM).getIndexForName(name))) {
      //  slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(IOCAttributeShareCache::SVD_CUSTOM).setDefaultValueForAttribute(attribute_index, value, value_size);
    //}
}

void SCAbstractControlUnit::setVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, std::string name, void *value, uint32_t value_size) {
        // add the attribute to the shared setting object
    VariableIndexType attribute_index = 0;
    if((attribute_index = slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(domain).getIndexForName(name))) {
        slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(domain).setDefaultValueForAttribute(attribute_index, value, value_size);
    }
}

ValueSetting *SCAbstractControlUnit::getVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, const char *variable_name) {
	AttributeSetting& attribute_setting = slowCommandExecutor->commandSandbox.sharedAttributeSetting.getSharedDomain(domain);
    VariableIndexType index = attribute_setting.getIndexForName(variable_name);
    return attribute_setting.getValueSettingForIndex(index);
}

