/*
 *	BatchCommandParallelSandbox.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/12/2016 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <chaos/common/batch_command/BatchCommandParallelsandbox.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;

#pragma mark RunningCommandStat
RunningCommandStat::RunningCommandStat(CDataWrapper *_cmd_data,
                                       BatchCommand *_cmd_impl):
command_info(_cmd_data,
             _cmd_impl),
next_exec_timestamp(0){}

RunningCommandStat::~RunningCommandStat() {}

void RunningCommandStat::computeNextRun(uint64_t current_ts) {
    
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
    thread_group.join_all();
}

void BatchCommandParallelSandbox::deinit() throw(CException) {
    
}

void BatchCommandParallelSandbox::runCommand() {
    uint64_t current_ts = 0;
    while(thread_run) {
        
        //get step timestamp
        current_ts = TimingUtil::getTimeStamp();
        
        //execute command step
        for(std::list< RunningCommandStatShrdPtr >::iterator it = execution_command_list.begin(),
            end = execution_command_list.end();
            it != end;
            it++){
            processCommand(false,
                           **it,
                           current_ts);
        }
        
        //check if new command are in queue
        {
            LockableSubmissionQueueWriteLock rl = submition_command_queue.getWriteLockObject();
            while(submition_command_queue().empty() == false){
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
        
        if(execution_command_list.size() == 0) {
            //with for new job
            sem_waith_for_job.wait();
        }
    }
}

bool BatchCommandParallelSandbox::processCommand(bool set_handler_call,
                                                 RunningCommandStat& command_stat, uint64_t current_timestamp) {
    if(command_stat.next_exec_timestamp > current_timestamp) return false;
    
    //the command ca be executed
    try{
        if(set_handler_call) {
            //run set handler
        } else {
            //run acquire and cc
        }
    }catch(...) {
        
    }
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

bool BatchCommandParallelSandbox::enqueueCommand(CDataWrapper *command_to_info,
                                                 BatchCommand *command_impl,
                                                 uint32_t priority) {
    //get lock on submission queue
    LockableSubmissionQueueWriteLock wl = submition_command_queue.getWriteLockObject();
    addCommandID(command_impl);
    submition_command_queue().push(RunningCommandStatShrdPtr(new RunningCommandStat(command_to_info, command_impl)));
    sem_waith_for_job.unlock();
    return true;
}
