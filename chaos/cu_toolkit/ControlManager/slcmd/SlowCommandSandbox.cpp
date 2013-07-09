//
//  SlowCommandSandbox.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandSandbox.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandConstants.h>

using namespace chaos::chrono;
using namespace chaos::cu::control_manager::slow_command;

#define DEFAULT_CHECK_TIME 500

//! Initialize instance
void SlowCommandSandbox::init(void*) throw(chaos::CException) {
    nextAvailableCommand.cmdInfo = NULL;
    nextAvailableCommand.cmdImpl = NULL;
    
    
}

// Start the implementation
void SlowCommandSandbox::start() throw(chaos::CException) {
    timeLastCheckCommand = boost::chrono::steady_clock::now() + milliseconds(DEFAULT_CHECK_TIME);
}

// Start the implementation
void SlowCommandSandbox::stop() throw(chaos::CException) {
    
}

//! Deinit the implementation
void SlowCommandSandbox::deinit() throw(chaos::CException) {
    
}

void SlowCommandSandbox::runCommand() {
    // call the acquire phase
    acquireHandlerFunctor();
    
    //call the correlation and commit phase();
    correlationHandlerFunctor();
    
    if(boost::chrono::steady_clock::now() >= timeLastCheckCommand) {
        //try to get the lock otherwhise continue with the current control
        boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler, boost::try_to_lock);
        if(lockScheduler && nextAvailableCommand.cmdImpl) {
            manageAvailableCommand();
            lockScheduler.unlock();
        }
        
        //we need to check for a new
        timeLastCheckCommand = boost::chrono::steady_clock::now() + milliseconds(DEFAULT_CHECK_TIME);
    }
    
}

//! Check if the new command can be installed
void SlowCommandSandbox::manageAvailableCommand() {
    bool newCommandCanBeUsed = false;
    // get the command submission info
    boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler);
    
    switch (nextAvailableCommand.cmdImpl->submissionRule) {
        case SubmissionRuleType::SUBMIT_NORMAL:
            newCommandCanBeUsed = currentExecutingCommand->runningState & RunningStateType::RS_End ||
            currentExecutingCommand->runningState & RunningStateType::RS_Over;
            break;
            
        case SubmissionRuleType::SUBMIT_AND_Kill:
            
            break;
            
        case SubmissionRuleType::SUBMIT_AND_Overload:
            
            break;
            
        case SubmissionRuleType::SUBMIT_AND_Stack:
            
            break;
            
    }
    installHandler(nextAvailableCommand.cmdImpl);
    
    //push current command into the stack
    commandStack.push(currentExecutingCommand = nextAvailableCommand.cmdImpl);
    
    //delete the command description
    if(nextAvailableCommand.cmdInfo) delete (nextAvailableCommand.cmdInfo);
}

//!install the command
void SlowCommandSandbox::installHandler(SlowCommand *cmdImpl) {
    CHAOS_ASSERT(cmdImpl)
    boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler);
    
    uint8_t handlerMask = cmdImpl->implementedHandler();
    //install the pointer of th ecommand into the respective handler functor
    
    //check set handler
    if(handlerMask & HandlerType::HT_Set) {
        setHandlerFunctor.cmdInstance = cmdImpl;
        //at this point the set need to bee called
        setHandlerFunctor();
    }
    
    //acquire handler
    if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = cmdImpl;
    
    //correlation commit
    if(handlerMask & HandlerType::HT_Crorrelation) correlationHandlerFunctor.cmdInstance = cmdImpl;
    
}

void SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    
    boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler);
    
    //delete the waiting command
    if(nextAvailableCommand.cmdImpl) delete (nextAvailableCommand.cmdImpl);
    if(nextAvailableCommand.cmdInfo) delete (nextAvailableCommand.cmdInfo);
    
    nextAvailableCommand.cmdInfo = cmdInfo;
    nextAvailableCommand.cmdImpl = cmdImpl;
}

//set the dafault startup command
void SlowCommandSandbox::setDefaultCommand(SlowCommand *cmdImpl) {
    //check if th epointer is valid
    if(!cmdImpl) return;
    
    //lock the scehduler
    boost::unique_lock<boost::recursive_mutex> lockScheduler(mutextCommandScheduler);
    
    if(utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTED ||
       utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTING) {
        delete(cmdImpl);
        throw CException(1, "The sandox is starting or is started so the default command can be installe", "SlowCommandSandbox::setDefaultCommand");
    }
    
    //stack the command
    commandStack.push(currentExecutingCommand = cmdImpl);
    
    //we can setup the base default command
    installHandler(cmdImpl);
}