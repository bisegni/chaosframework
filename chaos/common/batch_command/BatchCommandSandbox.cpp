//
//  BatchCommandSandbox.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//
#include <string>
#include <sched.h>
#include <exception>

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/batch_command/BatchCommandSandbox.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>
#include <chaos/common/batch_command/BatchCommandConstants.h>

using namespace chaos::chrono;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;


//------------------------------------------------------------------------------------------------------------


#define LOG_HEAD_SCS "[BatchCommandSandbox-" << identification << "] "

#define SCSLAPP_ LAPP_ << LOG_HEAD_SCS
#define SCSLDBG_ LDBG_ << LOG_HEAD_SCS
#define SCSLERR_ LERR_ << LOG_HEAD_SCS

#define FUNCTORLERR_ LERR_ << "[BatchCommandSandbox-" << sandbox_identifier <<"] "

#define SET_FAULT(l, c, m, d) \
SET_NAMED_FAULT(l, cmdInstance, c , m , d)

#define SET_NAMED_FAULT(l, n, c, m, d) \
l << c << m << d; \
n->setRunningProperty(RunningPropertyType::RP_Fault); \
n->fault_description.code = c; \
n->fault_description.description = d;

//! Functor implementation
void AcquireFunctor::operator()() {
	try{
		if(cmdInstance && (cmdInstance->runningProperty < RunningPropertyType::RP_End)) cmdInstance->acquireHandler();
	} catch(chaos::CException& ex) {
		SET_FAULT(FUNCTORLERR_, ex.errorCode, ex.errorMessage, ex.errorDomain)
	} catch(std::exception& ex) {
		SET_FAULT(FUNCTORLERR_, -1, ex.what(), "Acquisition Handler");
	} catch(...) {
		SET_FAULT(FUNCTORLERR_, -2, "Unmanaged exception", "Acquisition Handler");
	}
}


void CorrelationFunctor::operator()() {
	try {
		if(cmdInstance && (cmdInstance->runningProperty < RunningPropertyType::RP_End)) (cmdInstance->ccHandler());
	} catch(chaos::CException& ex) {
		SET_FAULT(FUNCTORLERR_, ex.errorCode, ex.errorMessage, ex.errorDomain)
	} catch(std::exception& ex) {
		SET_FAULT(FUNCTORLERR_, -1, ex.what(), "Correlation Handler");
	} catch(...) {
		SET_FAULT(FUNCTORLERR_, -2, "Unmanaged exception", "Correlation Handler");
	}
}

//------------------------------------------------------------------------------------------------------------
CommandInfoAndImplementation::CommandInfoAndImplementation(chaos_data::CDataWrapper *_cmdInfo, BatchCommand *_cmdImpl) {
	cmdInfo = _cmdInfo;
	cmdImpl = _cmdImpl;
}

CommandInfoAndImplementation::~CommandInfoAndImplementation() {
	deleteInfo();
	deleteImpl();
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
//SUBMIT_AND_Stack    = 0,
//SUBMIT_AND_Kill     = 1,
//SUBMIT_NORMAL       = 2

//RS_Exsc     = 0,
//RS_Normal   = 1,
//RS_End      = 2,
//RS_Fault    = 3

//!array precompiled ofr fast decision on what we need to do with runnong and waiting command
/*!
 this precompile jump table permit to know (fastly) what need to to with the runnin command.
 First element need to be the value of the running property of the current command, the second
 is th submission rule
 0 - mean no change to the running command
 1 - stack the command
 2 - kill the command
 3 - wait until the end of the current command
 4 - currenti in end
 5 - current in fault
 */

typedef enum RunningVSSubmissioneResult {
	RSR_NO_CHANGE = 0,
	RSR_TIMED_RETRY,
	RSR_STACK_CURENT_COMMAND,
	RSR_KILL_KURRENT_COMMAND,
	RSR_CURRENT_CMD_HAS_ENDED,
	RSR_CURRENT_CMD_HAS_FAULTED
} RunningVSSubmissioneResult;

static RunningVSSubmissioneResult running_vs_submition[4][3] = {
	/*esclusive running property*/  {RSR_TIMED_RETRY, RSR_TIMED_RETRY, RSR_NO_CHANGE},
	/*normal running property*/     {RSR_STACK_CURENT_COMMAND, RSR_KILL_KURRENT_COMMAND, RSR_NO_CHANGE},
	/*end running property*/        {RSR_CURRENT_CMD_HAS_ENDED, RSR_CURRENT_CMD_HAS_ENDED, RSR_CURRENT_CMD_HAS_ENDED},
	/*fault running property*/      {RSR_CURRENT_CMD_HAS_FAULTED, RSR_CURRENT_CMD_HAS_FAULTED, RSR_CURRENT_CMD_HAS_FAULTED}

};

#define CHECK_RUNNING_VS_SUBMISSION(r,s) running_vs_submition[r][s]

//------------------------------------------------------------------------------------------------------------
#define DEFAULT_STACK_ELEMENT 100
#define DEFAULT_TIME_STEP_INTERVALL 1000000 //1 seconds of delay
#define DEFAULT_CHECK_TIME 500

#define CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE() \
if(!scheduleWorkFlag) { \
canWork = false; \
continue; \
}



BatchCommandSandbox::BatchCommandSandbox() {
}

BatchCommandSandbox::~BatchCommandSandbox() {
}

//! Initialize instance
void BatchCommandSandbox::init(void *initData) throw(chaos::CException) {
	currentExecutingCommand = NULL;
	//setHandlerFunctor.cmdInstance = NULL;
	acquireHandlerFunctor.cmdInstance = NULL;
	acquireHandlerFunctor.sandbox_identifier = identification;

	correlationHandlerFunctor.cmdInstance = NULL;
	correlationHandlerFunctor.sandbox_identifier = identification;

	scheduleWorkFlag = false;
}

// Start the implementation
void BatchCommandSandbox::start() throw(chaos::CException) {

	//se the flag to the end o the scheduler
	SCSLDBG_ << "Set scheduler work flag to true";
	scheduleWorkFlag = true;

	//reset statistic
	std::memset(&stat, 0, sizeof(SandboxStat));

	//allocate thread
	SCSLDBG_ << "Allocate thread for the scheduler and checker";
	threadScheduler.reset(new boost::thread(boost::bind(&BatchCommandSandbox::runCommand, this)));
	threadNextCommandChecker.reset(new boost::thread(boost::bind(&BatchCommandSandbox::checkNextCommand, this)));

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
void BatchCommandSandbox::stop() throw(chaos::CException) {
	//we ned to get the lock on the scheduler
	//boost::recursive_mutex::scoped_lock lockScheduler(mutexNextCommandChecker);
	//SCSLDBG_ << "Lock on mutexNextCommandChecker acquired for stop";

	//se the flag to the end o fthe scheduler
	SCSLAPP_ << "Set scheduler work flag to false";
	scheduleWorkFlag = false;

	SCSLAPP_ << "Notify pauseCondition variable";
	threadSchedulerPauseCondition.unlock();
	waithForNextCheck.unlock();

	//waith that the current command will terminate the work
	//SCSLDBG_ << "Wait on conditionWaithSchedulerEnd";
/*	try{
		conditionWaithSchedulerEnd.wait(lockScheduler);
                
	}catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::condition_error> >& ex) {
		SCSLDBG_<< ex.what();
	}
*/
        if(threadScheduler->joinable()){
            SCSLAPP_ << "Join on schedulerThread";

            threadScheduler->join();
        }
        if(threadNextCommandChecker->joinable()){
            SCSLAPP_ << "Join on threadNextCommandChecker";
            threadNextCommandChecker->join();
        }
	SCSLAPP_ << "schedulerThread terminated";
}

//! Deinit the implementation
void BatchCommandSandbox::deinit() throw(chaos::CException) {
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
	if(event_handler && currentExecutingCommand) event_handler->handleCommandEvent(currentExecutingCommand->element->cmdImpl->unique_id, BatchCommandEventType::EVT_KILLED, NULL);
	DELETE_OBJ_POINTER(currentExecutingCommand)

	//free the remained commands into the stack
	SCSLAPP_ << "Remove paused command into the stack - size:" << commandStack.size();
	while (!commandStack.empty()) {
		nextAvailableCommand = commandStack.top();
		commandStack.pop();
		if(event_handler && currentExecutingCommand) event_handler->handleCommandEvent(nextAvailableCommand->element->cmdImpl->unique_id, BatchCommandEventType::EVT_KILLED, NULL);
		DELETE_OBJ_POINTER(nextAvailableCommand)
	}
	SCSLAPP_ << "Paused command into the stack removed";

	SCSLAPP_ << "Remove waiting command into the queue - size:"<<command_submitted_queue.size();
	while (!command_submitted_queue.empty()) {
		nextAvailableCommand = command_submitted_queue.top();
		command_submitted_queue.pop();
		if(event_handler && currentExecutingCommand) event_handler->handleCommandEvent(nextAvailableCommand->element->cmdImpl->unique_id, BatchCommandEventType::EVT_KILLED, NULL);
		DELETE_OBJ_POINTER(nextAvailableCommand)
	}

	//reset all the handler
	//setHandlerFunctor.cmdInstance = NULL;
	acquireHandlerFunctor.cmdInstance = NULL;
	correlationHandlerFunctor.cmdInstance = NULL;
}

#define WAIT_ON_NEXT_CMD \
lockOnNextCommandMutex.unlock(); \
waithForNextCheck.wait(); \
lockOnNextCommandMutex.lock();

#define TIMED_WAIT_ON_NEXT_CMD(x) \
lockOnNextCommandMutex.unlock(); \
waithForNextCheck.wait(x); \
lockOnNextCommandMutex.lock();

void BatchCommandSandbox::checkNextCommand() {
	bool canWork = scheduleWorkFlag;
	RunningVSSubmissioneResult current_check_value;


	SCSLDBG_ << "[checkNextCommand] checkNextCommand started waith run scheduler notify";
	waithForNextCheck.wait();


	if(!scheduleWorkFlag) {
		SCSLDBG_ << "[checkNextCommand] we need to exit befor start the loop";
		conditionWaithSchedulerEnd.notify_one();
		return;
	}
	SCSLDBG_ << "[checkNextCommand] checkNextCommand can work";
	//manage the lock on next command mutex
	boost::recursive_mutex::scoped_lock lockOnNextCommandMutex(mutexNextCommandChecker);

	while(canWork) {

		if(!command_submitted_queue.empty()){
			if(currentExecutingCommand) {
				PRIORITY_ELEMENT(CommandInfoAndImplementation)  *tmp_command = NULL;
				PRIORITY_ELEMENT(CommandInfoAndImplementation)  *command_to_delete = NULL;
				PRIORITY_ELEMENT(CommandInfoAndImplementation)  *next_available_command = NULL;
				//compute the runnig state or fault
				DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] try to acquire lock";)
				boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand);
				DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] lock acquired";)
				// cehck waht we need to do with current and submitted command
				next_available_command = command_submitted_queue.top();
				DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] got next command";)
				DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] check installation for enw command with pointer =" << std::hex << next_available_command << std::dec;)

				if(next_available_command->element->cmdImpl->implementedHandler()<=1) {
					DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] we have only a set handler";)
					installHandler(next_available_command);
					command_submitted_queue.pop();
					if(event_handler) {
						if(next_available_command->element->cmdImpl->runningProperty == RunningPropertyType::RP_Fault){
							event_handler->handleCommandEvent(next_available_command->element->cmdImpl->unique_id,
															  BatchCommandEventType::EVT_FAULT,
															  static_cast<FaultDescription*>(&next_available_command->element->cmdImpl->fault_description),
															  sizeof(FaultDescription));
						} else {
							event_handler->handleCommandEvent(next_available_command->element->cmdImpl->unique_id, BatchCommandEventType::EVT_COMPLETED, NULL);
						}
					}

					DELETE_OBJ_POINTER(next_available_command);
					continue;
				}

				switch((current_check_value = CHECK_RUNNING_VS_SUBMISSION(currentExecutingCommand->element->cmdImpl->runningProperty,
																		  next_available_command->element->cmdImpl->submissionRule))) {
					case RSR_NO_CHANGE:
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_NO_CHANGE";)
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] so waith for awake signal by other submission or command fault or end";)
						lockForCurrentCommandMutex.unlock();
						WAIT_ON_NEXT_CMD
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] awaked " << __LINE__;)
						continue; //we must recontorl the top element because it could be have changed
						break;

					case RSR_TIMED_RETRY:
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_TIMED_RETRY";)
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] wait the command submission retry period";)
						lockForCurrentCommandMutex.unlock();
						TIMED_WAIT_ON_NEXT_CMD(next_available_command->element->cmdImpl->commandFeatures.featureSubmissionRetryDelay);
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] awaked " << __LINE__;)
						continue; //we must recontorl the top element because it could be have changed
						break;

					case RSR_STACK_CURENT_COMMAND:
						//the stack feature need that the
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_STACK_CURENT_COMMAND";)
						command_submitted_queue.pop();
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] elemente in command_submitted_queue " << command_submitted_queue.size();)
						tmp_command = currentExecutingCommand;
						if(installHandler(next_available_command)) {
							DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] installed command with pointer " << std::hex << next_available_command<< std::dec;)
							DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] push last command into stack";)
							commandStack.push(tmp_command);
							DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] elemente in commandStack " << commandStack.size();)
							if(event_handler) event_handler->handleCommandEvent(tmp_command->element->cmdImpl->unique_id, BatchCommandEventType::EVT_PAUSED, NULL);
						} else {
							//install has not been done
							if(event_handler &&				//there is an handler
							   next_available_command ) {	//the command in not null
								if(next_available_command->element->cmdImpl->runningProperty & RunningPropertyType::RP_Fault) {
									event_handler->handleCommandEvent(next_available_command->element->cmdImpl->unique_id,
																	  BatchCommandEventType::EVT_FAULT,
																	  static_cast<FaultDescription*>(&next_available_command->element->cmdImpl->fault_description),
																	  sizeof(FaultDescription));
								}
								DELETE_OBJ_POINTER(next_available_command);
							}
						}

						threadSchedulerPauseCondition.unlock();
						break;

					case RSR_KILL_KURRENT_COMMAND:
					case RSR_CURRENT_CMD_HAS_ENDED:
					case RSR_CURRENT_CMD_HAS_FAULTED:
						tmp_command = currentExecutingCommand;
						command_submitted_queue.pop();
						DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] elemente in command_submitted_queue " << command_submitted_queue.size();)

						removeHandler(tmp_command);

						if(installHandler(next_available_command)) {
							DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] installed command with pointer " << std::hex << next_available_command<< std::dec;)
							command_to_delete = tmp_command;
						} else {
							//install has not been done
							if(event_handler &&				//there is an handler
							   next_available_command ) {	//the command in not null
								if(next_available_command->element->cmdImpl->runningProperty & RunningPropertyType::RP_Fault) {
									event_handler->handleCommandEvent(next_available_command->element->cmdImpl->unique_id,
																	  BatchCommandEventType::EVT_FAULT,
																	  static_cast<FaultDescription*>(&next_available_command->element->cmdImpl->fault_description),
																	  sizeof(FaultDescription));
								}
								DELETE_OBJ_POINTER(next_available_command);
							}
							DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] something goes wrong n set handler reinstall command wit pointer " << std::hex << tmp_command<< std::dec;)
							installHandler(tmp_command);
						}

						threadSchedulerPauseCondition.unlock();
						break;
				}

				lockForCurrentCommandMutex.unlock();

				//execute the set handler in this separate thread
				switch (current_check_value) {
					case RSR_KILL_KURRENT_COMMAND:
						if(event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																								 BatchCommandEventType::EVT_KILLED,
																								 NULL);
						break;

					case RSR_CURRENT_CMD_HAS_ENDED:
						if(event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																								 BatchCommandEventType::EVT_COMPLETED,
																								 NULL);
						break;

					case RSR_CURRENT_CMD_HAS_FAULTED:
						if(event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																								 BatchCommandEventType::EVT_FAULT,
																								 static_cast<FaultDescription*>(&command_to_delete->element->cmdImpl->fault_description),
																								 sizeof(FaultDescription));
						break;

					default:
						break;
				}
				//delete
				if(command_to_delete) DELETE_OBJ_POINTER(command_to_delete);
			} else {
				PRIORITY_ELEMENT(CommandInfoAndImplementation)  *nextAvailableCommand = command_submitted_queue.top();
				DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] there isn't any current runnig command";)
				DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Install command with pointer " << std::hex << nextAvailableCommand<< std::dec;)
				installHandler(nextAvailableCommand);
				command_submitted_queue.pop();
				DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] elemente in command_submitted_queue " << command_submitted_queue.size();)
				threadSchedulerPauseCondition.unlock();
			}
		} else {
			DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] command submitted queue is empty";)
			DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] try to acquire lock";)
			boost::mutex::scoped_lock lockForCurrentCommandMutex(mutextAccessCurrentCommand);
			DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] lock acquired on mutextAccessCurrentCommand";)
			DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] checking current running command";)
			bool curre_cmd_ended =  currentExecutingCommand && (currentExecutingCommand->element->cmdImpl->runningProperty>=RunningPropertyType::RP_End);

			if(curre_cmd_ended) {
				DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] we have no running or halted command";)
				if(!commandStack.empty()) {
					PRIORITY_ELEMENT(CommandInfoAndImplementation)  *command_to_delete = currentExecutingCommand;
					DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] get and install paused command";)
					PRIORITY_ELEMENT(CommandInfoAndImplementation)  *nextAvailableCommand = commandStack.top();
					DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Install command with pointer " << std::hex << nextAvailableCommand<< std::dec;)
					removeHandler(command_to_delete);
					installHandler(nextAvailableCommand);
					commandStack.pop();
					DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] elemente in commandStack " << commandStack.size();)
					threadSchedulerPauseCondition.unlock();

					switch(command_to_delete->element->cmdImpl->runningProperty) {
						case RunningPropertyType::RP_End:
							if(event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																				BatchCommandEventType::EVT_COMPLETED,
																				NULL);
							break;
						case RunningPropertyType::RP_Fault:
							if(event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																				BatchCommandEventType::EVT_FAULT,
																				static_cast<FaultDescription*>(&command_to_delete->element->cmdImpl->fault_description),
																				sizeof(FaultDescription));

							break;
					}
					DELETE_OBJ_POINTER(command_to_delete);
				} else {
					PRIORITY_ELEMENT(CommandInfoAndImplementation)  *command_to_delete = currentExecutingCommand;
					DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Current command ended or fault without no other command to execute ";)
					removeHandler(command_to_delete);
					installHandler(NULL);
					switch(command_to_delete->element->cmdImpl->runningProperty) {
						case RunningPropertyType::RP_End:
							if(event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																				BatchCommandEventType::EVT_COMPLETED,
																				NULL);
							break;
						case RunningPropertyType::RP_Fault:
							if(event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->unique_id,
																				BatchCommandEventType::EVT_FAULT,
																				static_cast<FaultDescription*>(&command_to_delete->element->cmdImpl->fault_description),
																				sizeof(FaultDescription));

							break;
					}
					DELETE_OBJ_POINTER(command_to_delete);
				}
			}
			DEBUG_CODE(SCSLAPP_ << "[checkNextCommand] unlock the mutextAccessCurrentCommand";)
			//unloc current command mutex
			lockForCurrentCommandMutex.unlock();
			DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] wait undefinitly";)
			WAIT_ON_NEXT_CMD
			DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] awaked " << __LINE__;)
		}

		if(!scheduleWorkFlag) { canWork = false; }
	}

	SCSLAPP_ << "[checkNextCommand] Check next command thread ended";
	SCSLDBG_ << "[checkNextCommand] so we need to -> Notify conditionWaithSchedulerEnd";
	//notify the end of the thread
	conditionWaithSchedulerEnd.notify_one();

}

void BatchCommandSandbox::runCommand() {
	bool canWork = scheduleWorkFlag;
	BatchCommand *curr_executing_impl = NULL;
	//check if the current command has ended or need to be substitute
	boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);
	do{
		if(currentExecutingCommand) {
			curr_executing_impl = currentExecutingCommand->element->cmdImpl;
			stat.lastCmdStepStart = TimingUtil::getTimeStamp();
            if(event_handler) {
                //signal the step of the run
                event_handler->handleSandboxEvent(identification,
                                                  BatchSandboxEventType::EVT_RUN_START,
                                                  &stat.lastCmdStepStart, sizeof(uint64_t));
            }
			// call the acquire phase
			acquireHandlerFunctor();

			//call the correlation and commit phase();
			correlationHandlerFunctor();

			//compute step duration
			stat.lastCmdStepTime = stat.lastCmdStepStart - TimingUtil::getTimeStamp();
            if(event_handler) {
                //signal the step of the run
                event_handler->handleSandboxEvent(identification,
                                                  BatchSandboxEventType::EVT_RUN_END,
                                                  &stat.lastCmdStepTime, sizeof(uint64_t));
            }

			//fire post command step
			curr_executing_impl->commandPost();

			lockForCurrentCommand.unlock();

			//check runnin property
			if(!scheduleWorkFlag && curr_executing_impl->runningProperty) {
				DEBUG_CODE(SCSLDBG_ << "[runCommand need to exit] - The command is not int the exec state...we stop scheduler";)
				canWork = false;
			}else {
				switch(curr_executing_impl->runningProperty) {
					case RunningPropertyType::RP_Exsc:
					case RunningPropertyType::RP_Normal: {
						DEBUG_CODE(SCSLDBG_ << "[runCommand] - waith the delay needed by current command";)
						//adjust a little bit the jitter
						if(curr_executing_impl->commandFeatures.featureSchedulerStepsDelay > 0) {
							int64_t timeToWaith = curr_executing_impl->commandFeatures.featureSchedulerStepsDelay - (stat.lastCmdStepTime*1000);
							threadSchedulerPauseCondition.waitUSec(timeToWaith>0?timeToWaith:0);
						}
						break;
					}

					case RunningPropertyType::RP_Fault:
					case RunningPropertyType::RP_End:
						//put this at null because someone can change it
						curr_executing_impl = NULL;
						DEBUG_CODE(SCSLDBG_ << "[runCommand] - current has ended or fault waithForNextCheck notify";)
						DEBUG_CODE(SCSLDBG_ << "[runCommand] - unlock waithForNextCheck";)
						waithForNextCheck.unlock();
						DEBUG_CODE(SCSLDBG_ << "[runCommand] - wait for new command";)
						threadSchedulerPauseCondition.wait();
						DEBUG_CODE(SCSLDBG_ << "[runCommand] - Scheduler is awaked - 1";)
						break;
				}

				DEBUG_CODE(SCSLDBG_ << "[runCommand] - lock lockForCurrentCommand";)
			}
			lockForCurrentCommand.lock();
			DEBUG_CODE(SCSLDBG_ << "[runCommand] - lockForCurrentCommand acquired with currentExecutingCommand pointer ="<< std::hex << currentExecutingCommand << std::dec;)
		}else {
			//unloc
			if(!scheduleWorkFlag) {
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - no running command and scheduler has been stopped we need to exit";)
				canWork = false;
			} else {
				//no more command to scehdule

				//reset the statistic befor sleep
				std::memset(&stat, 0, sizeof(SandboxStat));

				DEBUG_CODE(SCSLDBG_ << "[runCommand] - unlock lockForCurrentCommand";)
				lockForCurrentCommand.unlock();
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - Scheduler need sleep because no command to run waithForNextCheck notify";)
				waithForNextCheck.unlock();
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - wait for new command";)
				threadSchedulerPauseCondition.wait();
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - Scheduler is awaked - 2";)
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - lock lockForCurrentCommand";)
				lockForCurrentCommand.lock();
				DEBUG_CODE(SCSLDBG_ << "[runCommand] - lockForCurrentCommand acquired with currentExecutingCommand pointer ="<< std::hex << currentExecutingCommand<< std::dec;)
			}
		}

	} while(canWork);

	SCSLDBG_ << "Scheduler thread has finisched";
}

//!install the command
bool BatchCommandSandbox::installHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install) {
	bool installed = true;
	//set current command
	if(cmd_to_install) {
		chaos_data::CDataWrapper *tmp_info = cmd_to_install->element->cmdInfo;
		BatchCommand *tmp_impl = cmd_to_install->element->cmdImpl;

		uint8_t handlerMask = tmp_impl->implementedHandler();
		//install the pointer of th ecommand into the respective handler functor

		//set the shared stat befor cal set handler
		tmp_impl->shared_stat = &stat;

		tmp_impl->commandPre();

		//check set handler
		if(!tmp_impl->already_setupped && (handlerMask & HandlerType::HT_Set)) {
			try {
				tmp_impl->setHandler(tmp_info);
				tmp_impl->already_setupped = true;
			} catch(chaos::CException& ex) {
				installed = false;
				SET_NAMED_FAULT(SCSLERR_, tmp_impl, ex.errorCode, ex.errorMessage, ex.errorDomain)
			} catch(std::exception& ex) {
				installed = false;
				SET_NAMED_FAULT(SCSLERR_, tmp_impl, -1, ex.what(), "Acquisition Handler");
			} catch(...) {
				installed = false;
				SET_NAMED_FAULT(SCSLERR_, tmp_impl, -2, "Unmanaged exception", "Acquisition Handler");
			}
		}

		if(handlerMask <= 1 || !installed) {
			return false;
		}
		//acquire handler
		if(handlerMask & HandlerType::HT_Acquisition) acquireHandlerFunctor.cmdInstance = tmp_impl;

		//correlation commit
		if(handlerMask & HandlerType::HT_Correlation) correlationHandlerFunctor.cmdInstance = tmp_impl;

		currentExecutingCommand = cmd_to_install;

		//fire the running event
        if(event_handler) {
            event_handler->handleCommandEvent(tmp_impl->unique_id, BatchCommandEventType::EVT_RUNNING, NULL);
            //signal the step of the run
            event_handler->handleSandboxEvent(identification,
                                              BatchSandboxEventType::EVT_UPDATE_RUN_DELAY,
                                              &currentExecutingCommand->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay, sizeof(uint64_t));
        }
	} else {
		currentExecutingCommand = NULL;
		acquireHandlerFunctor.cmdInstance = NULL;
		correlationHandlerFunctor.cmdInstance = NULL;
	}
	return installed;
}

void BatchCommandSandbox::removeHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install) {
	if(!cmd_to_install) return;
	BatchCommand *tmp_impl = cmd_to_install->element->cmdImpl;
	uint8_t handlerMask = tmp_impl->implementedHandler();
	if(handlerMask <= 1) {
		//there is only the set handler so we finish here.
		return;
	}

	//acquire handler
	if( (handlerMask & HandlerType::HT_Acquisition) &&
	   acquireHandlerFunctor.cmdInstance == cmd_to_install->element->cmdImpl) acquireHandlerFunctor.cmdInstance = NULL;

	//correlation commit
	if((handlerMask & HandlerType::HT_Correlation)  &&
	   correlationHandlerFunctor.cmdInstance == cmd_to_install->element->cmdImpl) correlationHandlerFunctor.cmdInstance = NULL;
}

void BatchCommandSandbox::killCurrentCommand() {
	//lock the scheduler
	boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);

	// terminate the current command
	currentExecutingCommand->element->cmdImpl->setRunningProperty(RunningPropertyType::RP_End);
}

bool BatchCommandSandbox::enqueueCommand(chaos_data::CDataWrapper *command_to_info, BatchCommand *command_impl, uint32_t priority) {
	CHAOS_ASSERT(command_impl)
	boost::recursive_mutex::scoped_lock lock_checker(mutexNextCommandChecker);
	if(StartableService::serviceState == CUStateKey::DEINIT) return false;

	//
	SCSLDBG_ << "New command enqueue";
	command_submitted_queue.push(new PriorityQueuedElement<CommandInfoAndImplementation>(new CommandInfoAndImplementation(command_to_info, command_impl), priority, true));

	//fire the waiting command
	if(event_handler) event_handler->handleCommandEvent(command_impl->unique_id,
														BatchCommandEventType::EVT_QUEUED,
														(void*)command_impl->command_alias.c_str(),
														(uint32_t)command_impl->command_alias.size());
	lock_checker.unlock();
	waithForNextCheck.unlock();
	return true;
}

//! Command features modification rpc action
void BatchCommandSandbox::setCommandFeatures(features::Features& features) throw (CException) {
    uint64_t thread_step_delay = 0;
    //lock the scheduler
    boost::mutex::scoped_lock lockForCurrentCommand(mutextAccessCurrentCommand);

    //recheck current command
    if(!currentExecutingCommand) return;

    currentExecutingCommand->element->cmdImpl->commandFeatures.featuresFlag |= features.featuresFlag;
    currentExecutingCommand->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay = (thread_step_delay = features.featureSchedulerStepsDelay);

    threadSchedulerPauseCondition.unlock();

    if(event_handler) {
        //signal the step of the run
        event_handler->handleSandboxEvent(identification,
                                          BatchSandboxEventType::EVT_UPDATE_RUN_DELAY,
                                          &thread_step_delay , sizeof(uint64_t));
    }
}
