//
//  SlowCommandSandbox.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <sched.h>
#include <exception>

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandSandbox.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandConstants.h>

using namespace chaos::chrono;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;


//------------------------------------------------------------------------------------------------------------
#define FUNCTORLERR_ LERR_ << "[SlowCommandSandbox-" << "] "

#define SET_FAULT(c, m, d) \
SET_NAMED_FAULT(cmdInstance, c , m , d)

#define SET_NAMED_FAULT(n, c, m, d) \
FUNCTORLERR_ << c << m << d; \
n->setRunningProperty(n->getRunningProperty()|RunningStateType::RS_Fault); \
n->faultDescription.code = c; \
n->faultDescription.description = m; \
n->faultDescription.domain = d;

//! Functor implementation
void AcquireFunctor::operator()() {
    try{
        if(cmdInstance && (cmdInstance->runningProperty < RunningStateType::RS_End)) cmdInstance->acquireHandler();
    } catch(chaos::CException& ex) {
        SET_FAULT(ex.errorCode, ex.errorMessage, ex.errorDomain)
    } catch(std::exception& ex) {
        SET_FAULT(-1, ex.what(), "Acquisition Handler");
    } catch(...) {
        SET_FAULT(-2, "Unmanaged exception", "Acquisition Handler");
    }
}


void CorrelationFunctor::operator()() {
    try {
        if(cmdInstance && (cmdInstance->runningProperty < RunningStateType::RS_End)) (cmdInstance->ccHandler());
    } catch(chaos::CException& ex) {
        SET_FAULT(ex.errorCode, ex.errorMessage, ex.errorDomain)
    } catch(std::exception& ex) {
        SET_FAULT(-1, ex.what(), "Acquisition Handler");
    } catch(...) {
        SET_FAULT(-2, "Unmanaged exception", "Acquisition Handler");
    }
}

//------------------------------------------------------------------------------------------------------------
#define DEFAULT_STACK_ELEMENT 100
#define DEFAULT_TIME_STEP_INTERVALL 1000000 //1 seconds of delay
#define DEFAULT_CHECK_TIME 500

#define CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE() \
if(!scheduleWorkFlag) { \
canWork = false; \
continue; \
}

#define SCSLAPP_ LAPP_ << "[SlowCommandSandbox-" << "] "
#define SCSLDBG_ LDBG_ << "[SlowCommandSandbox-" << "] "
#define SCSLERR_ LERR_ << "[SlowCommandSandbox-" << "] "


SlowCommandSandbox::SlowCommandSandbox():submissionRetryDelay(posix_time::milliseconds(DEFAULT_CHECK_TIME)) {
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
    utility::InizializableService::initImplementation(sharedAttributeSetting, initData, "AttributeSetting", "SlowCommandSandbox::init");
    
    SCSLDBG_ << "Get base check time intervall for the scheduler";
    submissionRetryDelay = posix_time::milliseconds(DEFAULT_CHECK_TIME);
    
    //schedulerStepDelay = DEFAULT_TIME_STEP_INTERVALL;
    
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
	
	//set the scheduler thread priority
#if defined(__linux__) || defined(__APPLE__)
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) threadScheduler->native_handle();
    if (!pthread_getschedparam(threadID, &policy, &param))  {
		DEBUG_CODE(SCSLDBG_ << "Default thread scheduler policy";)
        DEBUG_CODE(SCSLDBG_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
											 (policy == SCHED_RR)    ? "SCHED_RR" :
											 (policy == SCHED_OTHER) ? "SCHED_OTHER" :
											 "???");)
		DEBUG_CODE(SCSLDBG_ << "priority " << param.sched_priority;)
		
		policy = SCHED_RR;
		param.sched_priority = sched_get_priority_max(SCHED_RR);
		if (!pthread_setschedparam(threadID, policy, &param)) {
			//successfull setting schedule priority to the thread
			DEBUG_CODE(SCSLDBG_ << "new thread scheduler policy";)
			DEBUG_CODE(SCSLDBG_ << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
												 (policy == SCHED_RR)    ? "SCHED_RR" :
												 (policy == SCHED_OTHER) ? "SCHED_OTHER" :
												 "???");)
			DEBUG_CODE(SCSLDBG_ << "priority " << param.sched_priority;)
			
		}
	}
#endif
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
    waithForNextCheck.unlock();
    
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
    SCSLAPP_ << "Scheduler thread deleted";
    
    SlowCommand *instance = NULL;
    
    //deinit next availabe commad if preset
	if(event_handler && currentExecutingCommand) event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
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
    utility::InizializableService::deinitImplementation(sharedAttributeSetting, "AttributeSetting", "SlowCommandSandbox::init");
    
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
        boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand, boost::try_to_lock);
        if(lockForCurrentCommandMutex) {
            curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningProperty:RunningStateType::RS_End;
            lockForCurrentCommandMutex.unlock();
        }else {
            continue;
        };
        
        if(curCmdRunningState) {
            
            //we have a state that permit to kill ora pause the command
            //check if the next comand need to besubmitted
            if(nextAvailableCommand.cmdImpl){  //we have a next available command
                if(curCmdRunningState >= nextAvailableCommand.cmdImpl->submissionRule) {
                    //we need to submit thewaiting new command
                    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
                    
                    bool hasAcquireOrCC = nextAvailableCommand.cmdImpl->implementedHandler() > 1;
                    
                    //if the current command is null we simulate and END state
                    if ( hasAcquireOrCC &&	(curCmdRunningState >= SubmissionRuleType::SUBMIT_AND_Kill ||
											 (nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Kill))) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want kill the current one";)
                        //for now we delete it after we need to manage it
                        if(currentExecutingCommand) {
							//send the rigth event
							if(currentExecutingCommand->runningProperty & RunningStateType::RS_Fault) {
								if(event_handler) event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_FAULT, static_cast<void*>(&currentExecutingCommand->faultDescription));
								
							} else if(currentExecutingCommand->runningProperty & RunningStateType::RS_End) {
								if(event_handler) event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_COMPLETED, NULL);
							}
							
                            delete(currentExecutingCommand);
                            currentExecutingCommand = NULL;
                            DEBUG_CODE(SCSLDBG_ << "Current executed command is deleted";)
                        }
                        
                        CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                        
                    } else if( hasAcquireOrCC && (currentExecutingCommand &&
												  (nextAvailableCommand.cmdImpl->submissionRule & SubmissionRuleType::SUBMIT_AND_Stack))) {
                        DEBUG_CODE(SCSLDBG_ << "New command that want pause the current one";)
                        //push current command into the stack
                        commandStack.push(currentExecutingCommand);
						//fire the paused event
						if(event_handler) event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_PAUSED, NULL);
						
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
                    if(!hasAcquireOrCC) {
                        DELETE_OBJ_POINTER(nextAvailableCommand.cmdImpl)
                        DEBUG_CODE(SCSLDBG_ << "cmdImpl deleted";)
                    }
                    //fire the scheduler
                    threadSchedulerPauseCondition.unlock();
                } else {
                    //submisison rule can't permit to remove the command
                    
                    //check if we need to end
                    CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    
                    //we don't have any available next command ad wee need to sleep for some times
					lockOnNextCommandMutex.unlock();
                    waithForNextCheck.waitUSec(submissionRetryDelay.total_microseconds());
					lockOnNextCommandMutex.lock();
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
                //fire the scheduler
				threadSchedulerPauseCondition.unlock();
            }
        } else {
            //command state don't permit to make modification
            DEBUG_CODE(SCSLDBG_ << "command state don't permit to make modification";)
            
            CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
            
            //waith for the next schedule
            if(nextAvailableCommand.cmdImpl) {
				DEBUG_CODE(SCSLDBG_ << "we have a waithing command so we need to wait for some time";)
				lockOnNextCommandMutex.unlock();
                waithForNextCheck.waitUSec(submissionRetryDelay.total_microseconds());
				lockOnNextCommandMutex.lock();
            } else {
				DEBUG_CODE(SCSLDBG_ << "we DON'T have a waithing command so we go to sleeping";)
				lockOnNextCommandMutex.unlock();
                waithForNextCheck.wait();
				lockOnNextCommandMutex.lock();
            }
        }
        
    }
    SCSLDBG_ << "Thread terminating so notify conditionWaithSchedulerEnd";
    //notify the end of the thread
    conditionWaithSchedulerEnd.notify_one();
}


void SlowCommandSandbox::runCommand() {
    bool canWork = scheduleWorkFlag;
    
    //check if the current command has ended or need to be substitute
    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
    do{
        stat.lastCmdStepStart = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now().time_since_epoch()).count();
		
        // call the acquire phase
        acquireHandlerFunctor();
        
        //call the correlation and commit phase();
        correlationHandlerFunctor();
		
		//fire post command step
		if(currentExecutingCommand) currentExecutingCommand->command_post_step();
		
        curCmdRunningState = currentExecutingCommand?currentExecutingCommand->runningProperty:RunningStateType::RS_End;
		if(!scheduleWorkFlag && curCmdRunningState) {
            DEBUG_CODE(SCSLDBG_ << "The command is not int the state of exec and thread need to be stopped";)
            canWork = false;
            continue;
            
            if(curCmdRunningState & (RunningStateType::RS_End|RunningStateType::RS_Fault)) {
                DEBUG_CODE(SCSLDBG_ << "Scheduler need sleep because no command to run";)
				waithForNextCheck.unlock();
                threadSchedulerPauseCondition.wait();
                DEBUG_CODE(SCSLDBG_ << "Scheduler is awaked";)
            }
        }

		//unloc
        lockForCurrentCommand.unlock();
		
		if(currentExecutingCommand && curCmdRunningState & (RunningStateType::RS_End|RunningStateType::RS_Fault)) {
			DEBUG_CODE(SCSLDBG_ << "Scheduler need sleep because no command to run";)
			waithForNextCheck.unlock();
			threadSchedulerPauseCondition.wait();
			DEBUG_CODE(SCSLDBG_ << "Scheduler is awaked";)
		}
		
        stat.lastCmdStepTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now().time_since_epoch()).count()-stat.lastCmdStepStart;
        //
        uint64_t sw = (uint64_t)currentExecutingCommand;
        switch (sw) {
            case 0:
                DEBUG_CODE(SCSLDBG_ << "Scheduler need sleep because no command to run";)
				waithForNextCheck.unlock();
                threadSchedulerPauseCondition.wait();
                DEBUG_CODE(SCSLDBG_ << "Scheduler is awaked";)
                break;
                
            default:
				int64_t timeToWaith = currentExecutingCommand->commandFeatures.featureSchedulerStepsDelay - stat.lastCmdStepTime;
                threadSchedulerPauseCondition.waitUSec(timeToWaith>0?timeToWaith:0);
                //boost::this_thread::sleep_for(boost::chrono::milliseconds(currentExecutingCommand->commandFeatures.featureSchedulerStepsDelay - stat.lastCmdStepTime));
                break;
        }
        
        lockForCurrentCommand.lock();
    } while(canWork);
    
    DEBUG_CODE(SCSLDBG_ << "Scheduler thread has finiscehd";)
}

//!install the command
void SlowCommandSandbox::installHandler(SlowCommand *cmdImpl, CDataWrapper* setData) {
    
    //set current command
    if(cmdImpl) {
        cmdImpl->sharedAttributeSettingPtr = &sharedAttributeSetting;
        //associate the keydata storage and the device database to the command
        cmdImpl->keyDataStoragePtr = keyDataStoragePtr;
        cmdImpl->deviceDatabasePtr = deviceSchemaDbPtr;
        
        uint8_t handlerMask = cmdImpl->implementedHandler();
        //install the pointer of th ecommand into the respective handler functor
        
		//set the shared stat befor cal set handler
		cmdImpl->shared_stat = &stat;
		
        //check set handler
        if(handlerMask & HandlerType::HT_Set) {
			try {
				cmdImpl->setHandler(setData);
			} catch(chaos::CException& ex) {
				SET_NAMED_FAULT(cmdImpl, ex.errorCode, ex.errorMessage, ex.errorDomain)
			} catch(std::exception& ex) {
				SET_NAMED_FAULT(cmdImpl, -1, ex.what(), "Acquisition Handler");
			} catch(...) {
				SET_NAMED_FAULT(cmdImpl, -2, "Unmanaged exception", "Acquisition Handler");
			}
        }
        
        if(handlerMask <= 1) {
            //there is only the set handler so we finish here.
            return;
        }
        
        //acquire handler
        if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = cmdImpl;
        
        //correlation commit
        if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = cmdImpl;
		
        currentExecutingCommand = cmdImpl;
		
		//fire the running event
		if(event_handler)event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_RUNNING, NULL);
		
		//exec the signal start in command
		cmdImpl->command_start();
    } else {
        currentExecutingCommand = NULL;
        acquireHandlerFunctor.cmdInstance = NULL;
        correlationHandlerFunctor.cmdInstance = NULL;
    }
}


void SlowCommandSandbox::killCurrentCommand() {
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
	
	// terminate the current command
	DELETE_OBJ_POINTER(currentExecutingCommand)
	installHandler(NULL, NULL);
	
	//fire the killed event
	if(event_handler) event_handler->handleEvent(currentExecutingCommand->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
}

bool SlowCommandSandbox::setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl) {
    if(!cmdImpl) return false;
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    if(utility::StartableService::serviceState == ::chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED) return false;
    
    nextAvailableCommand.cmdInfo = cmdInfo;
    nextAvailableCommand.cmdImpl = cmdImpl;
    //set the schedule step delay (time intervall between twp sequnece of the scehduler step)
    if((cmdImpl->commandFeatures.featuresFlag & features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY) == 0) {
        //we need to set a new delay between steps
        cmdImpl->commandFeatures.featureSchedulerStepsDelay =  DEFAULT_TIME_STEP_INTERVALL;
        DEBUG_CODE(SCSLDBG_ << "default scheduler delay has been installed with " << DEFAULT_TIME_STEP_INTERVALL << " microseconds";)
        
    }
    //check if the command has it's own time for the checker
    if(cmdImpl->commandFeatures.featuresFlag & features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY) {
        //we need to set a new delay between steps
        submissionRetryDelay = posix_time::milliseconds(cmdImpl->commandFeatures.featureSubmissionRetryDelay);
        DEBUG_CODE(SCSLDBG_ << "New checker delay has been installed with value of " << cmdImpl->commandFeatures.featureSubmissionRetryDelay << " milliseconds";)
    }else {
        submissionRetryDelay = posix_time::milliseconds(DEFAULT_CHECK_TIME);
        DEBUG_CODE(SCSLDBG_ << "Default checker delay has been used with value of " << DEFAULT_CHECK_TIME  << " milliseconds";)
    }
	//fire the waiting command
    if(event_handler) event_handler->handleEvent(cmdImpl->unique_id, SlowCommandEventType::EVT_WAITING, NULL);
	lockScheduler.unlock();
    waithForNextCheck.unlock();
    return true;
}