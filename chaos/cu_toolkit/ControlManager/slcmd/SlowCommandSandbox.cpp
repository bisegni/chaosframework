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

#define SCSLAPP_ LAPP_ << "[SlowCommandSandbox-" << "] "
#define SCSLDBG_ LDBG_ << "[SlowCommandSandbox-" << "] "
#define SCSLERR_ LERR_ << "[SlowCommandSandbox-" << "] "

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
    
    SCSLDBG_ << "Get base check time intervall for the scheduler";
    checkTimeIntervall = boost::chrono::milliseconds(DEFAULT_CHECK_TIME);
    
    schedulerThread = NULL;
    scheduleWorkFlag = true;
    
}

// Start the implementation
void SlowCommandSandbox::start() throw(chaos::CException) {
    
    //se the flag to the end o fthe scheduler
    SCSLDBG_ << "Set scheduler work flag to true";
    scheduleWorkFlag = true;
    
    //allocate thread
    SCSLDBG_ << "Allocate new thread for the scheduler";
    schedulerThread = new boost::thread(boost::bind(&SlowCommandSandbox::runCommand, this));
}

// Start the implementation
void SlowCommandSandbox::stop() throw(chaos::CException) {
    //we ned to get the lock on the scheduler
    SCSLDBG_ << "Try to lock mutext for command scheduler";
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    //se the flag to the end o fthe scheduler
    SCSLDBG_ << "Set scheduler work flag to false";
    scheduleWorkFlag = false;
    
    SCSLDBG_ << "Notify pauseCondition variable";
    pauseCondition.unlock();
    
    //waith that the current command will terminate the work
    //SCSLDBG_ << "Wait on conditionWaithSchedulerEnd";
    //conditionWaithSchedulerEnd.wait(lockScheduler);
    
    SCSLDBG_ << "Join on schedulerThread";
    schedulerThread->join();
    SCSLDBG_ << "schedulerThread terminated";
    
    //reset all the handler
    setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
}

//! Deinit the implementation
void SlowCommandSandbox::deinit() throw(chaos::CException) {
    
    //we ned to get the lock on the scheduler
    SCSLDBG_ << "Aquiring lock on mutextCommandScheduler";
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    SCSLDBG_ << "Lock on mutextCommandScheduler acquired";
    
    SCSLDBG_ << "Delete scheduler thread";
    if(schedulerThread) {
        delete(schedulerThread);
        schedulerThread = NULL;
    }
    SCSLDBG_ << "Scheduler deleted";
    
    SlowCommand *instance = NULL;
    
    //free the remained commands into the stack
    SCSLDBG_ << "Remove command into the stack";
    while (!commandStack.empty()) {
        if(commandStack.pop(instance) && instance) {
            delete instance;
        }
    }
    SCSLDBG_ << "Command into the stack removed";
    
    //initialize the shared channel setting
    utility::InizializableService::deinitImplementation(sharedChannelSetting, "ChannelSetting", "SlowCommandSandbox::init");
}

void SlowCommandSandbox::runCommand() {
    bool canWork = scheduleWorkFlag;
    uint8_t curCmdRunningState = 0;
    
    //boost::mutex::scoped_lock pauseLock(pauseMutex);
    
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
        curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningState&(RunningStateType::RS_End|RunningStateType::RS_Fault):RunningStateType::RS_End;
        if(curCmdRunningState ||
           boost::chrono::steady_clock::now() >= timeLastCheckCommand ) {
            //try to get the lock otherwhise continue with the current control
            SCSLDBG_ << "Acquiring lock on mutextCommandScheduler";
            boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler, boost::try_to_lock);

            DEBUG_CODE(if(lockScheduler) {SCSLDBG_ << "FAILURE Acquiring lock on mutextCommandScheduler";})
            if(lockScheduler) {
                DEBUG_CODE(SCSLDBG_ << "lock on mutextCommandScheduler ACQUIRED";)
                curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningState:RunningStateType::RS_End;
                DEBUG_CODE(SCSLDBG_ << "The running porcess has execution state = " << curCmdRunningState;)
                
                uint8_t submissionRule = nextAvailableCommand.cmdImpl->submissionRule;
                DEBUG_CODE(SCSLDBG_ << "We have a waiting command with submiossion rule = " << submissionRule;)
                if(nextAvailableCommand.cmdImpl && nextAvailableCommand.cmdImpl->submissionRule >= submissionRule) {
                    //if the current command is null we simulate and END state
                    if ( curCmdRunningState >= 2 || submissionRule & SubmissionRuleType::SUBMIT_AND_Kill) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                        //for now we delete it after we need to manage it
                        if(currentExecutingCommand) {
                            delete(currentExecutingCommand);
                            currentExecutingCommand = NULL;
                            DEBUG_CODE(SCSLDBG_ << "Current executed command is deleted";)
                        }
                    }else if( currentExecutingCommand && (submissionRule & SubmissionRuleType::SUBMIT_AND_Stack)) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                        //push current command into the stack
                        commandStack.push(currentExecutingCommand);
                        DEBUG_CODE(SCSLDBG_ << "Command stacked";)
                    }
                    
                    //install the new command handler
                    DEBUG_CODE(SCSLDBG_ << "Install next available command";)
                    installHandler(nextAvailableCommand.cmdImpl, nextAvailableCommand.cmdInfo->element);
                    DEBUG_CODE(SCSLDBG_ << "Next available command installed";)

                    //delete the command description
                    if(nextAvailableCommand.cmdInfo) delete (nextAvailableCommand.cmdInfo);
                    DEBUG_CODE(SCSLDBG_ << "nextAvailableCommand.cmdInfo deleted";)
                } else {
                    DEBUG_CODE(SCSLDBG_ << "We don'have any next available command";)
                    SlowCommand *popedCommand = NULL;
                    //we don't have new command so i need to view only if the current command has halted or faulted
                    if (currentExecutingCommand && currentExecutingCommand->runningState > RunningStateType::RS_End) {
                        DEBUG_CODE(SCSLDBG_ << "We need to delete current one and";)
                        //for now we delete it after we need to manage it
                        delete(currentExecutingCommand);
                        DEBUG_CODE(SCSLDBG_ << "We need to delete current one and";)
                        // command has finisced or has fault
                        if(!commandStack.empty() && !commandStack.pop(popedCommand)) {
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
            
            //we need to check for a new
            timeLastCheckCommand = boost::chrono::steady_clock::now() + checkTimeIntervall;
        }
        
        if(!scheduleWorkFlag) {
            // the command has end and the scehduler has been stopped
            canWork = false;
            continue;
        } else {
            //wait until it continue
            SCSLDBG_ << "waith on pauseCondition";
            pauseCondition.wait();
            if(!scheduleWorkFlag) {
                // the command has end and the scehduler has been stopped
                canWork = false;
                continue;
            }
            SCSLDBG_ << "respawn after pauseCondition";
        }
        
        runEnd = boost::chrono::steady_clock::now();
        boost::chrono::milliseconds m = schedulerStepDelay - boost::chrono::duration_cast<boost::chrono::milliseconds>(runEnd-runStart);
        boost::this_thread::sleep_for(m);
    }
    SCSLDBG_ << "Thread terminating so notify conditionWaithSchedulerEnd";
    //notify the end of the thread
    //conditionWaithSchedulerEnd.notify_one();
}

//!install the command
void SlowCommandSandbox::installHandler(SlowCommand *cmdImpl, CDataWrapper* setData) {
    CHAOS_ASSERT(cmdImpl)
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    
    //set current command
    currentExecutingCommand = cmdImpl;
    if(currentExecutingCommand) {
        currentExecutingCommand->sharedChannelSettingPtr = &sharedChannelSetting;
        //associate the keydata storage to the command
        currentExecutingCommand->keyDataStorage = keyDataStorage;
        
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