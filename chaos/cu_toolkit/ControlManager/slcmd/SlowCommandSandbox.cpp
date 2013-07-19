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

SlowCommandSandbox::SlowCommandSandbox() {
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
    schedulerStepDelay = boost::chrono::milliseconds(DEFAULT_TIME_STEP_INTERVALL);
    
    schedulerThread = NULL;
    scheduleWorkFlag = true;
}

// Start the implementation
void SlowCommandSandbox::start() throw(chaos::CException) {
    
    //se the flag to the end o the scheduler
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
    SCSLDBG_ << "Mutext for command scheduler locked";
    
    //se the flag to the end o fthe scheduler
    SCSLAPP_ << "Set scheduler work flag to false";
    scheduleWorkFlag = false;
    
    SCSLAPP_ << "Notify pauseCondition variable";
    pauseCondition.unlock();
    
    //waith that the current command will terminate the work
    //SCSLDBG_ << "Wait on conditionWaithSchedulerEnd";
    conditionWaithSchedulerEnd.wait(lockScheduler);
    
    SCSLAPP_ << "Join on schedulerThread";
    schedulerThread->join();
    SCSLAPP_ << "schedulerThread terminated";
    
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
    
    SCSLAPP_ << "Delete scheduler thread";
    DELETE_OBJ_POINTER(schedulerThread)
    
    SCSLAPP_ << "Scheduler deleted";
    
    SlowCommand *instance = NULL;
    
    //deinit next availabe commad if preset
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdImpl)
    
    //free the remained commands into the stack
    SCSLAPP_ << "Remove command into the stack";
    while (!commandStack.empty()) {
        instance = commandStack.top();
        DELETE_OBJ_POINTER(instance)
    }
    SCSLAPP_ << "Command into the stack removed";
    
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
        curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningState:RunningStateType::RS_End;
        if((curCmdRunningState & (RunningStateType::RS_End|RunningStateType::RS_Fault)) ||
           boost::chrono::steady_clock::now() >= timeLastCheckCommand ) {
            //try to get the lock otherwhise continue with the current control
            boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler, boost::try_to_lock);
            
            DEBUG_CODE(if(!lockScheduler) {SCSLDBG_ << "FAILURE Acquiring lock on mutextCommandScheduler";})
            if(lockScheduler) {
                if(scheduleWorkFlag){  //we still are running)
                    if(nextAvailableCommand.cmdImpl &&  //we have a next available command
                       nextAvailableCommand.cmdImpl->submissionRule >= curCmdRunningState) { //the submission rule match with running state
                        //if the current command is null we simulate and END state
                        if ( curCmdRunningState >= 2 || nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Kill) {
                            DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                            //for now we delete it after we need to manage it
                            if(currentExecutingCommand) {
                                delete(currentExecutingCommand);
                                currentExecutingCommand = NULL;
                                DEBUG_CODE(SCSLDBG_ << "Current executed command is deleted";)
                            }
                            
                            CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                            
                        } else if( currentExecutingCommand && (nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Stack)) {
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
                        DEBUG_CODE(SCSLDBG_ << "nextAvailableCommand.cmdInfo deleted";)
                    } else if(scheduleWorkFlag) {
                        SlowCommand *popedCommand = NULL;
                        //we don't have new command so i need to view only if the current command has halted or faulted
                        if(currentExecutingCommand) {
                            if (currentExecutingCommand->runningState > RunningStateType::RS_End) {
                                DEBUG_CODE(SCSLDBG_ << "We need to delete current one and";)
                                //for now we delete it after we need to manage it
                                delete(currentExecutingCommand);
                                DEBUG_CODE(SCSLDBG_ << "We need to delete current one and";)
                                // command has finisced or has fault
                                currentExecutingCommand = NULL;
                                
                                CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                                //get stack
                                popedCommand = getStackTopOrWaithOnMutext(lockScheduler);
                                //install it or nothing
                                DEBUG_CODE(SCSLDBG_ << "Install paused command";)
                                installHandler(popedCommand, NULL);
                                DEBUG_CODE(SCSLDBG_ << "Paused command installed";)
                            }
                        }else{
                            CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                            popedCommand = getStackTopOrWaithOnMutext(lockScheduler);
                            //install it or nothing
                            DEBUG_CODE(SCSLDBG_ << "Install paused command";)
                            installHandler(popedCommand, NULL);
                            DEBUG_CODE(SCSLDBG_ << "Paused command installed";)
                        }
                    }
                } else {
                    if(currentExecutingCommand ){
                        if(currentExecutingCommand->runningState & RunningStateType::RS_Exsc) {
                            continue;
                        } else {
                            DEBUG_CODE(SCSLDBG_ << "The current command with running state "<< (int)currentExecutingCommand->runningState <<" will be deleted and will terminate for permit thread to exit";)
                            DELETE_OBJ_POINTER(currentExecutingCommand)
                        }
                    }
                    
                    //we have finisched to work
                    // the command has end and the scehduler has been stopped
                    canWork = false;
                    continue;
                }
            } else {
                if(currentExecutingCommand) continue;
                //the has not ben successfull
                DEBUG_CODE(SCSLDBG_ << "lock on mutextCommandScheduler NOT ACQUIRED";)
                if(scheduleWorkFlag) {
                    //wait until it continue
                    DEBUG_CODE(SCSLDBG_ << "waith on pauseCondition";)
                    pauseCondition.wait();
                    
                    CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    
                    DEBUG_CODE(SCSLDBG_ << "respawn after pauseCondition";)
                } else {
                    //we have finisched to work
                    // the command has end and the scehduler has been stopped
                    canWork = false;
                    continue;
                }
            }
            
            //we need to check for a new
            timeLastCheckCommand = boost::chrono::steady_clock::now() + checkTimeIntervall;
        }
        runEnd = boost::chrono::steady_clock::now();
        boost::chrono::milliseconds m = schedulerStepDelay - boost::chrono::duration_cast<boost::chrono::milliseconds>(runEnd-runStart);
        boost::this_thread::sleep_for(m);
    }
    SCSLDBG_ << "Thread terminating so notify conditionWaithSchedulerEnd";
    //notify the end of the thread
    conditionWaithSchedulerEnd.notify_one();
}

SlowCommand* SlowCommandSandbox::getStackTopOrWaithOnMutext(boost::recursive_mutex::scoped_lock& lockScheduler) {
    SlowCommand *result = NULL;
    // pop the latest command
    if(commandStack.empty()) {
        //wait until it continue
        DEBUG_CODE(SCSLDBG_ << "Unlock scheduler mutex";)
        lockScheduler.unlock();
        DEBUG_CODE(SCSLDBG_ << "waith on pauseCondition";)
        pauseCondition.wait();
        DEBUG_CODE(SCSLDBG_ << "respawn after pauseCondition";)
    } else {
        DEBUG_CODE(SCSLDBG_ << "Stack size = " << commandStack.size();)
        result = commandStack.top();
        DEBUG_CODE(SCSLDBG_ << "We got a paused command";)
        commandStack.pop();
    }
    return result;
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
        
        //set the scehdule step delay (time intervall between twp sequnece of the scehduler step)
        if(setData && setData->hasKey(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL)) {
            schedulerStepDelay = boost::chrono::milliseconds(setData->getUInt32Value(SlowCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL));
        } else {
            schedulerStepDelay = boost::chrono::milliseconds(DEFAULT_TIME_STEP_INTERVALL);
        }
        
    } else {
        setHandlerFunctor.cmdInstance = NULL;
        acquireHandlerFunctor.cmdInstance = NULL;
        correlationHandlerFunctor.cmdInstance = NULL;
    }
}

bool SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    
    boost::recursive_mutex::scoped_lock lockScheduler(mutextCommandScheduler);
    if(utility::StartableService::serviceState == utility::InizializableServiceType::IS_DEINTIATED ||
       utility::StartableService::serviceState == utility::InizializableServiceType::IS_DEINITING) return false;
    //delete the waiting command
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdImpl)
    DELETE_OBJ_POINTER(nextAvailableCommand.cmdInfo)
    
    nextAvailableCommand.cmdInfo = cmdInfo;
    nextAvailableCommand.cmdImpl = cmdImpl;
    
    pauseCondition.unlock();
    return true;
}