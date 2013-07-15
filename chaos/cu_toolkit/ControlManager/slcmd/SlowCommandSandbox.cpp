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

#define DEFAULT_STACK_ELEMENT 100
#define DEFAULT_CHECK_TIME 500

SlowCommandSandbox::SlowCommandSandbox():commandStack(DEFAULT_STACK_ELEMENT) {
    //reset all the handler
    currentExecutingCommand = NULL;
    setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
}

SlowCommandSandbox::~SlowCommandSandbox() {
    
}

//! Initialize instance
void SlowCommandSandbox::init(void *initData) throw(chaos::CException) {
    nextAvailableCommand.cmdInfo = NULL;
    nextAvailableCommand.cmdImpl = NULL;
    
    //initialize the shared channel setting
    utility::InizializableService::initImplementation(sharedChannelSetting, initData, "ChannelSetting", "SlowCommandSandbox::init");
    
    checkTimeIntervall = boost::chrono::milliseconds(DEFAULT_CHECK_TIME);
    
    schedulerThread = NULL;
    scheduleWorkFlag = true;
    
}

// Start the implementation
void SlowCommandSandbox::start() throw(chaos::CException) {
    //se the flag to the end o fthe scheduler
    scheduleWorkFlag = true;
    
    //allocate threa
    schedulerThread = new boost::thread(boost::bind(&SlowCommandSandbox::runCommand, this));
}

// Start the implementation
void SlowCommandSandbox::stop() throw(chaos::CException) {
    //we ned to get the lock on the scheduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    //se the flag to the end o fthe scheduler
    scheduleWorkFlag = false;
    
    pauseCondition.notify_one();
    
    //waith that the current command will terminate the work
    conditionWaithSchedulerEnd.wait(lockScheduler);
    
    schedulerThread->join();
    //reset all the handler
    setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
}

//! Deinit the implementation
void SlowCommandSandbox::deinit() throw(chaos::CException) {
    
    //we ned to get the lock on the scheduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    if(schedulerThread) {
        schedulerThread->join();
        delete(schedulerThread);
        schedulerThread = NULL;
    }
    
    SlowCommand *instance = NULL;
    
    //free the remained commands into the stack
    while (!commandStack.empty()) {
        if(commandStack.pop(instance) && instance) {
            delete instance;
        }
    }
    
    //initialize the shared channel setting
    utility::InizializableService::deinitImplementation(sharedChannelSetting, "ChannelSetting", "SlowCommandSandbox::init");
}

void SlowCommandSandbox::runCommand() {
    bool canWork = scheduleWorkFlag;
    bool currentStateEnd = false;
    
    boost::mutex::scoped_lock pauseLock(pauseMutex);
    
    //point to the time for the next check for the available command
    high_resolution_clock::time_point timeLastCheckCommand = boost::chrono::steady_clock::now() + boost::chrono::milliseconds(checkTimeIntervall);
    high_resolution_clock::time_point runStart;
    high_resolution_clock::time_point runEnd;
    
    while(canWork) {
        runStart = boost::chrono::steady_clock::now();
        // call the acquire phase
        acquireHandlerFunctor();
        
        //call the correlation and commit phase();
        correlationHandlerFunctor();
        
        //compute the runnig state or fault
        currentStateEnd = currentExecutingCommand?(((currentExecutingCommand->runningState) & (RunningStateType::RS_End|RunningStateType::RS_Fault))):RunningStateType::RS_End;
        
        if( currentStateEnd ||
           boost::chrono::steady_clock::now() >= timeLastCheckCommand ) {
            //see if the next available or an old push command need to be installed
            if(currentStateEnd) {
                if(!scheduleWorkFlag) {
                    // the command has end and the scehduler has been stopped
                    canWork = false;
                    continue;
                } else {
                    //wait until it continue
                    pauseCondition.wait(pauseLock);
                }
            }
            //try to get the lock otherwhise continue with the current control
            boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler, boost::try_to_lock);
            if(lockScheduler && nextAvailableCommand.cmdImpl) {
                manageAvailableCommand();
                lockScheduler.unlock();
            }
            
            //we need to check for a new
            timeLastCheckCommand = boost::chrono::steady_clock::now() + checkTimeIntervall;
        }
        
        runEnd = boost::chrono::steady_clock::now();
        boost::chrono::milliseconds m = schedulerStepDelay - boost::chrono::duration_cast<boost::chrono::milliseconds>(runEnd-runStart);
        boost::this_thread::sleep_for(m);
    }
    
    //notify the end of the thread
    conditionWaithSchedulerEnd.notify_one();
}

// Check if the new command can be installed
void SlowCommandSandbox::manageAvailableCommand() {
    
    // get the command submission info
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    if(nextAvailableCommand.cmdImpl) {
        uint8_t submissionState = nextAvailableCommand.cmdImpl->submissionRule;
        
        //if the current command is null we simulate and END state
        uint8_t curCmdRunningState = (currentExecutingCommand?currentExecutingCommand->runningState:RunningStateType::RS_End);
        bool toRemoveOrStak = submissionState >= curCmdRunningState;
        if(!toRemoveOrStak) return; //we don't need to touch anthing
        if ( curCmdRunningState >= 2 || submissionState & SubmissionRuleType::SUBMIT_AND_Kill) {
            //for now we delete it after we need to manage it
            if(currentExecutingCommand) {
                delete(currentExecutingCommand);
                currentExecutingCommand = NULL;
            }
        }else if( currentExecutingCommand && (submissionState & SubmissionRuleType::SUBMIT_AND_Stack)) {
            //push current command into the stack
            commandStack.push(currentExecutingCommand);
        }
        
        //install the new command handler
        installHandler(nextAvailableCommand.cmdImpl, nextAvailableCommand.cmdInfo->element);
        
        //delete the command description
        if(nextAvailableCommand.cmdInfo) delete (nextAvailableCommand.cmdInfo);
        
    } else {
        SlowCommand *popedCommand = NULL;
        //we don't have new command so i need to view only if the current command has halted or faulted
        if ( currentExecutingCommand->runningState > 4) {
            
            //for now we delete it after we need to manage it
            delete(currentExecutingCommand);
            
            // command has finisced or has fault
            if(!commandStack.pop(popedCommand)) {
                //"Error popping the command form stack"
            }
            
        }
        
        //pop the latest command
        if(!commandStack.empty()) {
            if(!commandStack.pop(popedCommand)) {
                //"Error popping the command form stack"
            }
        }
        
        installHandler(popedCommand, NULL);
    }
}

//!install the command
void SlowCommandSandbox::installHandler(SlowCommand *cmdImpl, CDataWrapper* setData) {
    CHAOS_ASSERT(cmdImpl)
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    //set current command
    currentExecutingCommand = cmdImpl;
    currentExecutingCommand->sharedChannelSettingPtr = &sharedChannelSetting;
    //associate the keydata storage to the command
    currentExecutingCommand->keyDataStorage = keyDataStorage;
    
    if(currentExecutingCommand) {
        uint8_t handlerMask = currentExecutingCommand->implementedHandler();
        //install the pointer of th ecommand into the respective handler functor
        
        //check set handler
        if(handlerMask & HandlerType::HT_Set) {
            setHandlerFunctor.cmdInstance = currentExecutingCommand;
            //at this point the set need to bee called
            setHandlerFunctor(setData);
        }
        
        //acquire handler
        if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = currentExecutingCommand;
        
        //correlation commit
        if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = currentExecutingCommand;
    } else {
        setHandlerFunctor.cmdInstance = NULL;
        acquireHandlerFunctor.cmdInstance = NULL;
        correlationHandlerFunctor.cmdInstance = NULL;
    }
}

bool SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    if(utility::StartableService::serviceState != utility::StartableServiceType::SS_STARTED) return false;
    //delete the waiting command
    if(nextAvailableCommand.cmdImpl) delete (nextAvailableCommand.cmdImpl);
    if(nextAvailableCommand.cmdInfo) delete (nextAvailableCommand.cmdInfo);
    
    nextAvailableCommand.cmdInfo = cmdInfo;
    nextAvailableCommand.cmdImpl = cmdImpl;
    return true;
}

//set the dafault startup command
void SlowCommandSandbox::setDefaultCommand(SlowCommand *cmdImpl) {
    //check if th epointer is valid
    if(!cmdImpl) return;
    
    //lock the scehduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    if(utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTED ||
       utility::StartableService::serviceState == utility::StartableServiceType::SS_STARTING) {
        delete(cmdImpl);
        throw CException(1, "The sandox is starting or is started so the default command can be installe", "SlowCommandSandbox::setDefaultCommand");
    }
    
    
    
    //we can setup the base default command
    installHandler(cmdImpl, NULL);
}