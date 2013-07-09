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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandConstants.h>


using namespace chaos;
using namespace chaos::cu::control_manager::slow_command;

#define SCELAPP_ LAPP_ << "[SlowCommandExecutor-" << executorID << "]"
#define SCELDBG_ LDBG_ << "[SlowCommandExecutor-" << executorID << "]"
#define SCELERR_ LERR_ << "[SlowCommandExecutor-" << executorID << "]"

SlowCommandExecutor::SlowCommandExecutor() {}

SlowCommandExecutor::SlowCommandExecutor(CThread *_cuThreadForExecutor, std::string _executorID):executorID(_executorID), cuThreadForExecutor(_cuThreadForExecutor) {}

SlowCommandExecutor::~SlowCommandExecutor() {
    
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    SCELAPP_ << "Initializating ";
    utility::StartableService::init(initData);
    
    performQueueCheck = true;
    
    SCELAPP_ << "Initializating Sandbox";    
    utility::InizializableService::initImplementation(&commandSandbox, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");
    SCELAPP_ << "Sandbox inizializated"; 
}


// Start the implementation
void SlowCommandExecutor::start() throw(chaos::CException) {
    SCELAPP_ << "Starting";

    utility::StartableService::start();
    // thread creation
    
    SCELAPP_ << "Starting thread";
    incomingCheckThreadPtr.reset(new boost::thread(boost::bind(&SlowCommandExecutor::performIncomingCommandCheck, this)));
#if defined(__linux__) || defined(__APPLE__)
    int retcode;
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) incomingCheckThreadPtr->native_handle();
    if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)  {
        throw CException(retcode, "Get Thread schedule param", "SlowCommandExecutor::start");
    }
    
    
    policy = SCHED_RR;
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0) {
        throw CException(retcode, "Set Thread schedule param", "SlowCommandExecutor::start");
    }
#endif
    
    SCELAPP_ << "Starting sandbox";
    utility::StartableService::startImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::start");
    SCELAPP_ << "Sandbox started";
    
    SCELAPP_ << "Started";
}

// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    SCELAPP_ << "Stopping";
    
    SCELAPP_ << "Stopping sandbox";
    utility::StartableService::stopImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::stop");
    SCELAPP_ << "Stopped";
    
    //lock for queue access
    boost::unique_lock<boost::mutex> lock(mutextQueueManagment);
    
    performQueueCheck = false;
    
    //notify the thread to start
    SCELAPP_ << "Stopping queue thread";
    readThreadWhait.notify_one();
    
    //wait the thread
    SCELAPP_ << "Join queue thread";
    incomingCheckThreadPtr->join();
    SCELAPP_ << "Queue thread ended";

    utility::StartableService::stop();
    
    SCELAPP_ << "Stopped";
}

// Deinit the implementation
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    SCELAPP_ << "Deinitializing";
    //clear all instancer
    SCELAPP_ << "Removing all the instacer of the command";
    for(std::map<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >::iterator it = mapCommandInstancer.begin();
        it != mapCommandInstancer.end();
        it++) {
        SCELAPP_ << "Dispose instancer " << it->first;
        if(it->second) delete(it->second);
    }
    mapCommandInstancer.clear();
    SCELAPP_ << "Deinitializing sandbox";
    utility::InizializableService::deinitImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::deinit");
    SCELAPP_ << "Sandbox Deinitialized";
    
    utility::StartableService::deinit();
    SCELAPP_ << "Deinitialized";
}

//! Perform a command registration
void SlowCommandExecutor::setDefaultCommand(string& alias) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTED) {
        throw CException(1, "The command infrastructure is in running state", "SlowCommandExecutor::setDefaultCommand");
    }
    
    SCELAPP_ << "Install the default command with alias: " << alias;
    commandSandbox.setDefaultCommand(instanceCommandInfo(alias));
}

//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string& alias, chaos::common::utility::ObjectInstancer<SlowCommand> *instancer) {
    SCELAPP_ << "Install new command with alias" << alias;
    mapCommandInstancer.insert(make_pair<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >(alias, instancer));
}

//! Check the incoming command rule
void SlowCommandExecutor::performIncomingCommandCheck() {
    //lock for queue access
    boost::unique_lock<boost::mutex> lock(mutextQueueManagment);
    
    while(performQueueCheck) {
        boost::posix_time::time_duration delay(0,0,0,0);
        while(commandSubmittedQueue.empty() && !incomingCheckThreadPtr->timed_join(delay)) {
            emptyQueueConditionLock.notify_one();
            readThreadWhait.wait(lock);
        }
        
        //-------------------check if we can use the command-------------------------
#if DEBUG
        SCELDBG_ << "Check new queue command";
#endif
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        // we need to check if there is a "waiting command" and so we need to lock the scheduler
        if(!element) {
            //never need to happen
#if DEBUG
            SCELDBG_ << "COmmand pointer NULL";
#endif
            commandSubmittedQueue.pop();
            continue;
        }
        
        try {
#if DEBUG
            SCELDBG_ << "Install new command locking the sandbox";
#endif
            //lock the command scheduler
            boost::unique_lock<boost::recursive_mutex> lockScheduler(commandSandbox.mutextCommandScheduler);
            if(commandSandbox.nextAvailableCommand.cmdImpl == NULL && commandSandbox.nextAvailableCommand.cmdInfo == NULL) {
#if DEBUG
                SCELDBG_ << "No waiting command into sandbox, try to install new one";
#endif
                
                // no waiting command so set the next available wit the info and instance
                // if something goes wrong an axception is fired and element is fired
                if(commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element))){
#if DEBUG
                    SCELDBG_ << "Command installed";
#endif

                    //remove command form the queue
                    commandSubmittedQueue.pop();
#if DEBUG
                    SCELDBG_ << "Command popped from the queue";
#endif
                }
                
            } else if(element != commandSandbox.nextAvailableCommand.cmdInfo &&
                      element->getPriority() > commandSandbox.nextAvailableCommand.cmdInfo->getPriority()) {
#if DEBUG
                SCELDBG_ << "There is a waiting command into sandbox, but ne one have more priority";
#endif

                //We ave a new, higer priority, command to submit
#if DEBUG
                SCELDBG_ << "Swap waithing command whith new one";
#endif

                commandSubmittedQueue.push(commandSandbox.nextAvailableCommand.cmdInfo);
                if(commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element))) {
#if DEBUG
                    SCELDBG_ << "New command installed";
#endif
                    //remove command form the queue
                    commandSubmittedQueue.pop();
                }
                
            } else {
                //current top command can be used and is left into the queue
            }
            
            lockScheduler.unlock();
        }catch (CException& setupEx) {
            SCELERR_ << setupEx.what();
            //somenthing is gone worng with the current element and need to be free
            if(element) delete element;
        }
        //-------------------check if we can use the command-------------------------
    }
#if DEBUG
    SCELDBG_ << "Queue thread has been shutting down.. cleaning the queue";
#endif
   
    //whe are terminating and we need to erase the command information not elaborated
    //we still protected by the lock on mutextQueueManagment mutext
    while (commandSubmittedQueue.empty()) {
        //get the command on top
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        if(element) {
#if DEBUG
            SCELDBG_ << "----------------------------------------------------------------------------";
            SCELDBG_ << "Removeing element " << element->element->getJSONString();
#endif
            //remove from the queue
            commandSubmittedQueue.pop();
            
            //release it
            delete(element);
#if DEBUG
            SCELDBG_ << "element deleted";
            SCELDBG_ << "----------------------------------------------------------------------------";
#endif
        }
    }
    
    //end of the thread
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(CDataWrapper *submissionInfo) {
    std::string commandAlias;
    if(!submissionInfo->hasKey(SlowCommandSubmissionKey::COMMAND_ALIAS)) {
        throw CException(1, "The alias of the slow command is mandatory", "SlowCommandExecutor::setupCommand");
    }
    commandAlias = submissionInfo->getStringValue(SlowCommandSubmissionKey::COMMAND_ALIAS);
#if DEBUG
    SCELDBG_ << "Instancing command " << commandAlias;
#endif
    SlowCommand *instance = instanceCommandInfo(commandAlias);
    if(!submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_RULE)) {
        if(instance) instance->submissionRule = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_RULE);
#if DEBUG
        SCELDBG_ << "Submizzion rule for command " << commandAlias << " is: " << instance->submissionRule;
#endif
    }
    return instance;
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
    SlowCommand *result = NULL;
    if(mapCommandInstancer.count(commandAlias)) {
         result = mapCommandInstancer[commandAlias]->getInstance();
    } else {
#if DEBUG
        SCELDBG_ << "No command found for alias " << commandAlias;
#endif
    }
    return result;
}

//! Submite the new sloc command information
bool SlowCommandExecutor::submitCommand(CDataWrapper *commandDescription) {
    CHAOS_ASSERT(commandDescription)
    boost::unique_lock<boost::mutex> lock(mutextQueueManagment);
    if(serviceState != utility::StartableServiceType::SS_STARTED) return false;
    uint32_t priority = commandDescription->hasKey(SlowCommandSubmissionKey::SUBMISSION_PRIORITY) ? commandDescription->getUInt32Value(SlowCommandSubmissionKey::SUBMISSION_PRIORITY):50;
#if DEBUG
    SCELDBG_ << "Submit new command " << commandDescription->getJSONString();
    SCELDBG_ << "Submit priority: " << priority;
#endif
    commandSubmittedQueue.push(new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true));
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}