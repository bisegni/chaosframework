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

using namespace chaos::common::data;
using namespace chaos::common::pqueue;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;


//------------------------------------------------------------------------------------------------------------


#define LOG_HEAD_SCS "[BatchCommandSandbox-" << identification << "] "

#define SCSLAPP_ LAPP_ << LOG_HEAD_SCS
#define SCSLDBG_ LDBG_ << LOG_HEAD_SCS
#define SCSLERR_ LERR_ << LOG_HEAD_SCS

#define FUNCTORLERR_ LERR_ << "[BatchCommandSandbox-" << sandbox_identifier <<"] "

#define SET_FAULT(l, c, m, d) \
SET_NAMED_FAULT(l, cmd_instance, c , m , d)

#define SET_NAMED_FAULT(l, n, c, m, d) \
l << c << m << d; \
n->setRunningProperty(RunningPropertyType::RP_Fault); \
n->fault_description.code = c; \
n->fault_description.description = m; \
n->fault_description.domain = d;
//! Functor implementation

void AcquireFunctor::operator()() {
    try {
        if (cmd_instance && (cmd_instance->runningProperty < RunningPropertyType::RP_End)) cmd_instance->acquireHandler();
    } catch (chaos::CException& ex) {
        SET_FAULT(FUNCTORLERR_, ex.errorCode, ex.errorMessage, ex.errorDomain)
    } catch (std::exception& ex) {
        SET_FAULT(FUNCTORLERR_, -1, ex.what(), "Acquisition Handler:"+cmd_instance->getAlias());
    } catch (...) {
        SET_FAULT(FUNCTORLERR_, -2, "Unmanaged exception", "Acquisition Handler:"+cmd_instance->getAlias());
    }
}

void CorrelationFunctor::operator()() {
    try {
        if (cmd_instance && (cmd_instance->runningProperty < RunningPropertyType::RP_End)) (cmd_instance->ccHandler());
    } catch (chaos::CException& ex) {
        SET_FAULT(FUNCTORLERR_, ex.errorCode, ex.errorMessage, ex.errorDomain)
    } catch (std::exception& ex) {
        SET_FAULT(FUNCTORLERR_, -1, ex.what(), "Correlation Handler");
    } catch (...) {
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
    if (!cmdInfo) return;
    delete(cmdInfo);
    cmdInfo = NULL;
}

void CommandInfoAndImplementation::deleteImpl() {
    if (!cmdImpl) return;
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
    /*esclusive running property*/
    {RSR_TIMED_RETRY, RSR_TIMED_RETRY, RSR_NO_CHANGE},
    /*normal running property*/
    {RSR_STACK_CURENT_COMMAND, RSR_KILL_KURRENT_COMMAND, RSR_NO_CHANGE},
    /*end running property*/
    {RSR_CURRENT_CMD_HAS_ENDED, RSR_CURRENT_CMD_HAS_ENDED, RSR_CURRENT_CMD_HAS_ENDED},
    /*fault running property*/
    {RSR_CURRENT_CMD_HAS_FAULTED, RSR_CURRENT_CMD_HAS_FAULTED, RSR_CURRENT_CMD_HAS_FAULTED}
    
};

#define CHECK_RUNNING_VS_SUBMISSION(r,s) running_vs_submition[r][s]

//------------------------------------------------------------------------------------------------------------
#define DEFAULT_STACK_ELEMENT 100
#define DEFAULT_TIME_STEP_INTERVALL 1000000 //1 seconds of delay
#define DEFAULT_CHECK_TIME 500

#define CHECK_END_OF_SCHEDULER_WORK_AND_CONTINUE() \
if(!schedule_work_flag) { \
canWork = false; \
continue; \
}

BatchCommandSandbox::BatchCommandSandbox():
default_sticky_command(NULL){}

BatchCommandSandbox::~BatchCommandSandbox() {}

//! Initialize instance

void BatchCommandSandbox::init(void *initData) throw (chaos::CException) {
    current_executing_command = NULL;
    
    acquire_handler_functor.cmd_instance = NULL;
    acquire_handler_functor.sandbox_identifier = identification;
    
    correlation_handler_functor.cmd_instance = NULL;
    correlation_handler_functor.sandbox_identifier = identification;
    
    schedule_work_flag = false;
}

// Start the implementation

void BatchCommandSandbox::start() throw (chaos::CException) {
    
    //se the flag to the end o the scheduler
    SCSLDBG_ << "Set scheduler work flag to true";
    schedule_work_flag = true;
    
    //reset statistic
    std::memset(&stat, 0, sizeof (SandboxStat));
    
    //allocate thread
    thread_scheduler.reset(new boost::thread(boost::bind(&BatchCommandSandbox::runCommand, this)));
    thread_next_command_checker.reset(new boost::thread(boost::bind(&BatchCommandSandbox::checkNextCommand, this)));
    DEBUG_CODE(SCSLDBG_ << "Allocated thread for the scheduler runCommand "<<std::hex<<thread_scheduler->native_handle()<<" and checker "<< thread_next_command_checker->native_handle()<<std::dec;);
    
    //set the scheduler thread priority
#if defined(__linux__) || defined(__APPLE__)
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) thread_scheduler->native_handle();
    if (!pthread_getschedparam(threadID, &policy, &param)) {
        DEBUG_CODE(SCSLDBG_ << "Default thread scheduler policy";)
        DEBUG_CODE(SCSLDBG_ << "policy=" << ((policy == SCHED_FIFO) ? "SCHED_FIFO" :
                                             (policy == SCHED_RR) ? "SCHED_RR" :
                                             (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                             "???");)
        DEBUG_CODE(SCSLDBG_ << "priority " << param.sched_priority;)
        
        policy = SCHED_RR;
        param.sched_priority = sched_get_priority_max(SCHED_RR);
        if (!pthread_setschedparam(threadID, policy, &param)) {
            //successfull setting schedule priority to the thread
            DEBUG_CODE(SCSLDBG_ << "new thread scheduler policy";)
            DEBUG_CODE(SCSLDBG_ << "policy=" << ((policy == SCHED_FIFO) ? "SCHED_FIFO" :
                                                 (policy == SCHED_RR) ? "SCHED_RR" :
                                                 (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                 "???");)
            DEBUG_CODE(SCSLDBG_ << "priority " << param.sched_priority;)
            
        }
    }
#endif
}

// Start the implementation

void BatchCommandSandbox::stop() throw (chaos::CException) {
    //we ned to get the lock on the scheduler
    
    //se the flag to the end o fthe scheduler
    SCSLAPP_ << "Set scheduler work flag to false";
    schedule_work_flag = false;
    
    SCSLAPP_ << "Notify pauseCondition variable";
    thread_scheduler_pause_condition.unlock();
    whait_for_next_check.unlock();
    
    if (thread_scheduler->joinable()) {
        SCSLAPP_ << "Join on schedulerThread";
        
        thread_scheduler->join();
    }
    if (thread_next_command_checker->joinable()) {
        SCSLAPP_ << "Join on thread_next_command_checker";
        thread_next_command_checker->join();
    }
    SCSLAPP_ << "schedulerThread terminated";
}

//! Deinit the implementation

void BatchCommandSandbox::deinit() throw (chaos::CException) {
    PRIORITY_ELEMENT(CommandInfoAndImplementation) *nextAvailableCommand = NULL;
    
    SCSLAPP_ << "Delete scheduler thread";
    thread_scheduler.reset();
    thread_next_command_checker.reset();
    SCSLAPP_ << "Scheduler thread deleted";
    
    SCSLAPP_ << "clean all paused and waiting command";
    SCSLAPP_ << "Clear the executing command";
    if (current_executing_command &&
        current_executing_command->element->cmdImpl->sticky == false) {
        if(event_handler) {
            event_handler->handleCommandEvent(current_executing_command->element->cmdImpl->command_alias,
                                              current_executing_command->element->cmdImpl->unique_id,
                                              BatchCommandEventType::EVT_KILLED, NULL);
        }
        DELETE_OBJ_POINTER(current_executing_command)
    }
    
    //free the remained commands into the stack
    SCSLAPP_ << "Remove paused command into the stack - size:" << command_stack.size();
    while (!command_stack.empty()) {
        nextAvailableCommand = command_stack.top();
        command_stack.pop();
        if(nextAvailableCommand == NULL) continue;
        if(nextAvailableCommand->element->cmdImpl->sticky == true) continue;
        if (event_handler) {
            event_handler->handleCommandEvent(current_executing_command->element->cmdImpl->command_alias,
                                              nextAvailableCommand->element->cmdImpl->unique_id,
                                              BatchCommandEventType::EVT_KILLED, NULL);
        }
        DELETE_OBJ_POINTER(nextAvailableCommand);
    }
    SCSLAPP_ << "Paused command into the stack removed";
    
    SCSLAPP_ << "Remove waiting command into the queue - size:" << command_submitted_queue.size();
    while (!command_submitted_queue.empty()) {
        nextAvailableCommand = command_submitted_queue.top();
        command_submitted_queue.pop();
        if (event_handler && current_executing_command) event_handler->handleCommandEvent(current_executing_command->element->cmdImpl->command_alias,
                                                                                          nextAvailableCommand->element->cmdImpl->unique_id,
                                                                                          BatchCommandEventType::EVT_KILLED, NULL);
        DELETE_OBJ_POINTER(nextAvailableCommand)
    }
    
    //reset all the handler
    acquire_handler_functor.cmd_instance = NULL;
    correlation_handler_functor.cmd_instance = NULL;
}

void BatchCommandSandbox::setDefaultStickyCommand(BatchCommand *instance) {
    if(instance != NULL) {
        default_sticky_command.reset(new PriorityQueuedElement<CommandInfoAndImplementation>(new CommandInfoAndImplementation(NULL, instance),
                                                                                             instance->unique_id,
                                                                                             0,
                                                                                             true));
        
        if(default_sticky_command.get()) {
            //set sticky feature //settable only by sandbox
            default_sticky_command->element->cmdImpl->sticky = true;
        }
    }
}

#define WAIT_ON_NEXT_CMD \
whait_for_next_check.wait();

#define TIMED_WAIT_ON_NEXT_CMD(x) \
whait_for_next_check.wait(x);

void BatchCommandSandbox::checkNextCommand() {
    bool queue_empty = false;
    bool canWork = schedule_work_flag;
    RunningVSSubmissioneResult current_check_value;
    
    
    SCSLDBG_ << "[checkNextCommand] checkNextCommand started wait run scheduler notify";
    whait_for_next_check.wait();
    
    
    if (!schedule_work_flag) {
        SCSLDBG_ << "[checkNextCommand] we need to exit befor start the loop";
        //condition_waith_scheduler_end.notify_one();
        return;
    }
    SCSLDBG_ << "[checkNextCommand] checkNextCommand can work";
    //manage the lock on next command mutex
    boost::unique_lock<boost::mutex> lock_next_command_queue(mutex_next_command_queue, boost::defer_lock_t());
    
    while (canWork) {
        //lock the command queue access
        lock_next_command_queue.lock();
        queue_empty = command_submitted_queue.empty();
        lock_next_command_queue.unlock();
        //check for emptiness
        if (!queue_empty) {
            if (current_executing_command) {
                PRIORITY_ELEMENT(CommandInfoAndImplementation) *command_to_delete = NULL;
                PRIORITY_ELEMENT(CommandInfoAndImplementation) *next_available_command = NULL;
                //compute the runnig state or fault
                boost::mutex::scoped_lock lockForCurrentCommandMutex(mutext_access_current_command);
                // cehck waht we need to do with current and submitted command
                lock_next_command_queue.lock();
                next_available_command = command_submitted_queue.top();
                DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] check installation for new command with pointer =" << std::hex << next_available_command << std::dec;)
                
                //check what we need to do according to runng and submission property
                current_check_value = CHECK_RUNNING_VS_SUBMISSION(current_executing_command->element->cmdImpl->runningProperty,
                                                                  next_available_command->element->cmdImpl->submissionRule);
                //manage the situation
                switch (current_check_value) {
                    case RSR_NO_CHANGE:
                        DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_NO_CHANGE";)
                        lockForCurrentCommandMutex.unlock();
                        lock_next_command_queue.unlock();
                        WAIT_ON_NEXT_CMD
                        continue; //we must recontorl the top element because it could be have changed
                        break;
                        
                    case RSR_TIMED_RETRY:
                        DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_TIMED_RETRY";)
                        lockForCurrentCommandMutex.unlock();
                        lock_next_command_queue.unlock();
                        TIMED_WAIT_ON_NEXT_CMD(next_available_command->element->cmdImpl->commandFeatures.featureSubmissionRetryDelay);
                        continue; //we must recontorl the top element because it could be have changed
                        break;
                        
                    case RSR_STACK_CURENT_COMMAND:
                        //the stack feature need that the
                        DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] RSR_STACK_CURENT_COMMAND";)
                        command_submitted_queue.pop();
                        lock_next_command_queue.unlock();
                        if(current_executing_command->element->cmdImpl->sticky != true) {
                            command_stack.push(current_executing_command);
                        }
                        DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] element in command_stack " << command_stack.size();)
                        //signal that current handler will has been paused
                        if (event_handler) event_handler->handleCommandEvent(current_executing_command->element->cmdImpl->command_alias,
                                                                             current_executing_command->element->cmdImpl->unique_id,
                                                                             BatchCommandEventType::EVT_PAUSED, NULL);
                        //install next command
                        installHandler(next_available_command);
                        
                        //unlock scheduler
                        thread_scheduler_pause_condition.unlock();
                        break;
                        
                    case RSR_KILL_KURRENT_COMMAND:
                    case RSR_CURRENT_CMD_HAS_ENDED:
                    case RSR_CURRENT_CMD_HAS_FAULTED: {
                        //execute the handler
                        command_to_delete = current_executing_command;
                        current_executing_command = NULL;
#pragma GCC diagnostic ignored "-Wswitch"
                        switch (current_check_value) {
                            case RSR_KILL_KURRENT_COMMAND:
                                if (event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                                          command_to_delete->element->cmdImpl->unique_id,
                                                                                                          BatchCommandEventType::EVT_KILLED,
                                                                                                          NULL);
                                break;
                                
                            case RSR_CURRENT_CMD_HAS_ENDED:
                                if (event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                                          command_to_delete->element->cmdImpl->unique_id,
                                                                                                          BatchCommandEventType::EVT_COMPLETED,
                                                                                                          static_cast<void*>(command_to_delete->element->cmdInfo));
                                break;
                                
                            case RSR_CURRENT_CMD_HAS_FAULTED:
                                if (event_handler && command_to_delete) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                                          command_to_delete->element->cmdImpl->unique_id,
                                                                                                          BatchCommandEventType::EVT_FAULT,
                                                                                                          static_cast<FaultDescription*> (&command_to_delete->element->cmdImpl->fault_description),
                                                                                                          sizeof (FaultDescription));
                                break;
                        }
#pragma GCC diagnostic pop
                        
                        command_submitted_queue.pop();
                        lock_next_command_queue.unlock();
                        DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] element in command_submitted_queue " << command_submitted_queue.size();)
                        
                        removeHandler(command_to_delete);
                        
                        if (installHandler(next_available_command)) {
                            DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] installed command with pointer " << std::hex << next_available_command << std::dec;)
                        }
                        thread_scheduler_pause_condition.unlock();
                        break;
                    }
                }
                
                lockForCurrentCommandMutex.unlock();
                //delete
                if (command_to_delete && !command_to_delete->element->cmdImpl->sticky) {
                    DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Delete command with pointer " << std::hex << command_to_delete << std::dec;)
                    DELETE_OBJ_POINTER(command_to_delete);
                }
            } else {
                lock_next_command_queue.lock();
                PRIORITY_ELEMENT(CommandInfoAndImplementation) *nextAvailableCommand = command_submitted_queue.top();
                installHandler(nextAvailableCommand);
                command_submitted_queue.pop();
                //lock the command queue access
                lock_next_command_queue.unlock();
                thread_scheduler_pause_condition.unlock();
            }
        } else {
            if(current_executing_command) {
                if (current_executing_command->element->cmdImpl->runningProperty >= RunningPropertyType::RP_End) {
                    boost::mutex::scoped_lock lockForCurrentCommandMutex(mutext_access_current_command);
                    if (!command_stack.empty()) {
                        //keep track of running property that needs to be deleted
                        PRIORITY_ELEMENT(CommandInfoAndImplementation) *command_to_delete = current_executing_command;
                        current_executing_command = NULL;
                        switch (command_to_delete->element->cmdImpl->runningProperty) {
                            case RunningPropertyType::RP_End:
                                if (event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                     command_to_delete->element->cmdImpl->unique_id,
                                                                                     BatchCommandEventType::EVT_COMPLETED,
                                                                                     static_cast<void*>(command_to_delete->element->cmdInfo));
                                break;
                            case RunningPropertyType::RP_Fault:
                                if (event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                     command_to_delete->element->cmdImpl->unique_id,
                                                                                     BatchCommandEventType::EVT_FAULT,
                                                                                     static_cast<FaultDescription*> (&command_to_delete->element->cmdImpl->fault_description),
                                                                                     sizeof (FaultDescription));
                                
                                break;
                        }
                        //manage paused command
                        PRIORITY_ELEMENT(CommandInfoAndImplementation) * nextAvailableCommand = command_stack.top();
                        
                        removeHandler(command_to_delete);
                        installHandler(nextAvailableCommand);
                        command_stack.pop();
                        thread_scheduler_pause_condition.unlock();
                        if(command_to_delete->element->cmdImpl->sticky == false){
                            
                            DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Delete command with pointer " << std::hex << command_to_delete << std::dec;)
                            DELETE_OBJ_POINTER(command_to_delete);
                        }
                    } else {
                        PRIORITY_ELEMENT(CommandInfoAndImplementation) *command_to_delete = current_executing_command;
                        current_executing_command = NULL;
                        switch (command_to_delete->element->cmdImpl->runningProperty) {
                            case RunningPropertyType::RP_End:
                                if (event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                     command_to_delete->element->cmdImpl->unique_id,
                                                                                     BatchCommandEventType::EVT_COMPLETED,
                                                                                     static_cast<void*>(command_to_delete->element->cmdInfo));
                                break;
                            case RunningPropertyType::RP_Fault:
                                if (event_handler) event_handler->handleCommandEvent(command_to_delete->element->cmdImpl->command_alias,
                                                                                     command_to_delete->element->cmdImpl->unique_id,
                                                                                     BatchCommandEventType::EVT_FAULT,
                                                                                     static_cast<FaultDescription*> (&command_to_delete->element->cmdImpl->fault_description),
                                                                                     sizeof (FaultDescription));
                                
                                break;
                        }
                        if(command_to_delete->element->cmdImpl->sticky==false){
                            
                            removeHandler(command_to_delete);
                            
                            installHandler(NULL);
                            
                            DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Delete command with pointer " << std::hex << command_to_delete << std::dec;)
                            DELETE_OBJ_POINTER(command_to_delete);
                        }
                    }
                }
            }
            //at this point we need to check for the stiky comamnd submission
            if (current_executing_command == NULL &&
                default_sticky_command.get() &&
                default_sticky_command->element->cmdImpl->sticky) {
                //we have no commando so we need to apply the default command
                installHandler(default_sticky_command.get());
                thread_scheduler_pause_condition.unlock();
		//                DEBUG_CODE(SCSLDBG_ << "[checkNextCommand] Use sticky default command " << std::hex << default_sticky_command.get() << std::dec;)
            }
            WAIT_ON_NEXT_CMD
        }
        
        if (!schedule_work_flag) {
            canWork = false;
        }
    }
    
    SCSLAPP_ << "[checkNextCommand] Check next command thread ended";
    //notify the end of the thread
    //condition_waith_scheduler_end.notify_one();
    
}

void BatchCommandSandbox::runCommand() {
    bool canWork = schedule_work_flag;
    uint64_t next_predicted_run = 0;
    int64_t next_prediction_error = 0;
    BatchCommand *curr_executing_impl = NULL;
    //check if the current command has ended or need to be substitute
    boost::mutex::scoped_lock lockForCurrentCommand(mutext_access_current_command);
    do {
        if (current_executing_command) {
            curr_executing_impl = current_executing_command->element->cmdImpl;
            
            // count the time we have started a run step
            curr_executing_impl->timing_stats.command_step_counter++;
            
            stat.last_cmd_step_start_usec = TimingUtil::getTimeStampInMicrosends();
            if(next_predicted_run) {
                //are onthe second
                if((next_prediction_error = stat.last_cmd_step_start_usec - next_predicted_run) < 0){
                    next_prediction_error = 0;
                }
            }
            
            
            if (event_handler) {
                //signal the step of the run
                event_handler->handleSandboxEvent(identification,
                                                  BatchSandboxEventType::EVT_RUN_START,
                                                  &stat.last_cmd_step_start_usec, sizeof (uint64_t));
            }
            // call the acquire phase
            acquire_handler_functor();
            
            //call the correlation and commit phase();
            correlation_handler_functor();
            
            //compute step duration
            stat.last_cmd_step_duration_usec = TimingUtil::getTimeStampInMicrosends() - stat.last_cmd_step_start_usec;
            
            //fire post command step
            curr_executing_impl->commandPost();
            
            lockForCurrentCommand.unlock();
            
            if (event_handler) {
                //signal the step of the run
                event_handler->handleSandboxEvent(identification,
                                                  BatchSandboxEventType::EVT_RUN_END);
            }
            
            //check runnin property
            if (!schedule_work_flag && curr_executing_impl->runningProperty) {
                canWork = false;
            } else {
                switch (curr_executing_impl->runningProperty) {
                    case RunningPropertyType::RP_Exsc:
                    case RunningPropertyType::RP_Normal:
                    {
                        int64_t timeToWaith = (curr_executing_impl->commandFeatures.featureSchedulerStepsDelay) - (stat.last_cmd_step_duration_usec + next_prediction_error);
                        //adjust a little bit the jitter
                        if (timeToWaith > 0) {
                            next_predicted_run = TimingUtil::getTimeStampInMicrosends() + timeToWaith;
                            thread_scheduler_pause_condition.waitUSec(timeToWaith);
                        } else {
                            next_predicted_run = 0;
                            next_prediction_error = 0;
                        }
                        break;
                    }
                        
                    case RunningPropertyType::RP_Fault:
                    case RunningPropertyType::RP_End:
                        //put this at null because someone can change it
                        curr_executing_impl = NULL;
                        whait_for_next_check.unlock();
                        thread_scheduler_pause_condition.wait();
                        break;
                }
                
            }
            lockForCurrentCommand.lock();
        } else {
            //unloc
            if (!schedule_work_flag) {
                canWork = false;
            } else {
                //no more command to scehdule
                
                //reset the statistic befor sleep
                std::memset(&stat, 0, sizeof (SandboxStat));
                lockForCurrentCommand.unlock();
                whait_for_next_check.unlock();
                thread_scheduler_pause_condition.wait();
                lockForCurrentCommand.lock();
            }
        }
    } while (canWork);
    SCSLDBG_ << "Scheduler thread has finished";
}

//!install the command

bool BatchCommandSandbox::installHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation) * cmd_to_install) {
    //set current command
    if (cmd_to_install && cmd_to_install->element->cmdImpl) {
        chaos_data::CDataWrapper *tmp_info = cmd_to_install->element->cmdInfo;
        BatchCommand *tmp_impl = cmd_to_install->element->cmdImpl;
        
        uint8_t handlerMask = tmp_impl->implementedHandler();
        //install the pointer of th ecommand into the respective handler functor
        
        //set the shared stat befor cal set handler
        tmp_impl->shared_stat = &stat;
        
        //check set handler
        if (!tmp_impl->already_setupped && (handlerMask & HandlerType::HT_Set)) {
            try {
                tmp_impl->commandPre();
                tmp_impl->setHandler(tmp_info);
                tmp_impl->already_setupped = true;
            } catch (chaos::CException& ex) {
                SET_NAMED_FAULT(SCSLERR_, tmp_impl, ex.errorCode, ex.errorMessage, ex.errorDomain)
            } catch (std::exception& ex) {
                SET_NAMED_FAULT(SCSLERR_, tmp_impl, -1, ex.what(), "Acquisition Handler");
            } catch (...) {
                SET_NAMED_FAULT(SCSLERR_, tmp_impl, -2, "Unmanaged exception", "Acquisition Handler");
            }
        }
        
        //acquire handler
        if (handlerMask & HandlerType::HT_Acquisition) {acquire_handler_functor.cmd_instance = tmp_impl;}
        
        //correlation commit
        if (handlerMask & HandlerType::HT_Correlation) {correlation_handler_functor.cmd_instance = tmp_impl;}
        
        current_executing_command = cmd_to_install;
        
        //fire the running event
        if (event_handler) {
            event_handler->handleCommandEvent(tmp_impl->command_alias,
                                              tmp_impl->unique_id,
                                              BatchCommandEventType::EVT_RUNNING, NULL);
            //signal the step of the run
            event_handler->handleSandboxEvent(identification,
                                              BatchSandboxEventType::EVT_UPDATE_RUN_DELAY,
                                              &current_executing_command->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay, sizeof (uint64_t));
        }
    } else {
        current_executing_command = NULL;
        acquire_handler_functor.cmd_instance = NULL;
        correlation_handler_functor.cmd_instance = NULL;
    }
    return true;
}

void BatchCommandSandbox::removeHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation) * cmd_to_install) {
    if (!cmd_to_install ) return;
    BatchCommand *tmp_impl = cmd_to_install->element->cmdImpl;
    uint8_t handlerMask = tmp_impl->implementedHandler();
    if (handlerMask <= 1) {
        //there is only the set handler so we finish here.
        return;
    }
    
    //acquire handler
    if ((handlerMask & HandlerType::HT_Acquisition) &&
        acquire_handler_functor.cmd_instance == cmd_to_install->element->cmdImpl) acquire_handler_functor.cmd_instance = NULL;
    
    //correlation commit
    if ((handlerMask & HandlerType::HT_Correlation) &&
        correlation_handler_functor.cmd_instance == cmd_to_install->element->cmdImpl) correlation_handler_functor.cmd_instance = NULL;
}

void BatchCommandSandbox::killCurrentCommand() {
    //lock the scheduler
    boost::mutex::scoped_lock lockForCurrentCommand(mutext_access_current_command);
    
    // terminate the current command
    current_executing_command->element->cmdImpl->setRunningProperty(RunningPropertyType::RP_End);
}

bool BatchCommandSandbox::enqueueCommand(chaos_data::CDataWrapper *command_to_info, BatchCommand *command_impl, uint32_t priority) {
    CHAOS_ASSERT(command_impl)
    
    if (StartableService::serviceState == CUStateKey::DEINIT) return false;
    
    //
    {
        DEBUG_CODE(SCSLDBG_ << "Try to lock for command enqueue for:\"" << command_impl->command_alias.c_str()) << "\"";
        boost::unique_lock<boost::mutex> lock_next_command_queue(mutex_next_command_queue);
        command_submitted_queue.push(new PriorityQueuedElement<CommandInfoAndImplementation>(new CommandInfoAndImplementation(command_to_info, command_impl),
                                                                                             command_impl->unique_id,
                                                                                             priority,
                                                                                             true));
        SCSLDBG_ << "New command enqueued:\"" << command_impl->command_alias.c_str() << "\" \"" << ((command_to_info) ? command_to_info->getJSONString() : "") << "\"";
        SCSLDBG_ << "Queue size:" << command_submitted_queue.size();
        
    }
    //fire the waiting command
    if (event_handler) event_handler->handleCommandEvent(command_impl->command_alias,
                                                         command_impl->unique_id,
                                                         BatchCommandEventType::EVT_QUEUED,
                                                         (void*) command_impl->command_alias.c_str(),
                                                         (uint32_t) command_impl->command_alias.size());
    whait_for_next_check.unlock();
    return true;
}

//! Command features modification rpc action

void BatchCommandSandbox::setCommandFeatures(features::Features& features) throw (CException) {
    uint64_t thread_step_delay = 0;
    //lock the scheduler
    boost::mutex::scoped_lock lockForCurrentCommand(mutext_access_current_command);
    
    //recheck current command
    if (!current_executing_command) return;
    
    current_executing_command->element->cmdImpl->commandFeatures.featuresFlag |= features.featuresFlag;
    current_executing_command->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay = (thread_step_delay = features.featureSchedulerStepsDelay);
    
    thread_scheduler_pause_condition.unlock();
    
    if (event_handler) {
        //signal the step of the run
        event_handler->handleSandboxEvent(identification,
                                          BatchSandboxEventType::EVT_UPDATE_RUN_DELAY,
                                          &thread_step_delay, sizeof (uint64_t));
    }
}
