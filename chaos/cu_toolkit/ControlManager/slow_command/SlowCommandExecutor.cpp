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

#define LOG_HEAD "[SlowCommandExecutor-" << executorID << "] "

#define SCELAPP_ LAPP_ << LOG_HEAD
#define SCELDBG_ LDBG_ << LOG_HEAD
#define SCELERR_ LERR_ << LOG_HEAD

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID, DatasetDB *_deviceSchemaDbPtr):executorID(_executorID), deviceSchemaDbPtr(_deviceSchemaDbPtr), command_state_queue_max_size(COMMAND_STATE_QUEUE_DEFAULT_SIZE){
    // this need to be removed from here need to be implemented the def undef services
    // register the public rpc api
    std::string rpcActionDomain = executorID; //+ SlowCommandSubmissionKey::COMMAND_EXECUTOR_POSTFIX_DOMAIN;
    
	// add executor has event handler
	commandSandbox.event_handler = this;
    
	
    SCELAPP_ << "Register updateConfiguration action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::getQueuedCommand,
                                                                    rpcActionDomain.c_str(),
                                                                    SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND,
                                                                    "Return the number and the information of the queued command");
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

SlowCommandExecutor::~SlowCommandExecutor() {
    // the instancer of the command can't be uninstalled at deinit step
    // because the executor live  one o one with the contro unit.
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

//! get access to the custom data pointer of the channel setting instance
void SlowCommandExecutor::setSharedCustomDataPtr(void *customDataPtr) {
    commandSandbox.sharedAttributeSetting.customData = customDataPtr;
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    std::vector<string> attribute_names;
    
    utility::StartableService::init(initData);
    
    performQueueCheck = true;
    incomingCheckThreadPtr = NULL;
    
    //at this point, befor the sandbox initialization we need to setup the shared setting memory
    CHAOS_ASSERT(deviceSchemaDbPtr)
    
    
    //init the sand box
    utility::StartableService::initImplementation(commandSandbox, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");
    
    SCELAPP_ << "Populating sandbox shared setting for device input attribute";
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Input, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeShareCache::SVD_INPUT, attribute_names);

	SCELAPP_ << "Populating sandbox shared setting for device output attribute";
	attribute_names.clear();
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Output, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeShareCache::SVD_OUTPUT, attribute_names);
    

    SCELAPP_ << "Check if we need to use the dafult command or we have pause instance";
    if(defaultCommandAlias.size()) {
        SCELAPP_ << "Set the default command ->"<<defaultCommandAlias;
        commandSandbox.setNextAvailableCommand(NULL, instanceCommandInfo(defaultCommandAlias));
        DEBUG_CODE(SCELDBG_ << "Command " << defaultCommandAlias << " successfull installed";)
    }
    
}

void SlowCommandExecutor::initAttributeOnSahredVariableDomain(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<string>& attribute_names) {
	//add input attribute to shared setting
    RangeValueInfo attributeInfo;
	
	AttributeSetting& attribute_setting = commandSandbox.sharedAttributeSetting.getSharedDomain(domain);
	
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
    try {
        // set thread run flag for work
        performQueueCheck = true;
        
        utility::StartableService::start();
        // thread creation
        
        SCELAPP_ << "Starting scheduler thread";
        
        SCELDBG_ << "Acquire lock on mutextQueueManagment";
        boost::mutex::scoped_lock lock(mutextQueueManagment);
        SCELDBG_ << "Lock on mutextQueueManagment acquired";
        
        CHAOS_ASSERT(!incomingCheckThreadPtr)
        incomingCheckThreadPtr = new boost::thread(boost::bind(&SlowCommandExecutor::performIncomingCommandCheck, this));
        if(!incomingCheckThreadPtr) throw CException(1, "Error allocating incoming check thread", "SlowCommandExecutor::start");
        SCELAPP_ << "Scheduler thread started";
        
        utility::StartableService::startImplementation(commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::start");
    } catch (...) {
        SCELAPP_ << "Error starting";
        throw CException(1000, "Generic Error", "SlowCommandExecutor::start");
    }
}

// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    //lock for queue access
    SCELDBG_ << "Acquire lock on mutextQueueManagment";
    boost::mutex::scoped_lock lock(mutextQueueManagment);
    SCELDBG_ << "Lock on mutextQueueManagment acquired";
    
    utility::StartableService::stopImplementation(commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::stop");
    
    // set thread run flag for finish work
    SCELDBG_ << "Set performQueueCheck to false";
    performQueueCheck = false;
    
    //notify the thread to start
    SCELAPP_ << "Stopping queue thread";
    readThreadWhait.notify_one();
    
    //wait the thread
    SCELDBG_ << "Waith on emptyQueueConditionLock";
    emptyQueueConditionLock.wait(lock);
    
    SCELAPP_ << "Join queue thread";
    incomingCheckThreadPtr->join();
    delete(incomingCheckThreadPtr);
    incomingCheckThreadPtr = NULL;
    SCELAPP_ << "Queue thread ended";
    
    
    utility::StartableService::stop();
}

// Deinit the implementation
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    utility::StartableService::deinitImplementation(commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::deinit");
    
    utility::StartableService::deinit();
}

//Event handler
void SlowCommandExecutor::handleEvent(uint64_t command_id, SlowCommandEventType::SlowCommandEventType type, void* type_attribute_ptr) {
	DEBUG_CODE(SCELDBG_ << "Received event of type->" << type << "on command id>"<<command_id;)
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
void SlowCommandExecutor::setDefaultCommand(string commandAlias) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(utility::StartableService::serviceState == ::chaos::utility::service_state_machine::StartableServiceType::SS_STARTED) {
        throw CException(1, "The command infrastructure is in running state", "SlowCommandExecutor::setDefaultCommand");
    }
    
    SCELAPP_ << "Install the default command with alias: " << commandAlias;
    defaultCommandAlias = commandAlias;
}

//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string alias, chaos::common::utility::ObjectInstancer<SlowCommand> *instancer) {
    SCELAPP_ << "Install new command with alias -> " << alias;
    boost::mutex::scoped_lock lock(mutextQueueManagment);
    mapCommandInstancer.insert(make_pair<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >(alias, instancer));
}

//! Check the incoming command rule
void SlowCommandExecutor::performIncomingCommandCheck() {
    //lock for queue access
    
    DEBUG_CODE(SCELDBG_ << "Start thread";)
    boost::mutex::scoped_lock lock(mutextQueueManagment);
    
    while(performQueueCheck) {
        while(commandSubmittedQueue.empty() && performQueueCheck) {
            DEBUG_CODE(SCELDBG_ << "Start waith on the readThreadWhait";)
            readThreadWhait.wait(lock);
            DEBUG_CODE(SCELDBG_ << "Respawn on the readThreadWhait";)
        }
        
        //probable error or quit condition
        if(commandSubmittedQueue.empty() || !performQueueCheck) {
            DEBUG_CODE(SCELDBG_ << "commandSubmittedQueue.empty()=" << commandSubmittedQueue.empty() <<" || " << "!performQueueCheck=" << !performQueueCheck;)
            continue;
        }
        
        //-------------------check if we can use the command-------------------------
        DEBUG_CODE(SCELDBG_ << "Check new queue command";)
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        // we need to check if there is a "waiting command" and so we need to lock the scheduler
        if(!element) {
            //never need to happen
            DEBUG_CODE(SCELDBG_ << "Cmmand pointer NULL";)
            commandSubmittedQueue.pop();
            continue;
        }
        try {
            DEBUG_CODE(    SCELDBG_ << "Install new command locking the sandbox";)
            //lock the command scheduler
            boost::unique_lock<boost::recursive_mutex> lockScheduler(commandSandbox.mutexNextCommandChecker);
            if(commandSandbox.nextAvailableCommand.cmdImpl == NULL && commandSandbox.nextAvailableCommand.cmdInfo == NULL) {
                DEBUG_CODE(SCELDBG_ << "No waiting command into sandbox, try to install new one";)
                // no waiting command so set the next available wit the info and instance
                // if something goes wrong an axception is fired and element is fired
                
                DEBUG_CODE(SCELDBG_ << "Command popped from the queue";)
                //remove command form the queue
                commandSubmittedQueue.pop();
                
                if(commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element))){
                    DEBUG_CODE(SCELDBG_ << "Command installed";)
                } else {
                    //delete the non submitted command
                    DEBUG_CODE(SCELDBG_ << "Error installing command";)
                    delete(element);
                }
            } else if(element != commandSandbox.nextAvailableCommand.cmdInfo &&
                      element->getPriority() > commandSandbox.nextAvailableCommand.cmdInfo->getPriority()) {
                DEBUG_CODE(SCELDBG_ << "There is a waiting command into sandbox, but new one have more priority";)
                //We ave a new, higer priority, command to submit
                DEBUG_CODE(SCELDBG_ << "Swap waithing command whith new one";)
                commandSubmittedQueue.push(commandSandbox.nextAvailableCommand.cmdInfo);
				
				//handle the queued event
				handleEvent(commandSandbox.nextAvailableCommand.cmdImpl->unique_id, SlowCommandEventType::EVT_QUEUED, NULL);
				
                //delete the waiting implementation
                DELETE_OBJ_POINTER(commandSandbox.nextAvailableCommand.cmdImpl)
				
                //remove command form the queue
                commandSubmittedQueue.pop();
                
                if(commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element))) {
                    DEBUG_CODE(SCELDBG_ << "New command installed";)
                } else {
                    //delete the non submitted command
                    DEBUG_CODE(SCELDBG_ << "Error installing command";)
                    delete(element);
                }
                
            } else {
                //current top command can be used and is left into the queue
            }
            
            lockScheduler.unlock();
        } catch (CException& setupEx) {
            SCELERR_ << setupEx.what();
            //somenthing is gone worng with the current element and need to be free
            if(element) delete element;
        } catch(...) {
            if(element) delete element;
            boost::exception_ptr error = boost::current_exception();
            SCELERR_ << error;
        }
        //-------------------check if we can use the command-------------------------
    }
    DEBUG_CODE(SCELDBG_ << "Queue thread has been shutting down.. cleaning the queue";)
    //whe are terminating and we need to erase the command information not elaborated
    //we still protected by the lock on mutextQueueManagment mutext
    while (!commandSubmittedQueue.empty()) {
        //get the command on top
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        if(element) {
            DEBUG_CODE( SCELDBG_ << "----------------------------------------------------------------------------";
                       SCELDBG_ << "Removeing element " << element->element->getJSONString();)            //remove from the queue
            commandSubmittedQueue.pop();
            
            //release it
            delete(element);
            DEBUG_CODE( SCELDBG_ << "element deleted";
                       SCELDBG_ << "----------------------------------------------------------------------------";)
        }
    }
    
    //end of the thread
    emptyQueueConditionLock.notify_one();
    DEBUG_CODE(SCELDBG_ << "Thread terminated";)
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
bool SlowCommandExecutor::submitCommand(CDataWrapper *commandDescription, uint64_t& command_id) {
    CHAOS_ASSERT(commandDescription)
    boost::mutex::scoped_lock lock(mutextQueueManagment);
    if(serviceState != ::chaos::utility::service_state_machine::StartableServiceType::SS_STARTED) return false;
    uint32_t priority = commandDescription->hasKey(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32):50;
    //DEBUG_CODE(SCELDBG_ << "Submit new command " << commandDescription->getJSONString();)
    DEBUG_CODE(SCELDBG_ << commandDescription->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);)
	
	//add unique id for command
	command_id = ++command_sequence_id;
	commandDescription->addInt64Value(SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
	
	//queue the command
    commandSubmittedQueue.push(new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true));
	
	//andle queue event
	handleEvent(command_id, SlowCommandEventType::EVT_QUEUED, NULL);
	
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}

//----------------------------public rpc command---------------------------

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* SlowCommandExecutor::getQueuedCommand(CDataWrapper *params, bool& detachParam) throw (CException) {
	boost::mutex::scoped_lock lock(mutextQueueManagment);
	CDataWrapper *result = new CDataWrapper();
	//get the number
	result->addInt32Value(SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND_NUMBER_UI32, static_cast<uint32_t>(commandSubmittedQueue.size()));
	
	//get last command name
	std::string name = commandSubmittedQueue.top()->element->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);
	result->addStringValue(SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND_TOP_ALIAS_STR, name);
    return result;
}

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
	if(!params || !commandSandbox.currentExecutingCommand) return NULL;
	SCELAPP_ << "Set command feature on current command into the executor with id: " << executorID;
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(commandSandbox.mutextAccessCurrentCommand);
	
	//recheck current command
	if(!commandSandbox.currentExecutingCommand) return NULL;
	
	//check wath feature we need to setup
	if(params->hasKey(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL)) {
		//has lock information to setup
		commandSandbox.currentExecutingCommand->lockFeaturePropertyFlag[0] = params->getBoolValue(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL);
	}
	
	if(params->hasKey(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64)) {
		//has scheduler step wait
		commandSandbox.currentExecutingCommand->commandFeatures.featureSchedulerStepsDelay = params->getUInt64Value(SlowControlExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64);
	}
    lockForCurrentCommand.unlock();
    commandSandbox.threadSchedulerPauseCondition.unlock();
    return NULL;
}

//! Command features modification rpc action
/*!
 Updat ethe modiable features of the running command
 */
void SlowCommandExecutor::setCommandFeatures(features::Features features) throw (CException) {
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(commandSandbox.mutextAccessCurrentCommand);
	
	//recheck current command
	if(!commandSandbox.currentExecutingCommand) return;

	commandSandbox.currentExecutingCommand->commandFeatures.featuresFlag |= features.featuresFlag;
	commandSandbox.currentExecutingCommand->commandFeatures.featureSchedulerStepsDelay = features.featureSchedulerStepsDelay;
	//}
    lockForCurrentCommand.unlock();
    commandSandbox.threadSchedulerPauseCondition.unlock();
}

//! Kill current command rpc action
CDataWrapper* SlowCommandExecutor::killCurrentCommand(CDataWrapper *params, bool& detachParam) throw (CException) {
	if(!commandSandbox.currentExecutingCommand) return NULL;
	SCELAPP_ << "Kill current command into the executor id: " << executorID;
	commandSandbox.killCurrentCommand();
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