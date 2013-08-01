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
using namespace chaos::cu::control_manager::slow_command;
namespace cccs = chaos::cu::control_manager::slow_command;

#define SCELAPP_ LAPP_ << "[SlowCommandExecutor-" << executorID << "] "
#define SCELDBG_ LDBG_ << "[SlowCommandExecutor-" << executorID << "] "
#define SCELERR_ LERR_ << "[SlowCommandExecutor-" << executorID << "] "

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID, DeviceSchemaDB *_deviceSchemaDbPtr):executorID(_executorID), deviceSchemaDbPtr(_deviceSchemaDbPtr){
    //this need to be removed from here need to be implemented the def undef services
    // register the public rpc api
    std::string rpcActionDomain = executorID + cccs::SlowCommandSubmissionKey::COMMAND_EXECUTOR_POSTFIX_DOMAIN;
    
    //
    SCELAPP_ << "Register updateConfiguration action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::getQueuedCommand,
                                                                    rpcActionDomain.c_str(),
                                                                    cccs::SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND,
                                                                    "Return the number and the information of the queued command");
    SCELAPP_ << "Register getCommandSandboxStatistics action";
    DeclareAction::addActionDescritionInstance<SlowCommandExecutor>(this,
                                                                    &SlowCommandExecutor::getCommandSandboxStatistics,
                                                                    rpcActionDomain.c_str(),
                                                                    cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_SANDBOX_STATISTICS,
                                                                    "Return the statistics of the sandbox reguaring to the current running command");
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
    mapCommandInstancer.clear();
}

//! get access to the custom data pointer of the channel setting instance
void SlowCommandExecutor::setSharedCustomDataPtr(void *customDataPtr) {
    commandSandbox.sharedAttributeSetting.customData = customDataPtr;
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    std::vector<string> inputAttributeNames;
    
    utility::StartableService::init(initData);
    
    performQueueCheck = true;
    incomingCheckThreadPtr = NULL;
    
    //at this point, befor the sandbox initialization we need to setup the shared setting memory
    CHAOS_ASSERT(deviceSchemaDbPtr)
    
    SCELAPP_ << "Populating sandbox shared setting for device input attribute";
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Input, inputAttributeNames);
    
    //add input attribute to shared setting
    RangeValueInfo attributeInfo;
    
    for(int idx = 0;
        idx < inputAttributeNames.size();
        idx++) {
        
        attributeInfo.reset();
        
        // retrive the attribute description from the device database
        deviceSchemaDbPtr->getAttributeRangeValueInfo(inputAttributeNames[idx], attributeInfo);
        
        // add the attribute to the shared setting object
        commandSandbox.sharedAttributeSetting.addAttribute(inputAttributeNames[idx], attributeInfo.maxSize, attributeInfo.valueType);
        
        if(!attributeInfo.defaultValue.size()) continue;
        
        //setting value using index (the index into the sharedAttributeSetting are sequencial to the inserted order)
        switch (attributeInfo.valueType) {
            case DataType::TYPE_BOOLEAN : {
                bool val = boost::lexical_cast<bool>(attributeInfo.defaultValue);
                commandSandbox.sharedAttributeSetting.setValueForAttribute(idx, &val, sizeof(bool));
                break;}
            case DataType::TYPE_DOUBLE : {
                double val = boost::lexical_cast<double>(attributeInfo.defaultValue);
                commandSandbox.sharedAttributeSetting.setValueForAttribute(idx, &val, sizeof(double));
                break;}
            case DataType::TYPE_INT32 : {
                int32_t val = boost::lexical_cast<int32_t>(attributeInfo.defaultValue);
                commandSandbox.sharedAttributeSetting.setValueForAttribute(idx, &val, sizeof(int32_t));
                break;}
            case DataType::TYPE_INT64 : {
                int64_t val = boost::lexical_cast<int64_t>(attributeInfo.defaultValue);
                commandSandbox.sharedAttributeSetting.setValueForAttribute(idx, &val, sizeof(int64_t));
                break;}
            case DataType::TYPE_STRING : {
                const char * val = attributeInfo.defaultValue.c_str();
                commandSandbox.sharedAttributeSetting.setValueForAttribute(idx, val, (uint32_t)attributeInfo.defaultValue.size());
                break;}
            default:
                break;
                
        }
    }

    //start the sand box
    utility::StartableService::initImplementation(commandSandbox, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");

    SCELAPP_ << "Check if we need to use the dafult command or we have pause instance";
    if(defaultCommandAlias.size()) {
        SCELAPP_ << "Set the default command ->"<<defaultCommandAlias;
        commandSandbox.setNextAvailableCommand(NULL, instanceCommandInfo(defaultCommandAlias));
        DEBUG_CODE(SCELDBG_ << "Command " << defaultCommandAlias << " successfull installed";)
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
                DEBUG_CODE(SCELDBG_ << "There is a waiting command into sandbox, but ne one have more priority";)
                //We ave a new, higer priority, command to submit
                DEBUG_CODE(SCELDBG_ << "Swap waithing command whith new one";)
                commandSubmittedQueue.push(commandSandbox.nextAvailableCommand.cmdInfo);
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
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI32)) {
            instance->featuresFlag |= FeatureFlagTypes::FF_SET_SCHEDULER_DELAY;
            instance->featureSchedulerStepsDelay = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI32);
            DEBUG_CODE(SCELDBG_ << "Set custom  SCHEDULER_STEP_TIME_INTERVALL to " << instance->featureSchedulerStepsDelay << " milliseconds";)
        }
        
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32)) {
            instance->featuresFlag |= FeatureFlagTypes::FF_SET_SUBMISSION_RETRY;
            instance->featureSubmissionRetryDelay = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32);
            DEBUG_CODE(SCELDBG_ << "Set custom  SUBMISSION_RETRY_DELAY to " << instance->featureSubmissionRetryDelay << " milliseconds";)
        }
    }
    return instance;
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
    SlowCommand *result = NULL;
    if(mapCommandInstancer.count(commandAlias)) {
        result = mapCommandInstancer[commandAlias]->getInstance();
    } else {
        DEBUG_CODE(SCELDBG_ << "No command found for alias " << commandAlias;)
    }
    return result;
}

//! Submite the new sloc command information
bool SlowCommandExecutor::submitCommand(CDataWrapper *commandDescription) {
    CHAOS_ASSERT(commandDescription)
    boost::mutex::scoped_lock lock(mutextQueueManagment);
    if(serviceState != ::chaos::utility::service_state_machine::StartableServiceType::SS_STARTED) return false;
    uint32_t priority = commandDescription->hasKey(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_PRIORITY_UI32):50;
    //DEBUG_CODE(SCELDBG_ << "Submit new command " << commandDescription->getJSONString();)
    DEBUG_CODE(SCELDBG_ << commandDescription->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);)
    commandSubmittedQueue.push(new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true));
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}


//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* SlowCommandExecutor::getQueuedCommand(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
	boost::mutex::scoped_lock lock(mutextQueueManagment);
	chaos::CDataWrapper *result = new chaos::CDataWrapper();
	//get the number
	result->addInt32Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND_NUMBER_UI32, static_cast<uint32_t>(commandSubmittedQueue.size()));
	
	//get last command name
	std::string name = commandSubmittedQueue.top()->element->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS_STR);
	result->addStringValue(cccs::SlowControlExecutorRpcActionKey::RPC_GET_QUEUED_COMMAND_TOP_ALIAS_STR, name);
    return result;
}

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* SlowCommandExecutor::getCommandSandboxStatistics(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
	boost::mutex::scoped_lock lock(mutextQueueManagment);
	chaos::CDataWrapper *result = new chaos::CDataWrapper();
	
	//add statistic to result
	result->addInt64Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_SANDBOX_STATISTICS_START_TIME_UI64, commandSandbox.stat.lastCmdStepStart);
	result->addInt64Value(cccs::SlowControlExecutorRpcActionKey::RPC_GET_COMMAND_SANDBOX_STATISTICS_END_TIME_UI64, commandSandbox.stat.lastCmdStepTime);
    return result;
}