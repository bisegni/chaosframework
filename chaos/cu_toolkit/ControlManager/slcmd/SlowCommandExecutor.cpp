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
#define SCELERR_ LERR_ << "[SlowCommandExecutor-" << executorID << "]"
SlowCommandExecutor::SlowCommandExecutor(CThread *_cuThreadForExecutor, std::string _executorID):executorID(_executorID), cuThreadForExecutor(_cuThreadForExecutor) {}

SlowCommandExecutor::~SlowCommandExecutor() {
    
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    utility::StartableService::init(initData);
    
    performQueueCheck = true;
    
    utility::InizializableService::initImplementation(&commandSandbox, initData, "SlowCommandSandbox", "SlowCommandExecutor::init");
}


// Start the implementation
void SlowCommandExecutor::start() throw(chaos::CException) {
    utility::StartableService::start();
    // thread creation
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
    
    utility::StartableService::startImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::start");
}

// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    
    utility::StartableService::stopImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::stop");
    
    //lock for queue access
    boost::unique_lock<boost::mutex> lock(mutextQueueManagment);
    
    performQueueCheck = false;
    
    //notify the thread to start
    readThreadWhait.notify_one();
    
    //wait the thread
    incomingCheckThreadPtr->join();
    
    utility::StartableService::stop();
}

// Deinit the implementation
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    //clear all instancer
    for(std::map<string, chaos::common::utility::ObjectInstancer<SlowCommand>* >::iterator it = mapCommandInstancer.begin();
        it != mapCommandInstancer.end();
        it++) {
        SCELAPP_ << "Dispose instancer " << it->first;
        if(it->second) delete(it->second);
    }
    mapCommandInstancer.clear();
    
    utility::InizializableService::deinitImplementation(&commandSandbox, "SlowCommandSandbox", "SlowCommandExecutor::deinit");
    
    utility::StartableService::deinit();
}

//! Perform a command registration
void SlowCommandExecutor::setDefaultCommand(string& alias) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTED) {
        throw CException(1, "The command infrastructure is in running state", "SlowCommandExecutor::setDefaultCommand");
    }
    
    commandSandbox.setDefaultCommand(instanceCommandInfo(alias));
}

//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string& alias, chaos::common::utility::ObjectInstancer<SlowCommand> *instancer) {
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
        
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        // we need to check if there is a "waiting command" and so we need to lock the scheduler
        
        try {
            //lock the command scheduler
            boost::unique_lock<boost::recursive_mutex> lockScheduler(commandSandbox.mutextCommandScheduler);
            if(commandSandbox.nextAvailableCommand.cmdImpl == NULL && commandSandbox.nextAvailableCommand.cmdInfo == NULL) {
                
                // no waiting command so set the next available wit the info and instance
                // if something goes wrong an axception is fired and element is fired
                commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element));
                //remove command form the queue
                commandSubmittedQueue.pop();
                
            } else if(element != commandSandbox.nextAvailableCommand.cmdInfo &&
                      element->getPriority() > commandSandbox.nextAvailableCommand.cmdInfo->getPriority()) {
                
                //We ave a new, higer priority, command to submit
                commandSubmittedQueue.push(commandSandbox.nextAvailableCommand.cmdInfo);
                commandSandbox.setNextAvailableCommand(element, instanceCommandInfo(element->element));
                //remove command form the queue
                commandSubmittedQueue.pop();
                
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
    
    //whe are terminating and we need to erase the command information not elaborated
    //we still protected by the lock on mutextQueueManagment mutext
    while (commandSubmittedQueue.empty()) {
        //get the command on top
        PRIORITY_ELEMENT(CDataWrapper) *element = commandSubmittedQueue.top();
        if(element) {
            //remove from the queue
            commandSubmittedQueue.pop();
            
            //release it
            delete(element);
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
    SlowCommand *instance = instanceCommandInfo(commandAlias);
    if(!submissionInfo->hasKey(SlowCommandSubmissionKey::SUBMISSION_FLAG)) {
        if(instance) instance->submissionRule = submissionInfo->getInt32Value(SlowCommandSubmissionKey::SUBMISSION_FLAG);
    }
    return instance;
}

//! Check if the waithing command can be installed
SlowCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
    if(mapCommandInstancer.count(commandAlias)) {
        return mapCommandInstancer[commandAlias]->getInstance();
    } else {
        std::string message = string("No command found for the alias ") + commandAlias;
        std::string domain = "SlowCommandExecutor::setupCommand";
        throw CException(2, message, domain);
    }
}

//! Submite the new sloc command information
bool SlowCommandExecutor::submitCommand(CDataWrapper *commandDescription, uint8_t priority) {
    boost::unique_lock<boost::mutex> lock(mutextQueueManagment);
    if(serviceState != utility::StartableServiceType::SS_STARTED) return false;
    PriorityQueuedElement<CDataWrapper> *_element = new PriorityQueuedElement<CDataWrapper>(commandDescription, priority, true);
    commandSubmittedQueue.push(_element);
    lock.unlock();
    readThreadWhait.notify_one();
    return true;
}