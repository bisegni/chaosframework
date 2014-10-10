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
using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;

using namespace chaos::common::data;
using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;


#define SCACU_LAPP_ LAPP_ << "[SCAbstractControlUnit:" << getCUInstance() <<"] - "
#define SCACU_LDBG_ LDBG_ << "[SCAbstractControlUnit:" << getCUInstance() <<"] - "
#define SCACU_LERR_ LERR_ << "[SCAbstractControlUnit:" << getCUInstance() <<" ("<< __LINE__ <<")] - "

SCAbstractControlUnit::SCAbstractControlUnit(const std::string& _control_unit_id,
											 const std::string& _control_unit_param):
AbstractControlUnit(CUType::SCCU,
					_control_unit_id,
					_control_unit_param),
slow_command_executor(NULL) {}

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
slow_command_executor(NULL){}

SCAbstractControlUnit::~SCAbstractControlUnit() {
	
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
	//this need to be made first of AbstractControlUnit::init(initData); because
	//SlowCommandExecutor has his own instance of cache and this control unit need to
	//use that
	
	//allocate the SlowCommandExecutor, custom implementation of batch command executor engine
	slow_command_executor = new SlowCommandExecutor(control_unit_instance, this);
	//set the driver delegator to the executor
	slow_command_executor->driverAccessorsErogator = this;
	//associate the shared cache of the executor to the asbtract control unit one
	attribute_value_shared_cache = slow_command_executor->getAttributeSharedCache();
	//init executor
	utility::StartableService::initImplementation(slow_command_executor, (void*)NULL, "Slow Command Executor", __PRETTY_FUNCTION__);
	
	//call parent impl
	AbstractControlUnit::init(initData);
	
    //associate the data storage
    slow_command_executor->keyDataStoragePtr = AbstractControlUnit::keyDataStorage;
    slow_command_executor->deviceSchemaDbPtr = this; //control unit is it'self the database
    
    SCACU_LAPP_ << "Install default slow command for device " << DatasetDB::getDeviceID();
    installCommand<command::SetAttributeCommand>(chaos_batch::BatchCommandsKey::ATTRIBUTE_SET_VALUE_CMD_ALIAS);
    
    SCACU_LAPP_ << "Initializing slow command sandbox for device " << DatasetDB::getDeviceID();
	
    
    //now we can call funciton for custom definition of the shared variable
    SCACU_LAPP_ << "Setting up custom shared variable for device " << DatasetDB::getDeviceID();

}

/*
 Deinit the Control Unit
 */
void SCAbstractControlUnit::deinit() throw(CException) {
	//call parent impl
	AbstractControlUnit::deinit();
	
	if(slow_command_executor) {
		SCACU_LAPP_ << "Deinitialize the command executor for " << DatasetDB::getDeviceID();
		utility::StartableService::deinitImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);
		//deassociate the data storage
		slow_command_executor->keyDataStoragePtr = NULL;
		slow_command_executor->deviceSchemaDbPtr = NULL;
		
		delete(slow_command_executor);
		slow_command_executor = NULL;
	} else {
		SCACU_LAPP_ << "No command executor allocated for " << DatasetDB::getDeviceID();
	}
}

/*
 Starto the  Control Unit scheduling for device
 */
void SCAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
    SCACU_LAPP_ << "Start sandbox for device:" << DatasetDB::getDeviceID();
    utility::StartableService::startImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);

}

/*
 Stop the Custom Control Unit scheduling for device
 */
void SCAbstractControlUnit::stop() throw(CException) {
	//call parent impl
	AbstractControlUnit::stop();
	
    SCACU_LAPP_ << "Stop slow command executor for device:" << DatasetDB::getDeviceID();
    utility::StartableService::stopImplementation(slow_command_executor, "Slow Command Executor", __PRETTY_FUNCTION__);

}

// Perform a command registration
void SCAbstractControlUnit::setDefaultCommand(std::string dafaultCommandName, unsigned int sandbox_instance) {
    slow_command_executor->setDefaultCommand(dafaultCommandName, sandbox_instance);
}

void SCAbstractControlUnit::addExecutionChannels(unsigned int execution_channels) {
    slow_command_executor->addSandboxInstance(execution_channels);
}

/*
 Receive the event for set the dataset input element
 */
CDataWrapper* SCAbstractControlUnit::setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
	uint64_t command_id =0;
    if(!datasetAttributeValues->hasKey(chaos_batch::BatchCommandSubmissionKey::COMMAND_ALIAS_STR)) {
        throw CException(-1, "The alias of the slow command is mandatory", __PRETTY_FUNCTION__);
    }

    // in slow control cu the CDataWrapper instance received from rpc is internally managed
    //so we need to detach it
    // submit the detacched command to slow controll subsystem
    slow_command_executor->submitCommand(datasetAttributeValues, command_id);
    detachParam = true;

	//construct the result
    CDataWrapper *result = new CDataWrapper();
	result->addInt64Value(chaos_batch::BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    return result;
}

/*
 Event for update some CU configuration
 */
CDataWrapper* SCAbstractControlUnit::updateConfiguration(CDataWrapper *updatePack, bool& detachParam) throw (CException) {
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

