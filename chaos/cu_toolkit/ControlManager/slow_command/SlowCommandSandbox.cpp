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
CommandInfoAndImplementation::CommandInfoAndImplementation(chaos_data::CDataWrapper *_cmdInfo, SlowCommand *_cmdImpl) {
    cmdInfo = _cmdInfo;
    cmdImpl = _cmdImpl;
}

CommandInfoAndImplementation::~CommandInfoAndImplementation() {
    if(cmdInfo) delete(cmdInfo);
    if(cmdImpl) delete(cmdImpl);
}

void CommandInfoAndImplementation::deleteInfo() {
    if(!cmdInfo) return;
    delete(cmdInfo);
    cmdInfo = NULL;
}

void CommandInfoAndImplementation::deleteImpl() {
    if(!cmdImpl) return;
    delete(cmdImpl);
    cmdImpl = NULL;
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


#define LOG_HEAD "[SlowCommandSandbox-" << deviceSchemaDbPtr->getDeviceID() << "] "

#define SCSLAPP_ LAPP_ << LOG_HEAD
#define SCSLDBG_ LDBG_ << LOG_HEAD
#define SCSLERR_ LERR_ << LOG_HEAD


SlowCommandSandbox::SlowCommandSandbox() {
}

SlowCommandSandbox::~SlowCommandSandbox() {
}

//! Initialize instance
void SlowCommandSandbox::init(void *initData) throw(chaos::CException) {
    currentExecutingCommand = NULL;
    //setHandlerFunctor.cmdInstance = NULL;
    acquireHandlerFunctor.cmdInstance = NULL;
    correlationHandlerFunctor.cmdInstance = NULL;
    
    //initialize the shared channel setting
    utility::InizializableService::initImplementation(sharedAttributeSetting, initData, "AttributeSetting", "SlowCommandSandbox::init");
    
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
    PRIORITY_ELEMENT(CommandInfoAndImplementation)  *nextAvailableCommand = NULL;
    
    //we ned to get the lock on the scheduler
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    SCSLDBG_ << "Lock on mutexNextCommandChecker acquired for deinit";
    
    SCSLAPP_ << "Delete scheduler thread";
    threadScheduler.reset();
    threadNextCommandChecker.reset();
    SCSLAPP_ << "Scheduler thread deleted";
    
    SCSLAPP_ << "clean all paused and waiting command";
    SCSLAPP_ << "Clear the executing command";
    if(event_handler && currentExecutingCommand) event_handler->handleEvent(currentExecutingCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
    DELETE_OBJ_POINTER(currentExecutingCommand)
    
    //free the remained commands into the stack
    SCSLAPP_ << "Remove paused command into the stack";
    while (!commandStack.empty()) {
        nextAvailableCommand = commandStack.top();
        if(event_handler && currentExecutingCommand) event_handler->handleEvent(nextAvailableCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
        DELETE_OBJ_POINTER(nextAvailableCommand)
    }
    SCSLAPP_ << "Paused command into the stack removed";
    
    SCSLAPP_ << "Remove waiting command into the queue";
    while (!command_submitted_queue.empty()) {
        nextAvailableCommand = command_submitted_queue.top();
        if(event_handler && currentExecutingCommand) event_handler->handleEvent(nextAvailableCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
        command_submitted_queue.pop();
        DELETE_OBJ_POINTER(nextAvailableCommand)
    }
    
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
    PRIORITY_ELEMENT(CommandInfoAndImplementation)  *nextAvailableCommand = NULL;
    //boost::mutex::scoped_lock pauseLock(pauseMutex);
    
    //point to the time for the next check for the available command
    high_resolution_clock::time_point runStart;
    high_resolution_clock::time_point runEnd;
    
    SCSLDBG_ << "[checkNextCommand] checkNextCommand started waith run scheduler notify";
    waithForNextCheck.wait();
    SCSLDBG_ << "[checkNextCommand] checkNextCommand can work";
    
    
        //manage the lock on next command mutex
    boost::recursive_mutex::scoped_lock lockOnNextCommandMutex(mutexNextCommandChecker);
    
    while(canWork) {
        
        //compute the runnig state or fault
        boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand, boost::try_to_lock);
        if(lockForCurrentCommandMutex) {
            SCSLDBG_ << "[checkNextCommand] Got lock for check new command";
            curCmdRunningState = currentExecutingCommand?currentExecutingCommand->element->cmdImpl->runningProperty:RunningStateType::RS_End;
            //chec if command has ended
            if(currentExecutingCommand) {
                
                if(curCmdRunningState & RunningStateType::RS_Fault) {
                    //remove handler
                    removeHandler(currentExecutingCommand);
                    SCSLDBG_ << "[checkNextCommand] Current executed command will be deleted because it has fault";
                    if(event_handler) event_handler->handleEvent(currentExecutingCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_FAULT, static_cast<void*>(&currentExecutingCommand->element->cmdImpl->faultDescription));
                    DELETE_OBJ_POINTER(currentExecutingCommand);
                    SCSLDBG_ << "[checkNextCommand] Current executed has been is deleted";
                    
                } else if(curCmdRunningState & RunningStateType::RS_End) {
                    //remove handler
                    removeHandler(currentExecutingCommand);
                    SCSLDBG_ << "[checkNextCommand] Current executed command will be deleted because it is ended";
                    if(event_handler) event_handler->handleEvent(currentExecutingCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_COMPLETED, NULL);
                    DELETE_OBJ_POINTER(currentExecutingCommand);
                    SCSLDBG_ << "[checkNextCommand] Current executed has been deleted";
                }
            }
            lockForCurrentCommandMutex.unlock();
        }else {
            //waith some time
            //boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
            continue;
        };
        
        
        if(curCmdRunningState && (command_submitted_queue.size() ||
                                  commandStack.size())) {
            SCSLDBG_ << "[checkNextCommand] We have or new command o paused and command state permit to check";
            //we need to manage the case in which the ueue is empty
            nextAvailableCommand = command_submitted_queue.empty()?NULL: command_submitted_queue.top();
            //we have a state that permit to kill ora pause the command
            //check if the next comand need to besubmitted
            if(nextAvailableCommand){  //we have a next available command
                SCSLDBG_ << "[checkNextCommand] We have waiting command in queue";
                SlowCommand *tmp_impl = nextAvailableCommand->element->cmdImpl;
                
                if(curCmdRunningState >= tmp_impl->submissionRule) {
                    SCSLDBG_ << "[checkNextCommand] Submission rule match with running property of current command";
                    //we need to submit thewaiting new command
                    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
                    
                    bool hasAcquireOrCC = tmp_impl->implementedHandler() > 1;
                    
                    //if the current command is null we simulate and END state
                    if ( hasAcquireOrCC &&	(curCmdRunningState >= RunningStateType::RS_Kill &&
											 (tmp_impl->submissionRule & SubmissionRuleType::SUBMIT_AND_Kill))) {
                        SCSLDBG_ << "[checkNextCommand] New command that want kill the current one";
                        //for now we delete it after we need to manage it
                        if(currentExecutingCommand) {
							//send the rigth event
							removeHandler(currentExecutingCommand);
                            DELETE_OBJ_POINTER(currentExecutingCommand);
                            if(event_handler) event_handler->handleEvent(tmp_impl->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
                            SCSLDBG_ << "[checkNextCommand] Current executed has been killed by submitted one";
                        }
                        
                        CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                        
                    } else if( hasAcquireOrCC && (currentExecutingCommand &&
                                                  curCmdRunningState >= RunningStateType::RS_Stack &&
												  (tmp_impl->submissionRule & SubmissionRuleType::SUBMIT_AND_Stack))) {
                        SCSLDBG_ << "[checkNextCommand] New command that want pause the current one";
                        //send the rigth event
                        //push current command into the stack
                        commandStack.push(currentExecutingCommand);
                        //fire the paused event
                        if(event_handler) event_handler->handleEvent(tmp_impl->unique_id, SlowCommandEventType::EVT_PAUSED, NULL);
                        
                        SCSLDBG_ << "[checkNextCommand] Command stacked";
                        SCSLDBG_ << "[checkNextCommand] Command in stack = " << commandStack.size();
                        
                        CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    }
                    
                    //install the new command handler
                    SCSLDBG_ << "[checkNextCommand] Install next available command";
                    //first curret element from queue (it is the element that we are checking)
                    command_submitted_queue.pop();
                    installHandler(nextAvailableCommand);
                    
                    SCSLDBG_ << "[checkNextCommand] Next available command installed";
                    SCSLDBG_ << "[checkNextCommand] Command in queue = " << command_submitted_queue.size();
                    
                    if(!hasAcquireOrCC) {
                        DELETE_OBJ_POINTER(nextAvailableCommand)
                        SCSLDBG_ << "[checkNextCommand] Command has been delete it has composed only by set handler";
                    }
                    //fire the scheduler
                    threadSchedulerPauseCondition.unlock();
                    SCSLDBG_ << "[checkNextCommand] unlocked the runCommand";
                } else {
                    //submisison rule can't permit to remove the command
                    SCSLDBG_ << "[checkNextCommand] Submission rule don't match with running property of current command";
                    //check if we need to end
                    CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                    
                    //waith the desidered command time before restry
					lockOnNextCommandMutex.unlock();
                    SCSLDBG_ << "[checkNextCommand] whait " << (tmp_impl->commandFeatures.featureSubmissionRetryDelay*1000) << " microsecond and retry";
                    //delay of the retry is expressed in millisecond but waith class use micro...
                    waithForNextCheck.waitUSec(tmp_impl->commandFeatures.featureSubmissionRetryDelay*1000);
					lockOnNextCommandMutex.lock();
                }
            }else {
                SCSLDBG_ << "[checkNextCommand] We can have paused command";
                //check if we need to end
                CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                
                // we don'nt have a nex command so check if we have some command in pause
                if(!commandStack.empty()) {
                    boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand);
                    
                    SCSLDBG_ << "[checkNextCommand] We need to install a paused command";
                    nextAvailableCommand = commandStack.top();
                    commandStack.pop();
                    SCSLDBG_ << "[checkNextCommand] Command got from stack";
                    SCSLDBG_ << "[checkNextCommand] Command in stack = " << commandStack.size();
                    
                    //install it or nothing
                    SCSLDBG_ << "[checkNextCommand] Install paused command";
                    installHandler(nextAvailableCommand);
                    SCSLDBG_ << "[checkNextCommand] Paused command installed";
                }
                //fire the scheduler
				threadSchedulerPauseCondition.unlock();
                SCSLDBG_ << "[checkNextCommand] unlocked the runCommand";
            }
        } else {
            if(currentExecutingCommand) {
                SCSLDBG_ << "[checkNextCommand] command state don't permit to make modification curCmdRunningState = "<<curCmdRunningState;
                
                CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE()
                
                SCSLDBG_ << "[checkNextCommand] waith some time before try to check incoming command";
                nextAvailableCommand = command_submitted_queue.empty()?NULL: command_submitted_queue.top();
                
                //waith for the next schedule
                if(nextAvailableCommand) {
                    SCSLDBG_ << "[checkNextCommand] we have a waithing command so we need to wait for some time";
                    lockOnNextCommandMutex.unlock();
                    //! delay between two submiossion check
                    //delay of the retry is expressed in millisecond but waith class use micro...
                    waithForNextCheck.waitUSec(nextAvailableCommand->element->cmdImpl->commandFeatures.featureSubmissionRetryDelay*1000);
                    lockOnNextCommandMutex.lock();
                } else {
                    SCSLDBG_ << "[checkNextCommand] we DON'T have a waithing command so we go to sleeping";
                    lockOnNextCommandMutex.unlock();
                    waithForNextCheck.wait();
                    lockOnNextCommandMutex.lock();
                }
            }else {
                SCSLDBG_ << "[checkNextCommand] we DON'T have a waithing command so we go to sleeping";
                lockOnNextCommandMutex.unlock();
                waithForNextCheck.wait();
                lockOnNextCommandMutex.lock();
            }
        }
    }
    
    SCSLAPP_ << "[checkNextCommand] Check next command thread ended";
    SCSLDBG_ << "[checkNextCommand] so we need to -> Notify conditionWaithSchedulerEnd";
    //notify the end of the thread
    conditionWaithSchedulerEnd.notify_one();
    
}


void SlowCommandSandbox::runCommand() {
    bool canWork = scheduleWorkFlag;
    SlowCommand *curr_executing_impl = NULL;
    //check if the current command has ended or need to be substitute
    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
    do{
        if(currentExecutingCommand) {
            curr_executing_impl = currentExecutingCommand->element->cmdImpl;
            stat.lastCmdStepStart = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now().time_since_epoch()).count();
            
            // call the acquire phase
            acquireHandlerFunctor();
            
            //call the correlation and commit phase();
            correlationHandlerFunctor();
 
			//coompute step duration
			stat.lastCmdStepTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now().time_since_epoch()).count()-stat.lastCmdStepStart;
			
            //fire post command step
            curr_executing_impl->command_post_step();
            
            //check runnin property
            if(!scheduleWorkFlag && curr_executing_impl->runningProperty) {
                SCSLDBG_ << "[runCommand need to exit] - The command is not int the exec state...we stop scheduler";
                canWork = false;
			}else {
				//unloc
				SCSLDBG_ << "[runCommand] - unlock lockForCurrentCommand";
				lockForCurrentCommand.unlock();
				if(currentExecutingCommand) {
					//we have a valid command running
					if(curr_executing_impl->runningProperty & (RunningStateType::RS_End|RunningStateType::RS_Fault)) {
						SCSLDBG_ << "[runCommand] - current has ended or fault waithForNextCheck notify";
						waithForNextCheck.unlock();
                        SCSLDBG_ << "[runCommand] - wait for new command";
						threadSchedulerPauseCondition.wait();
						SCSLDBG_ << "[runCommand] - Scheduler is awaked";
					} else {
						SCSLDBG_ << "[runCommand] - command is executing waith his scehdule delay";
						int64_t timeToWaith = curr_executing_impl->commandFeatures.featureSchedulerStepsDelay - stat.lastCmdStepTime;
						threadSchedulerPauseCondition.waitUSec(timeToWaith>0?timeToWaith:0);
					}
				} else {
					SCSLDBG_ << "[runCommand] - Scheduler need sleep waithForNextCheck notify";
					waithForNextCheck.unlock();
                    SCSLDBG_ << "[runCommand] - wait for new command";
					threadSchedulerPauseCondition.wait();
					SCSLDBG_ << "[runCommand] - Scheduler is awaked";
				}
				SCSLDBG_ << "[runCommand] - lock lockForCurrentCommand";
				lockForCurrentCommand.lock();
			}
        }else {
            //unloc
            if(!scheduleWorkFlag) {
                SCSLDBG_ << "[runCommand] - no running command and scheduler has been stopped we need to exit";
                canWork = false;
            } else {
				SCSLDBG_ << "[runCommand] - unlock lockForCurrentCommand";
				lockForCurrentCommand.unlock();
				SCSLDBG_ << "[runCommand] - Scheduler need sleep because no command to run waithForNextCheck notify";
				waithForNextCheck.unlock();
                SCSLDBG_ << "[runCommand] - wait for new command";
				threadSchedulerPauseCondition.wait();
				SCSLDBG_ << "[runCommand] - Scheduler is awaked";
				SCSLDBG_ << "[runCommand] - lock lockForCurrentCommand";
				lockForCurrentCommand.lock();
			}
        }

    } while(canWork);
    
    SCSLDBG_ << "Scheduler thread has finiscehd";
}

//!install the command
void SlowCommandSandbox::installHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install) {
    
    //set current command
    if(cmd_to_install) {
        chaos_data::CDataWrapper *tmp_info = cmd_to_install->element->cmdInfo;
        SlowCommand *tmp_impl = cmd_to_install->element->cmdImpl;
        
        tmp_impl->sharedAttributeSettingPtr = &sharedAttributeSetting;
        //associate the keydata storage and the device database to the command
        tmp_impl->keyDataStoragePtr = keyDataStoragePtr;
        tmp_impl->deviceDatabasePtr = deviceSchemaDbPtr;
        
        uint8_t handlerMask = tmp_impl->implementedHandler();
        //install the pointer of th ecommand into the respective handler functor
        
		//set the shared stat befor cal set handler
		tmp_impl->shared_stat = &stat;
        
            //check set handler
        if(!tmp_impl->already_setupped && (handlerMask & HandlerType::HT_Set)) {
			try {
				tmp_impl->setHandler(tmp_info);
                tmp_impl->already_setupped = true;
			} catch(chaos::CException& ex) {
				SET_NAMED_FAULT(tmp_impl, ex.errorCode, ex.errorMessage, ex.errorDomain)
			} catch(std::exception& ex) {
				SET_NAMED_FAULT(tmp_impl, -1, ex.what(), "Acquisition Handler");
			} catch(...) {
				SET_NAMED_FAULT(tmp_impl, -2, "Unmanaged exception", "Acquisition Handler");
			}
        }
        
        if(handlerMask <= 1) {
            return;
        }
        //acquire handler
        if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = tmp_impl;
        
        //correlation commit
        if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = tmp_impl;
		
        currentExecutingCommand = cmd_to_install;
		
		//fire the running event
		if(event_handler)event_handler->handleEvent(tmp_impl->unique_id, SlowCommandEventType::EVT_RUNNING, NULL);
		
		//exec the signal start in command
		tmp_impl->command_start();
    } else {
        currentExecutingCommand = NULL;
        acquireHandlerFunctor.cmdInstance = NULL;
        correlationHandlerFunctor.cmdInstance = NULL;
    }
}

void SlowCommandSandbox::removeHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install) {
    if(!cmd_to_install) return;
    SlowCommand *tmp_impl = cmd_to_install->element->cmdImpl;
    uint8_t handlerMask = tmp_impl->implementedHandler();
    if(handlerMask <= 1) {
        //there is only the set handler so we finish here.
        return;
    }
    
    //acquire handler
    if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = NULL;
    
    //correlation commit
    if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = NULL;
}

void SlowCommandSandbox::killCurrentCommand() {
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
	
	// terminate the current command
	DELETE_OBJ_POINTER(currentExecutingCommand)
	installHandler(NULL);
	
	//fire the killed event
	if(event_handler) event_handler->handleEvent(currentExecutingCommand->element->cmdImpl->unique_id, SlowCommandEventType::EVT_KILLED, NULL);
}

bool SlowCommandSandbox::enqueueCommand(chaos_data::CDataWrapper *command_to_info, SlowCommand *command_impl, uint32_t priority) {
    CHAOS_ASSERT(command_impl)
    boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
    if(utility::StartableService::serviceState == ::chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED) return false;
    
    //
	SCSLDBG_ << "New command enqueue";
    command_submitted_queue.push(new PriorityQueuedElement<CommandInfoAndImplementation>(new CommandInfoAndImplementation(command_to_info, command_impl), priority, true));
    
	//fire the waiting command
    if(event_handler) event_handler->handleEvent(command_impl->unique_id, SlowCommandEventType::EVT_QUEUED, NULL);
	lockScheduler.unlock();
    waithForNextCheck.unlock();
    return true;
}