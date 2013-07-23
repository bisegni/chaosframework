//
//  SlowCommandSandbox.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandSandbox.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandConstants.h>

using namespace chaos::chrono;
using namespace chaos::cu::control_manager::slow_command;

#define DEFAULT_STACK_ELEMENT 100
#define DEFAULT_TIME_STEP_INTERVALL 1000
#define DEFAULT_CHECK_TIME 500

#define CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE() \
if(!scheduleWorkFlag) { \
canWork = false; \
continue; \
}

#define SCSLAPP_ LAPP_ << "[SlowCommandSandbox-" << "] "
#define SCSLDBG_ LDBG_ << "[SlowCommandSandbox-" << "] "
#define SCSLERR_ LERR_ << "[SlowCommandSandbox-" << "] "

SlowCommandSandbox::SlowCommandSandbox():checkTimeIntervall(posix_time::milliseconds(DEFAULT_CHECK_TIME)) {
    //reset all the handler
}

SlowCommandSandbox::~SlowCommandSandbox() {
}

//! Initialize instance
void SlowCommandSandbox::init(void *initData) throw(chaos::CException) {
    
    nextAvailableCommand.cmdInfo = NULL;
    nextAvailableCommand.cmdImpl = NULL;
    
    currentExecutingCommand = NULL;
    //setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
    
    //initialize the shared channel setting
    utility::InizializableService::initImplementation(sharedChannelSetting, initData, "ChannelSetting", "SlowCommandSandbox::init");
    
    SCSLDBG_ << "Get base check time intervall for the scheduler";
    checkTimeIntervall = posix_time::milliseconds(DEFAULT_CHECK_TIME);
    schedulerStepDelay = boost::chrono::milliseconds(DEFAULT_TIME_STEP_INTERVALL);
    
    scheduleWorkFlag = false;
}

// Start the implementation
void SlowCommandSandbox::start() throw(chaos::CException) {
    
    //se the flag to the end o the scheduler
    SCSLDBG_ << "Set scheduler work flag to true";
    scheduleWorkFlag = true;
    
    //allocate thread
    SCSLDBG_ << "Allocate thread for the scheduler and checker";
    threadScheduler.reset(new boost::thread(boost::bind(&SlowCommandSandbox::runCommand, this)));
    threadNextCommandChecker.reset(new boost::thread(boost::bind(&SlowCommandSandbox::checkNextCommand, this)));
}

// Start the implementation
void SlowCommandSandbox::stop() throw(chaos::CException) {
    //we ned to get the lock on the scheduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    SCSLDBG_ << "Lock on mutexNextCommandChecker acquired for stop";
    
    //se the flag to the end o fthe scheduler
    SCSLAPP_ << "Set scheduler work flag to false";
    scheduleWorkFlag = false;
    
    SCSLAPP_ << "Notify pauseCondition variable";
    threadSchedulerPauseCondition.unlock();
    waithForNextCheck.notify_one();
    
    //waith that the current command will terminate the work
    //SCSLDBG_ << "Wait on conditionWaithSchedulerEnd";
    conditionWaithSchedulerEnd.wait(lockScheduler);
    
    SCSLAPP_ << "Join on schedulerThread";
    threadScheduler->join();
    threadNextCommandChecker->join();
    SCSLAPP_ << "schedulerThread terminated";
}

//! Deinit the implementation
void SlowCommandSandbox::deinit() throw(chaos::CException) {
    
    //we ned to get the lock on the scheduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    SCSLDBG_ << "Lock on mutexNextCommandChecker acquired for deinit";
    
    SCSLAPP_ << "Delete scheduler thread";
    threadScheduler.reset();
    threadNextCommandChecker.reset();
    SCSLAPP_ << "Scheduler deleted";
    
    SlowCommand *instance = NULL;
    
    //deinit next availabe commad if preset
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdImpl)
    DELETE_OBJ_POINTER(currentExecutingCommand)
    
    //free the remained commands into the stack
    SCSLAPP_ << "Remove command into the stack";
    while (!commandStack.empty()) {
        instance = commandStack.top();
        DELETE_OBJ_POINTER(instance)
    }
    SCSLAPP_ << "Command into the stack removed";
    
    //initialize the shared channel setting
    utility::InizializableService::deinitImplementation(sharedChannelSetting, "ChannelSetting", "SlowCommandSandbox::init");
    
    //reset all the handler
    //setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
}

void SlowCommandSandbox::checkNextCommand() {
    bool canWork = scheduleWorkFlag;
    uint8_t curCmdRunningState = 0;
    
    //boost::mutex::scoped_lock pauseLock(pauseMutex);
    
    //point to the time for the next check for the available command
    high_resolution_clock::time_point runStart;
    high_resolution_clock::time_point runEnd;
    
    
    while(canWork) {
        //manage the lock on next command mutex
        boost::recursive_mutex::scoped_lock lockOnNextCommandMutex(mutexNextCommandChecker);
        
        //compute the runnig state or fault
        boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand);
        curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningState:RunningStateType::RS_End;
        lockForCurrentCommandMutex.unlock();
        
        if(curCmdRunningState) {
            
            //we have a state that permit to kill ora pause the command
            //check if the next comand need to besubmitted
            if(nextAvailableCommand.cmdImpl){  //we have a next available command
                if(nextAvailableCommand.cmdImpl->submissionRule >= curCmdRunningState) {
                    //we need to submit thewaiting new command
                    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
                    
                    bool hasAcquireOrCC = nextAvailableCommand.cmdImpl->implementedHandler() > 1;
                    
                    //if the current command is null we simulate and END state
                    if ( hasAcquireOrCC && (curCmdRunningState >= SubmissionRuleType::SUBMIT_AND_Kill || nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Kill)) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                        //for now we delete it after we need to manage it
                        if(currentExecutingCommand) {
                            delete(currentExecutingCommand);
                            currentExecutingCommand = NULL;
                            DEBUG_CODE(SCSLDBG_ << "Current executed command is deleted";)
                        }
                        
                        CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                        
                    } else if( hasAcquireOrCC && (currentExecutingCommand && (nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Stack))) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                        //push current command into the stack
                        commandStack.push(currentExecutingCommand);
                        DEBUG_CODE(SCSLDBG_ << "Command stacked";)
                        
                        CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    }
                    
                    //install the new command handler
                    DEBUG_CODE(SCSLDBG_ << "Install next available command";)
                    installHandler(nextAvailableCommand.cmdImpl, nextAvailableCommand.cmdInfo?nextAvailableCommand.cmdInfo->element:NULL);
                    DEBUG_CODE(SCSLDBG_ << "Next available command installed";)
                    
                    //delete the command description
                    nextAvailableCommand.cmdImpl = NULL;
                    DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
                    if(!hasAcquireOrCC) DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
                    DEBUG_CODE(SCSLDBG_ << "nextAvailableCommand.cmdInfo deleted";)
                    
                } else {
                    //submisison rule can't permit to remove the command
                    
                    //check if we need to end
                    CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    
                    //we don't have any available next command ad wee need to sleep for some times
                    waithForNextCheck.timed_wait(lockOnNextCommandMutex, checkTimeIntervall);
                }
            }else {
                //check if we need to end
                CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                
                // we don'nt have a nex command so check if we have some command in pause
                if(!commandStack.empty()) {
                    boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand);
                    
                    DEBUG_CODE(SCSLDBG_ << "We need to install a paused command";)
                    SlowCommand *popedCommand = commandStack.top();
                    
                    commandStack.pop();
                    
                    //install it or nothing
                    DEBUG_CODE(SCSLDBG_ << "Install paused command";)
                    installHandler(popedCommand, NULL);
                    DEBUG_CODE(SCSLDBG_ << "Paused command installed";)
                }
                
                //we don't have any available next command ad wee need to sleep until another command is preset
                waithForNextCheck.wait(lockOnNextCommandMutex);
            }
        } else {
            //command state don't permit to make modification
            DEBUG_CODE(SCSLDBG_ << "command state don't permit to make modification";)
            
            CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
            
            //waith for the next schedule
            if(nextAvailableCommand.cmdImpl) {
                waithForNextCheck.timed_wait(lockOnNextCommandMutex, checkTimeIntervall);
            } else {
                waithForNextCheck.wait(lockOnNextCommandMutex);
            }
        }
        
    }
    SCSLDBG_ << "Thread terminating so notify conditionWaithSchedulerEnd";
    //notify the end of the thread
    conditionWaithSchedulerEnd.notify_one();
}


void SlowCommandSandbox::runCommand() {
    bool canWork = scheduleWorkFlag;
    
    //point to the time for the next check for the available command
    high_resolution_clock::time_point runStart;
    high_resolution_clock::time_point runEnd;
    
    while(canWork) {
        runStart = boost::chrono::steady_clock::now();
        // call the acquire phase
        acquireHandlerFunctor();
        
        //call the correlation and commit phase();
        correlationHandlerFunctor();
        
        //check if the current command has ended or need to be substitute
        boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand,  boost::try_to_lock);
        if(lockForCurrentCommand) {
            curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningState:RunningStateType::RS_End;
            
            if(!scheduleWorkFlag && curCmdRunningState) {
                DEBUG_CODE(SCSLDBG_ << "The command is not int the state of exec and thread nned to be stopped";)
                canWork = false;
                continue;
                
                if(curCmdRunningState & (RunningStateType::RS_End|RunningStateType::RS_Fault)) {
                    threadSchedulerPauseCondition.wait();
                }
            }
        }
        runEnd = boost::chrono::steady_clock::now();
        // - boost::chrono::duration_cast<boost::chrono::milliseconds>(runEnd-runStart)
        boost::this_thread::sleep_for(schedulerStepDelay);
    }
    
    DEBUG_CODE(SCSLDBG_ << "Scheduler thread has finiscehd";)
}

//!install the command
void SlowCommandSandbox::installHandler(SlowCommand *cmdImpl, CDataWrapper* setData) {
    CHAOS_ASSERT(cmdImpl)
    
    //set current command
    currentExecutingCommand = cmdImpl;
    if(currentExecutingCommand) {
        currentExecutingCommand->sharedChannelSettingPtr = &sharedChannelSetting;
        //associate the keydata storage and the device database to the command
        currentExecutingCommand->keyDataStoragePtr = keyDataStoragePtr;
        currentExecutingCommand->deviceDatabasePtr = deviceSchemaDbPtr;
        
        uint8_t handlerMask = currentExecutingCommand->implementedHandler();
        //install the pointer of th ecommand into the respective handler functor
        
        //check set handler
        if(handlerMask & HandlerType::HT_Set) {
            currentExecutingCommand->setHandler(setData);
        }
        
        if(handlerMask <= 1) {
            //there is only the set handler so we finish here.
            return;
        }
        
        //acquire handler
        if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = currentExecutingCommand;
        
        //correlation commit
        if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = currentExecutingCommand;
        
        //set the scehdule step delay (time intervall between twp sequnece of the scehduler step)
        if(setData && setData->hasKey(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL)) {
            schedulerStepDelay = boost::chrono::milliseconds(setData->getUInt32Value(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL));
        } else {
            schedulerStepDelay = boost::chrono::milliseconds(DEFAULT_TIME_STEP_INTERVALL);
        }
        
    } else {
        //setHandlerFunctor.cmdInstance = NULL;
        acquireHandlerFunctor.cmdInstance = NULL;
        correlationHandlerFunctor.cmdInstance = NULL;
    }
}

bool SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    if(!cmdImpl) return false;
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    if(utility::StartableService::serviceState == utility::InizializableServiceType::IS_DEINTIATED ||
       utility::StartableService::serviceState == utility::InizializableServiceType::IS_DEINITING) return false;
    //delete the waiting command
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdImpl)
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
    
    nextAvailableCommand.cmdInfo = cmdInfo;
    nextAvailableCommand.cmdImpl = cmdImpl;
    
    waithForNextCheck.notify_one();
    return true;
}