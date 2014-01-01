/*
 *	SlowCommandExecutor.cpp
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

#include <vector>

#include <boost/lexical_cast.hpp>

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandConstants.h>


using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;

#define LOG_HEAD_SBE "[SlowCommandExecutor-" << deviceSchemaDbPtr->getDeviceID() << "] "

#define SCELAPP_ LAPP_ << LOG_HEAD_SBE
#define SCELDBG_ LDBG_ << LOG_HEAD_SBE
#define SCELERR_ LERR_ << LOG_HEAD_SBE

#define SETUP_CMD_POINTER(ist) \
ist->device_id = deviceSchemaDbPtr->getDeviceID(); \
ist->sharedAttributeSettingPtr = &global_attribute_cache; \
ist->keyDataStoragePtr = keyDataStoragePtr; \
ist->deviceDatabasePtr = deviceSchemaDbPtr;

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID, DatasetDB *_deviceSchemaDbPtr):executorID(_executorID), default_command_sandbox_instance(1), deviceSchemaDbPtr(_deviceSchemaDbPtr), command_state_queue_max_size(COMMAND_STATE_QUEUE_DEFAULT_SIZE) {
    // this need to be removed from here need to be implemented the def undef services
    // register the public rpc api
    std::string rpcActionDomain = executorID; //+ SlowCommandSubmissionKey::COMMAND_EXECUTOR_POSTFIX_DOMAIN;
	// add executor has event handler
    
	
    SCELAPP_ << "Register getCommandSandboxStatistics action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::getCommandState,
                                                                    rpcActionDomain.c_str(),
                                                                    SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE,
                                                                    "Return the state of the specified command");
	SCELAPP_ << "Register setCommandFeatures action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::setCommandFeatures,
                                                                    rpcActionDomain.c_str(),
                                                                    SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES,
                                                                    "Set the features of the running command");
	SCELAPP_ << "Register killCurrentCommand action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::killCurrentCommand,
                                                                    rpcActionDomain.c_str(),
                                                                    SlowControlExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND,
                                                                    "Set the features of the running command");
	SCELAPP_ << "Register flushCommandStates action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::flushCommandStates,
                                                                    rpcActionDomain.c_str(),
                                                                    SlowControlExecutorRpcActionKey::RPC_FLUSH_COMMAND_HISTORY,
                                                                    "Flush the non active command history state");

	
}

void SlowCommandExecutor::addNewSandboxInstance() {
    SlowCommandSandbox *tmp_ptr = new SlowCommandSandbox();
    tmp_ptr->event_handler = this;
    tmp_ptr->identification.append(deviceSchemaDbPtr->getDeviceID());
    tmp_ptr->identification.append("-sandbox-");
    
    unsigned int new_size = ((unsigned int) sandbox_map.size() + COMMAND_BASE_SANDOXX_ID);
    tmp_ptr->identification.append(lexical_cast<std::string>(new_size));
    

    sandbox_map.insert(make_pair(new_size, tmp_ptr));
    SCELDBG_ << "Add new sandbox to the executor with identification ->" << tmp_ptr->identification;
}

//! Add a number of sandobx to this instance of executor
void SlowCommandExecutor::addSandboxInstance(unsigned int _sandbox_number) {
    //lock for map modification
    WriteLock       lock(sandbox_map_mutex);
    
    //! add new instances
    for(unsigned int idx = 0; idx < _sandbox_number; idx++) {
        addNewSandboxInstance();
    }
}

SlowCommandExecutor::~SlowCommandExecutor() {
 
    SCELAPP_ << "Removing all the instance of sandbox";
    for(std::map<unsigned int, SlowCommandSandbox*>::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        SCELAPP_ << "Dispose instance " << it->first;
        if(it->second) delete(it->second);
    }
    
    // the instancer of the command can't be uninstalled at deinit step
    // because the executor live  one o one with the control unit.
    // In teh CU the commadn are installed at the definition step
    // this mean that until executor live the command remain installed
    // and the CU is not redefined unti it is reloaded but startup
    // a and so new executor will be used
    SCELAPP_ << "Removing all the instacer of the command";
    for(std::map<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >::iterator it = mapCommandInstancer.begin();
        it != mapCommandInstancer.end();
        it++) {
        SCELAPP_ << "Dispose instancer " << it->first;
        if(it->second) delete(it->second);
    }
	
	//clear the instancer command map
    mapCommandInstancer.clear();
	
	//clear the queue of the command state
	command_state_queue.clear();
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);

    std::vector<string> attribute_names;
    
    //reset the command sequence on initialization
	command_sequence_id = 0;
	
    utility::StartableService::init(initData);
    
    //at this point, befor the sandbox initialization we need to setup the shared setting memory
    CHAOS_ASSERT(deviceSchemaDbPtr)
    
    //initialize the shared channel setting
    utility::InizializableService::initImplementation(global_attribute_cache, initData, "global_attribute_cache", "SlowCommandExecutor::init");
    
    if(sandbox_map.size() == 0) {
        //if not sandbox has been added force to create one
        addNewSandboxInstance();
    }
    
    SCELAPP_ << "Initializing all the instance of sandbox";
    for(std::map<unsigned int, SlowCommandSandbox*>::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        SlowCommandSandbox *tmp_ptr =  it->second;
        //init the sand box
        SCELAPP_ << "Initilize instance " << tmp_ptr->identification;
        utility::StartableService::initImplementation(tmp_ptr, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");
    }
	
    SCELAPP_ << "Populating sandbox shared setting for device input attribute";
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Input, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SVD_INPUT, attribute_names);

	SCELAPP_ << "Populating sandbox shared setting for device output attribute";
	attribute_names.clear();
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Output, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SVD_OUTPUT, attribute_names);
    

    SCELAPP_ << "Check if we need to use the dafult command or we have pause instance";
    if(default_command_alias.size()) {
        SCELAPP_ << "Set the default command ->"<<default_command_alias;
        SlowCommand * def_cmd_impl = instanceCommandInfo(default_command_alias);
        SETUP_CMD_POINTER(def_cmd_impl)
        def_cmd_impl->unique_id = ++command_sequence_id;
        sandbox_map[default_command_sandbox_instance]->enqueueCommand(NULL, def_cmd_impl, 50);
        DEBUG_CODE(SCELDBG_ << "Command " << default_command_alias << " successfull installed";)
    }
    
}

void SlowCommandExecutor::initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SharedVeriableDomain domain, std::vector<string>& attribute_names) {
	//add input attribute to shared setting
    RangeValueInfo attributeInfo;
	
	AttributeSetting& attribute_setting = global_attribute_cache.getSharedDomain(domain);
	
	for(int idx = 0;
        idx < attribute_names.size();
        idx++) {
        
        attributeInfo.reset();
        
        // retrive the attribute description from the device database
        deviceSchemaDbPtr->getAttributeRangeValueInfo(attribute_names[idx], attributeInfo);
        
        // add the attribute to the shared setting object
        attribute_setting.addAttribute(attribute_names[idx], attributeInfo.maxSize, attributeInfo.valueType);
        
        if(!attributeInfo.defaultValue.size()) continue;
        
        //setting value using index (the index into the sharedAttributeSetting are sequencial to the inserted order)
        switch (attributeInfo.valueType) {
            case DataType::TYPE_BOOLEAN : {
                bool val = boost::lexical_cast<bool>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(bool));
                break;}
            case DataType::TYPE_DOUBLE : {
                double val = boost::lexical_cast<double>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(double));
                break;}
            case DataType::TYPE_INT32 : {
                int32_t val = boost::lexical_cast<int32_t>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(int32_t));
                break;}
            case DataType::TYPE_INT64 : {
                int64_t val = boost::lexical_cast<int64_t>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(int64_t));
                break;}
            case DataType::TYPE_STRING : {
                const char * val = attributeInfo.defaultValue.c_str();
                attribute_setting.setDefaultValueForAttribute(idx, val, (uint32_t)attributeInfo.defaultValue.size());
                break;}
            default:
                break;
        }
    }

}

// Start the implementation
void SlowCommandExecutor::start() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);

    try {
        // set thread run flag for work
        utility::StartableService::start();
        
        SCELAPP_ << "Starting all the instance of sandbox";
        for(std::map<unsigned int, SlowCommandSandbox*>::iterator it = sandbox_map.begin();
            it != sandbox_map.end();
            it++) {
            SlowCommandSandbox *tmp_ptr =  it->second;
            SCELAPP_ << "Starting instance " << tmp_ptr->identification;
            //starting the sand box
            utility::StartableService::startImplementation(tmp_ptr, "SlowCommandSandbox", "SlowCommandExecutor::start");
        }
       
    } catch (...) {
        SCELAPP_ << "Error starting";
        throw CException(1000, "Generic Error", "SlowCommandExecutor::start");
    }
}

// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);

    //lock for queue access
    SCELAPP_ << "Stopping all the instance of sandbox";
    for(std::map<unsigned int, SlowCommandSandbox*>::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        SlowCommandSandbox *tmp_ptr =  it->second;
        SCELAPP_ << "Stop instance " << tmp_ptr->identification;

        //stopping the sand box
        utility::StartableService::stopImplementation(tmp_ptr, "SlowCommandSandbox", "SlowCommandExecutor::stop");
    }
    utility::StartableService::stop();
}

// Deinit the implementation
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);

    SCELAPP_ << "Deinitializing all the instance of sandbox";
    for(std::map<unsigned int, SlowCommandSandbox*>::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        SlowCommandSandbox *tmp_ptr =  it->second;
        SCELAPP_ << "Deinitializing instance " << tmp_ptr->identification;
        //deinit the sand box
        utility::StartableService::deinitImplementation(tmp_ptr, "SlowCommandSandbox", "SlowCommandExecutor::deinit");
    }
    
    //initialize the shared channel setting
    utility::InizializableService::deinitImplementation(global_attribute_cache, "AttributeCache", "SlowCommandExecutor::deinit");
    
    utility::StartableService::deinit();
}

//Event handler
void SlowCommandExecutor::handleEvent(uint64_t command_id, SlowCommandEventType::SlowCommandEventType type, void* type_attribute_ptr) {
	DEBUG_CODE(SCELDBG_ << "Received event of type->" << type << " on command id -> "<<command_id;)
	switch(type) {
		case SlowCommandEventType::EVT_QUEUED: {
			// get upgradable access
			boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
			
			// get exclusive access
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
			
			addComamndState(command_id);
			//cap the queue
			capCommanaQueue();
			break;
		}

		case SlowCommandEventType::EVT_FAULT: {
			ReadLock lock(command_state_rwmutex);
			boost::shared_ptr<CommandState>  cmd_state = getCommandState(command_id);
			if(cmd_state.get()) {
				cmd_state->last_event = type;
				cmd_state->fault_description = *static_cast<FaultDescription*>(type_attribute_ptr);
			}
			break;
		}
		default: {
			ReadLock lock(command_state_rwmutex);
			boost::shared_ptr<CommandState>  cmd_state = getCommandState(command_id);
			if(cmd_state.get()) {
				cmd_state->last_event = type;
			}
			break;
		}
	}
}


//! Add a new command state structure to the queue (checking the alredy presence)
void SlowCommandExecutor::addComamndState(uint64_t command_id) {
	//WriteLock write_lock(command_state_rwmutex);
	boost::shared_ptr<CommandState> cmd_state(new CommandState());
	cmd_state->command_id = command_id;
	cmd_state->last_event = SlowCommandEventType::EVT_QUEUED;
	
	//add to the queue
	command_state_queue.push_back(cmd_state);
	//add also to the map for a fast access
	command_state_fast_access_map.insert(std::make_pair(command_id, cmd_state));
}

//! Thanke care to limit the size of the queue to the max size permitted
void SlowCommandExecutor::capCommanaQueue() {
	if(command_state_queue.size() <= command_state_queue_max_size) return;
	
	//we need to cap the queue
	for (size_t idx = command_state_queue.size(); idx < command_state_queue_max_size; idx--) {
		if(command_state_queue.empty()) break;
		//get the state
		boost::shared_ptr<CommandState> cmd_state = command_state_queue.back();
		
		//chec if the command can be removed it need to be terminate (complete, fault or killed)
		if(cmd_state->last_event < SlowCommandEventType::EVT_COMPLETED) break;
		//remove from the map
		command_state_fast_access_map.erase(cmd_state->command_id);
		
		//delete it
		command_state_queue.pop_back();
	}
}

//! Add a new command state structure to the queue (checking the alredy presence)
boost::shared_ptr<CommandState> SlowCommandExecutor::getCommandState(uint64_t command_sequence) {
	boost::shared_ptr<CommandState> result;
	if(command_state_fast_access_map.count(command_sequence) > 0 ) {
		result = command_state_fast_access_map[command_sequence];
	}
	return result;
}

//! Perform a command registration
void SlowCommandExecutor::setDefaultCommand(string command_alias, unsigned int sandbox_instance) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(utility::StartableService::serviceState == ::chaos::utility::service_state_machine::StartableServiceType::SS_STARTED) {
        throw CException(1, "The command infrastructure is in running state", "SlowCommandExecutor::setDefaultCommand");
    }
    
    SCELAPP_ << "Install the default command with alias: " << default_command_alias;

    default_command_alias = command_alias;
    default_command_sandbox_instance = sandbox_instance<1?1:sandbox_instance;
}

//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string alias, chaos::common::utility::ObjectInstancer<SlowCommand> *instancer) {
    SCELAPP_ << "Install new command with alias -> " << alias;
    mapCommandInstancer.insert(make_pair<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >(alias, instancer));
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(CDataWrapper *submissionInfo) {
    std::string commandAlias;
    commandAlias = submissionInfo->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);
    DEBUG_CODE(SCELDBG_ << "Instancing command " << commandAlias;)
    SlowCommand *instance = instanceCommandInfo(commandAlias);
    if(instance) {
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_RULE_UI32)) {
            instance->submissionRule = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_RULE_UI32);
            DEBUG_CODE(SCELDBG_ << "Submizzion rule for command " << commandAlias << " is: " << instance->submissionRule;)
        } else {
            instance->submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
        }
    
        //check if the comamnd pack has some feature to setup in the command instance
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64)) {
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY;
            instance->commandFeatures.featureSchedulerStepsDelay = submissionInfo->getInt64Value(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64);
            DEBUG_CODE(SCELDBG_ << "Set custom  SCHEDULER_STEP_TIME_INTERVALL to " << instance->commandFeatures.featureSchedulerStepsDelay << " microseconds";)
        }
        
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32)) {
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY;
            instance->commandFeatures.featureSubmissionRetryDelay = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32);
            DEBUG_CODE(SCELDBG_ << "Set custom  SUBMISSION_RETRY_DELAY to " << instance->commandFeatures.featureSubmissionRetryDelay << " milliseconds";)
        }
		
		//get the assigned id
		instance->unique_id = submissionInfo->getUInt64Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
        SETUP_CMD_POINTER(instance)
        //associate the device id to the command
        //instance->device_id = deviceSchemaDbPtr->getDeviceID();
        
        // associate the global cache
        //instance->sharedAttributeSettingPtr = &global_attribute_cache;
        //associate the live data push services
        //instance->keyDataStoragePtr = keyDataStoragePtr;
        // assocaite the db pointer
        //instance->deviceDatabasePtr = deviceSchemaDbPtr;
    }
    return instance;
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
    SlowCommand *result = NULL;
    if(mapCommandInstancer.count(commandAlias)) {
        result = mapCommandInstancer[commandAlias]->getInstance();
		//forward the pointer of the driver accessor
		result->driverAccessorsErogator = driverAccessorsErogator;
    } else {
        DEBUG_CODE(SCELDBG_ << "No command found for alias " << commandAlias;)
    }
    return result;
}

//! Submite the new sloc command information
void SlowCommandExecutor::submitCommand(CDataWrapper *commandDescription, uint64_t& command_id)  throw (CException) {
    if(!commandDescription)
        throw CException(-1, "Invalid parameter", "SlowCommandExecutor::setCommandFeatures");
    
    if(serviceState != ::chaos::utility::service_state_machine::StartableServiceType::SS_STARTED)
        throw CException(-2, "Slow command executor is not started", "SlowCommandExecutor::submitCommand");
    
    WriteLock       lock(sandbox_map_mutex);
    
    //get execution channel if submitted
    uint32_t execution_channel = commandDescription->hasKey(SlowCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;
    
    //check if the channel is present
    if(sandbox_map.count(execution_channel) == 0)
        throw CException(-3, "Execution channel not found", "SlowCommandExecutor::submitCommand");
    
    SlowCommandSandbox *tmp_ptr = sandbox_map[execution_channel];

    //get priority if submitted
    uint32_t priority = commandDescription->hasKey(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32):50;
    
	SCELDBG_ << "Submit new command " << commandDescription->getJSONString();
    
	DEBUG_CODE(SCELDBG_ << commandDescription->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);)
	
	//add unique id for command
	command_id = ++command_sequence_id;
	commandDescription->addInt64Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
	
	//queue the command
    //commandSubmittedQueue.push(new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true));
    SlowCommand *cmd_instance = instanceCommandInfo(commandDescription);
    if(cmd_instance) {
        tmp_ptr->enqueueCommand(commandDescription, cmd_instance, priority);
    } else {
        throw CException(-4, "Command instantiation failed", "SlowCommandExecutor::submitCommand");

    }
}

//----------------------------public rpc command---------------------------

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* SlowCommandExecutor::getCommandState(CDataWrapper *params, bool& detachParam) throw (CException) {
	SCELAPP_ << "Get command state fromthe executor with id: " << executorID;
	//boost::mutex::scoped_lock lock(mutextQueueManagment);
	ReadLock lock(command_state_rwmutex);
	uint64_t command_id = params->getUInt64Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
	boost::shared_ptr<CommandState> cmd_state = getCommandState(command_id);
	if(!cmd_state.get()) throw CException(1, "The command requested is not present", "SlowCommandExecutor::getCommandSandboxStatistics");
	
	CDataWrapper *result = new CDataWrapper();

	//add statistic to result
	result->addInt32Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_LAST_EVENT_UI32, cmd_state->last_event);
	if(cmd_state->last_event == SlowCommandEventType::EVT_FAULT) {
		result->addInt32Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_CODE_UI32, cmd_state->fault_description.code);
		result->addStringValue(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DESC_STR, cmd_state->fault_description.description);
		result->addStringValue(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR, cmd_state->fault_description.domain);
	}
    return result;
}

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* SlowCommandExecutor::setCommandFeatures(CDataWrapper *params, bool& detachParam) throw (CException) {
	if(!params || sandbox_map.size()==0)
        throw CException(-1, "Invalid parameter", "SlowCommandExecutor::setCommandFeatures");
    
    ReadLock       lock(sandbox_map_mutex);

	SCELAPP_ << "Set command feature on current command into the executor with id: " << executorID;
    //get execution channel if submitted
    uint32_t execution_channel = params->hasKey(SlowCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? params->getUInt32Value(SlowCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;

    SlowCommandSandbox *tmp_ptr = sandbox_map[execution_channel];

    //lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(tmp_ptr->mutextAccessCurrentCommand);
    
    if(!tmp_ptr->currentExecutingCommand)
        throw CException(2, "No Current running command", "SlowCommandExecutor::setCommandFeatures");
    
    //check wath feature we need to setup
	if(params->hasKey(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL)) {
		//has lock information to setup
		tmp_ptr->currentExecutingCommand->element->cmdImpl->lockFeaturePropertyFlag[0] = params->getBoolValue(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL);
	}
	
	if(params->hasKey(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64)) {
		//has scheduler step wait
		tmp_ptr->currentExecutingCommand->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay = params->getUInt64Value(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64);
	}
	
	//recheck current command
	if(!tmp_ptr->currentExecutingCommand) return NULL;
	
	
    lockForCurrentCommand.unlock();
    if(tmp_ptr->threadSchedulerPauseCondition.isInWait())
        tmp_ptr->threadSchedulerPauseCondition.unlock();
    return NULL;
}

//! Command features modification rpc action
/*!
 Updat ethe modiable features of the running command
 */
void SlowCommandExecutor::setCommandFeatures(features::Features features) throw (CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    SlowCommandSandbox *tmp_ptr = sandbox_map[1];
    
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(tmp_ptr->mutextAccessCurrentCommand);
	
	//recheck current command
	if(!tmp_ptr->currentExecutingCommand) return;

	tmp_ptr->currentExecutingCommand->element->cmdImpl->commandFeatures.featuresFlag |= features.featuresFlag;
	tmp_ptr->currentExecutingCommand->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay = features.featureSchedulerStepsDelay;
	//}
    lockForCurrentCommand.unlock();
    tmp_ptr->threadSchedulerPauseCondition.unlock();
}

//! Kill current command rpc action
CDataWrapper* SlowCommandExecutor::killCurrentCommand(CDataWrapper *params, bool& detachParam) throw (CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    SlowCommandSandbox *tmp_ptr = sandbox_map[1];
    
	if(!tmp_ptr->currentExecutingCommand) return NULL;
	SCELAPP_ << "Kill current command into the executor id: " << executorID;
	tmp_ptr->killCurrentCommand();
	return NULL;
}

//! Flush the command state history
CDataWrapper* SlowCommandExecutor::flushCommandStates(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException) {
	SCELAPP_ << "Flushing all endend command state history for executr id:" << executorID;
	// get upgradable access
	boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
	
	while (!command_state_queue.empty() )  {
		//remove command form
		boost::shared_ptr<CommandState> cmd_state = command_state_queue.back();
		
		//chec if the command can be removed it need to be terminate (complete, fault or killed)
		if(cmd_state->last_event < SlowCommandEventType::EVT_COMPLETED) break;
		SCELAPP_ << "Flushing command with id:" << cmd_state->command_id ;
		// get exclusive access
		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
		//remove from the map
		command_state_fast_access_map.erase(cmd_state->command_id);
		
		//delete it
		command_state_queue.pop_back();
	}
	return NULL;
}