/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/batch_command/BatchCommandParallelSandbox.h>

#include <chaos/common/utility/TimingUtil.h>

#include <chaos/common/global.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;

#pragma mark RunningCommandStat
RunningCommandStat::RunningCommandStat(CDataWrapper *_cmd_data,
                                       BatchCommand *_cmd_impl):
command_info(_cmd_data,
             _cmd_impl),
next_exec_timestamp(0){
    command_info.cmdImpl->shared_stat = &stat;
}

RunningCommandStat::~RunningCommandStat() {}

void RunningCommandStat::computeNextRun(uint64_t current_ts) {
    next_exec_timestamp = current_ts + (command_info.cmdImpl->commandFeatures.featureSchedulerStepsDelay);
}

bool RunningCommandStat::canRun(uint64_t current_ts) {
    return next_exec_timestamp <= current_ts;
}
#pragma mark BatchCommandParallelSandbox
BatchCommandParallelSandbox::BatchCommandParallelSandbox():
thread_run(false){}

BatchCommandParallelSandbox::~BatchCommandParallelSandbox() {}

void BatchCommandParallelSandbox::init(void *init_data) throw(CException) {
    
}

void BatchCommandParallelSandbox::start() throw(CException) {
    thread_run = true;
    thread_group.add_thread(new boost::thread(boost::bind(&BatchCommandParallelSandbox::runCommand, this)));
}

void BatchCommandParallelSandbox::stop() throw(CException) {
    thread_run = false;
    sem_waith_for_job.unlock();
    thread_group.join_all();
}

void BatchCommandParallelSandbox::deinit() throw(CException) {
    
}

void BatchCommandParallelSandbox::runCommand() {
    uint64_t current_ts = 0;
    while(thread_run) {
        
        //get step timestamp
        if (event_handler) {
            //signal the step of the run
            event_handler->handleSandboxEvent(identification,
                                              BatchSandboxEventType::EVT_RUN_START,
                                              &(current_ts=TimingUtil::getTimeStampInMicroseconds()), sizeof (uint64_t));
        }
        //execute command step
        std::list< RunningCommandStatShrdPtr >::iterator it = execution_command_list.begin();
        std::list< RunningCommandStatShrdPtr >::iterator end = execution_command_list.end();
        while(it!=end){
            if((*it)->canRun(current_ts) == false) {
                it++;
                continue;
            }
            if(processCommand(false,
                              **it,
                              current_ts) == false) {
                it = execution_command_list.erase(it);
            } else {
                it++;
            }
        }
        
        //check if new command are in queue
        {
            LockableSubmissionQueueWriteLock rl = submition_command_queue.getWriteLockObject();
            while(submition_command_queue().empty() == false){
                cmd_stat.queued_commands--;
                if (event_handler){
                    event_handler->handleCommandEvent(submition_command_queue().front()->command_info.cmdImpl->command_alias,
                                                      submition_command_queue().front()->command_info.cmdImpl->unique_id,
                                                      BatchCommandEventType::EVT_RUNNING,
                                                      submition_command_queue().front()->command_info.cmdInfo,
                                                      cmd_stat);
                }
                if(processCommand(true,
                                  *submition_command_queue().front(),
                                  current_ts)) {
                    //command still runing so we punt into execution queue
                    execution_command_list.push_back(submition_command_queue().front());
                }
                //check if command has ended
                submition_command_queue().pop();
            }
        }
        
        if (event_handler) {
            //signal the step of the run
            event_handler->handleSandboxEvent(identification,
                                              BatchSandboxEventType::EVT_RUN_END);
        }
        
        if(execution_command_list.size() == 0) {
            //with for new job
            sem_waith_for_job.wait();
        }
    }
}

bool BatchCommandParallelSandbox::processCommand(bool set_handler_call,
                                                 RunningCommandStat& command_stat,
                                                 uint64_t& current_timestamp) {
    bool result = false;
    uint8_t handlerMask = command_stat.command_info.cmdImpl->implementedHandler();
    
    command_stat.stat.last_cmd_step_start_usec = TimingUtil::getTimeStampInMicroseconds();
    try{
        if(set_handler_call) {
            //run set handler
            command_stat.command_info.cmdImpl->commandPre();
            command_stat.command_info.cmdImpl->setHandler(command_stat.command_info.cmdInfo);
            command_stat.command_info.cmdImpl->already_setupped = true;
            
            //check if command nend to run on other handler
            result = (handlerMask & HandlerType::HT_Acquisition) || (handlerMask & HandlerType::HT_Correlation);
        } else {
            //run acquire and cc
            if(command_stat.command_info.cmdImpl->runningProperty < RunningPropertyType::RP_END) {
                // count the time we have started a run step
                command_stat.command_info.cmdImpl->timing_stats.command_step_counter++;
                
                
                // call the acquire phase
                command_stat.command_info.cmdImpl->acquireHandler();
                command_stat.command_info.cmdImpl->ccHandler();
                
                command_stat.command_info.cmdImpl->commandPost();
                
                switch (command_stat.command_info.cmdImpl->runningProperty) {
                    case RunningPropertyType::RP_EXSC:
                    case RunningPropertyType::RP_NORMAL:
                        result = true;
                        break;
                    default:
                        break;
                }
                
            }
        }
    } catch (chaos::CFatalException& ex) {
        SET_NAMED_FAULT(ERR_LOG(BatchCommandParallelSandbox), command_stat.command_info.cmdImpl, ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FATAL_FAULT);
    } catch (chaos::CException& ex) {
        SET_NAMED_FAULT(ERR_LOG(BatchCommandParallelSandbox), command_stat.command_info.cmdImpl, ex.errorCode, ex.errorMessage, ex.errorDomain,RunningPropertyType::RP_FAULT)
    } catch (std::exception& ex) {
        SET_NAMED_FAULT(ERR_LOG(BatchCommandParallelSandbox), command_stat.command_info.cmdImpl, -1, ex.what(), "Set Handler",RunningPropertyType::RP_FATAL_FAULT);
    } catch (...) {
        SET_NAMED_FAULT(ERR_LOG(BatchCommandParallelSandbox), command_stat.command_info.cmdImpl, -2, "Unmanaged exception", "Set Handler",RunningPropertyType::RP_FATAL_FAULT);
    }
    command_stat.stat.last_cmd_step_duration_usec =  (current_timestamp = TimingUtil::getTimeStampInMicroseconds()) - command_stat.stat.last_cmd_step_start_usec;
    switch(command_stat.command_info.cmdImpl->runningProperty) {
        case RunningPropertyType::RP_FAULT:
            if (event_handler){
                event_handler->handleCommandEvent(command_stat.command_info.cmdImpl->command_alias,
                                                  command_stat.command_info.cmdImpl->unique_id,
                                                  BatchCommandEventType::EVT_FAULT,
                                                  command_stat.command_info.cmdInfo,
                                                  cmd_stat);
            }
            result = false;
            command_stat.command_info.cmdImpl->endHandler();

            break;
        case RunningPropertyType::RP_FATAL_FAULT:
            if (event_handler){
                event_handler->handleCommandEvent(command_stat.command_info.cmdImpl->command_alias,
                                                  command_stat.command_info.cmdImpl->unique_id,
                                                  BatchCommandEventType::EVT_FATAL_FAULT,
                                                  command_stat.command_info.cmdInfo,
                                                  cmd_stat);
            }
            result = false;
            command_stat.command_info.cmdImpl->endHandler();

            break;
        case RunningPropertyType::RP_END:
            if (event_handler){
                event_handler->handleCommandEvent(command_stat.command_info.cmdImpl->command_alias,
                                                  command_stat.command_info.cmdImpl->unique_id,
                                                  BatchCommandEventType::EVT_COMPLETED,
                                                  command_stat.command_info.cmdInfo,
                                                  cmd_stat);
            }
            result = false;
            command_stat.command_info.cmdImpl->endHandler();

            break;
        default:
            break;
    }
    command_stat.computeNextRun(current_timestamp);
    return result;
}

void BatchCommandParallelSandbox::killCurrentCommand() {
    
}

void BatchCommandParallelSandbox::setCurrentCommandScheduerStepDelay(uint64_t scheduler_step_delay) {
    
}

void BatchCommandParallelSandbox::lockCurrentCommandFeature(bool lock) {
    
}

void BatchCommandParallelSandbox::setCurrentCommandFeatures(features::Features& features) throw (CException) {
    
}

void BatchCommandParallelSandbox::setDefaultStickyCommand(BatchCommand *sticky_command) {
    
}

bool BatchCommandParallelSandbox::enqueueCommand(CDataWrapper *command_data,
                                                 BatchCommand *command_impl,
                                                 uint32_t priority) {
    //get lock on submission queue
    LockableSubmissionQueueWriteLock wl = submition_command_queue.getWriteLockObject();
    cmd_stat.queued_commands++;
    addCommandID(command_impl);
    submition_command_queue().push(RunningCommandStatShrdPtr(new RunningCommandStat(command_data, command_impl)));
    if (event_handler){
        event_handler->handleCommandEvent(command_impl->command_alias,
                                          command_impl->unique_id,
                                          BatchCommandEventType::EVT_QUEUED,
                                          command_data,
                                          cmd_stat);
    }
    sem_waith_for_job.unlock();
    return true;
}
