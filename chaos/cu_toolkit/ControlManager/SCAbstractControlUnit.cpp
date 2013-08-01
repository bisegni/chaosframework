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
using namespace chaos::cu;
namespace cucs = chaos::cu::control_manager::slow_command;

#define LCCU_ LAPP_ << "[Slow Command Control Unit:" << getCUInstance() <<"] - "

SCAbstractControlUnit::SCAbstractControlUnit() {
    slowCommandExecutor = new cucs::SlowCommandExecutor(cuInstance, this);

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
void SCAbstractControlUnit::init() throw(CException) {
    LCCU_ << "Install default slow command for device " << DeviceSchemaDB::getDeviceID();
    installCommand<cucs::command::SetAttributeCommand>(control_manager::slow_command::SlowCommandsKey::ATTRIBUTE_SET_VALUE_CMD_ALIAS);
    
    LCCU_ << "Initializing slow command sandbox for device " << DeviceSchemaDB::getDeviceID();
    utility::StartableService::initImplementation(slowCommandExecutor, (void*)NULL, "Slow Command Executor", "SCAbstractControlUnit::init");
    
    //now we can call funciton for custom definition of the shared variable
    LCCU_ << "Setting up custom shared variable for device " << DeviceSchemaDB::getDeviceID();
    defineSharedVariable();
    
    //associate the data storage
    slowCommandExecutor->commandSandbox.keyDataStoragePtr = AbstractControlUnit::keyDataStorage;
    slowCommandExecutor->commandSandbox.deviceSchemaDbPtr = this; //control unit is it'self the database
}

/*
 Deinit the Control Unit
 */
void SCAbstractControlUnit::deinit() throw(CException) {
    LCCU_ << "Deinitialize sandbox deinitialization for device:" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::deinitImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::deinit");
    //deassociate the data storage
    slowCommandExecutor->commandSandbox.keyDataStoragePtr = NULL;
    slowCommandExecutor->commandSandbox.deviceSchemaDbPtr = NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
void SCAbstractControlUnit::start() throw(CException) {
    LCCU_ << "Start sandbox for device:" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::startImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::start");

}

/*
 Stop the Custom Control Unit scheduling for device
 */
void SCAbstractControlUnit::stop() throw(CException) {
    LCCU_ << "Stop slow command executor for device:" << DeviceSchemaDB::getDeviceID();
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
    if(!datasetAttributeValues->hasKey(cucs::SlowCommandSubmissionKey::COMMAND_ALIAS_STR)) {
        throw CException(-4, "The alias of the slow command is mandatory", "SlowCommandExecutor::setupCommand");
    }
    // in slow control cu the CDataWrapper instance received from rpc is internally managed
    //so we need to detach it
    detachParam = true;
    // submit the detacched command to slow controll subsystem
    slowCommandExecutor->submitCommand(datasetAttributeValues);
    return NULL;
}

void SCAbstractControlUnit::addSharedVariable(std::string name, uint32_t max_size, chaos::DataType::DataType type) {
    // add the attribute to the shared setting object
    slowCommandExecutor->commandSandbox.sharedAttributeSetting.addAttribute(name, max_size, type);
}

void SCAbstractControlUnit::setSharedVariableValue(std::string name, void *value, uint32_t value_size) {
    // add the attribute to the shared setting object
    cucs::AttributeIndexType attribute_index = 0;
    if((attribute_index = slowCommandExecutor->commandSandbox.sharedAttributeSetting.getIndexForName(name))) {
        slowCommandExecutor->commandSandbox.sharedAttributeSetting.setValueForAttribute(attribute_index, value, value_size);
    }
}

