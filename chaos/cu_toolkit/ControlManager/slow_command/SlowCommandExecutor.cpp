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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandConstants.h>


using namespace chaos;
using namespace chaos::cu::control_manager::slow_command;

#define SCELAPP_ LAPP_ << "[SlowCommandExecutor-" << executorID << "] "
#define SCELDBG_ LDBG_ << "[SlowCommandExecutor-" << executorID << "] "
#define SCELERR_ LERR_ << "[SlowCommandExecutor-" << executorID << "] "

SlowCommandExecutor::SlowCommandExecutor() {}

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID, DeviceSchemaDB *_deviceSchemaDbPtr):executorID(_executorID), deviceSchemaDbPtr(_deviceSchemaDbPtr){}

SlowCommandExecutor::~SlowCommandExecutor() {}

//! get access to the custom data pointer of the channel setting instance
void SlowCommandExecutor::setSharedCustomDataPtr(void *customDataPtr) {
    commandSandbox.sharedChannelSetting.customData = customDataPtr;
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
    
    RangeValueInfo attributeInfo;
    for(std::vector<string>::iterator it = inputAttributeNames.begin();
        it != inputAttributeNames.end();
        it++) {
        
        // retrive the attribute description from the device database
        deviceSchemaDbPtr->getAttributeRangeValueInfo(*it, attributeInfo);
        
        // add the attribute to the shared setting object
        commandSandbox.sharedChannelSetting.addAttribute(*it, attributeInfo.maxSize, attributeInfo.valueType);
    }
    
    utility::InizializableService::initImplementation(commandSandbox, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");
    
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
    //clear all instancer
    SCELAPP_ << "Removing all the instacer of the command";
    for(std::map<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >::iterator it = mapCommandInstancer.begin();
        it != mapCommandInstancer.end();
        it++) {
        SCELAPP_ << "Dispose instancer " << it->first;
        if(it->second) delete(it->second);
    }
    mapCommandInstancer.clear();
    
    utility::InizializableService::deinitImplementation(commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::deinit");
    
    utility::StartableService::deinit();
}

//! Perform a command registration
void SlowCommandExecutor::setDefaultCommand(string commandAlias) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTED) {
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
    commandAlias = submissionInfo->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS);
    DEBUG_CODE(SCELDBG_ << "Instancing command " << commandAlias;)
    SlowCommand *instance = instanceCommandInfo(commandAlias);
    if(instance) {
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_RULE)) {
            instance->submissionRule = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_RULE);
            DEBUG_CODE(SCELDBG_ << "Submizzion rule for command " << commandAlias << " is: " << instance->submissionRule;)
        } else {
            instance->submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
        }
        
        //che if a new scheduler delay has been passed in the info
        if(submissionInfo->hasKey(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL)) {
            instance->featuresFlag |= FeatureFlagTypes::FF_SET_SCHEDULER_DELAY;
            instance->schedulerStepsDelay = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL);
            
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
    if(serviceState != utility::StartableServiceType::SS_STARTED) return false;
    uint32_t priority = commandDescription->hasKey(SlowCommandSubmissionKey::SUBMISSION_PRIORITY) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_PRIORITY):50;
    //DEBUG_CODE(SCELDBG_ << "Submit new command " << commandDescription->getJSONString();)
    DEBUG_CODE(SCELDBG_ << commandDescription->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS);)
    commandSubmittedQueue.push(new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true));
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}