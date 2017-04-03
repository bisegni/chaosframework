/*
 *	TestCommandExecutor.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/08/2016 INFN, National Institute of Nuclear Physics
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

#include "TestCommandExecutor.h"

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::batch_command::test;

TestCommandExecutor::TestCommandExecutor():
BatchCommandExecutor("exec_id"),
last_end_time(0),
completed_count(0),
fault_count(0),
killed_count(0),
paused_count(0),
has_default(false){}

TestCommandExecutor::~TestCommandExecutor() {
    
}

BatchCommand *  TestCommandExecutor::instanceCommandInfo(const std::string& command_alias,
                                                         uint32_t submission_rule,
                                                         uint32_t submission_retry_delay,
                                                         uint64_t scheduler_step_delay) {
    //install command into the batch command executor root class
    TestBatchCommand *result = (TestBatchCommand*) BatchCommandExecutor::instanceCommandInfo(command_alias,
                                                                                             submission_rule,
                                                                                             submission_retry_delay,
                                                                                             scheduler_step_delay);
    
    //customize the newly create batch command
    if(result) {
        LockableObjectWriteLock_t wr;
        map_id_command.getWriteLock(wr);
        map_id_command().insert(IDCommandMapPair(result->getUID(), TestElement(result)));
    }
    return result;
}

void TestCommandExecutor::handleCommandEvent(const std::string& command_alias,
                                             uint64_t command_seq,
                                             BatchCommandEventType::BatchCommandEventType type,
                                             chaos::common::data::CDataWrapper *command_info,
                                             const BatchCommandStat& commands_stats) {
    LockableObjectWriteLock_t wl;
    map_id_command.getWriteLock(wl);
    local_stat = commands_stats;
    if(map_id_command().count(command_seq) == 0) return;
    TestElement& element = map_id_command()[command_seq];
    element.last_event = type;
    switch(type){
        case common::batch_command::BatchCommandEventType::EVT_QUEUED:
            break;
        case common::batch_command::BatchCommandEventType::EVT_WAITING:
            break;
        case common::batch_command::BatchCommandEventType::EVT_RUNNING:
            break;
        case common::batch_command::BatchCommandEventType::EVT_PAUSED:
            paused_count++;
            break;
        case common::batch_command::BatchCommandEventType::EVT_COMPLETED: {
            element.has_completed = true;
            completed_count++;
            uint64_t end_to_set_time = 0;
            if(last_end_time){
                end_to_set_time = element.command_instance->set_ts - last_end_time;
            }
            last_end_time = element.command_instance->end_ts;
            uint64_t set_ts_total = element.command_instance->set_ts-element.command_instance->create_ts;
            uint64_t end_ts_total = element.command_instance->end_ts-element.command_instance->set_ts;
            //std::cout << CHAOS_FORMAT("[Completed]Command with id %1% completed with st:%2% ed:%3% count:%4% ETS:%5%",%command_seq
            //                          %(set_ts_total)
            //                          %(end_ts_total)
            //                          %element.command_instance->cicle_count
            //                          %end_to_set_time) << std::endl;
            map_id_command().erase(command_seq);
            break;
        }
        case common::batch_command::BatchCommandEventType::EVT_FAULT:{
            map_id_command().erase(command_seq);
            fault_count++;
            break;
        }
        case common::batch_command::BatchCommandEventType::EVT_KILLED:
            map_id_command().erase(command_seq);
            killed_count++;
            break;
    }
}

void TestCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                             common::batch_command::BatchSandboxEventType::BatchSandboxEventType type,
                                             void* type_value_ptr,
                                             uint32_t type_value_size) {
}

uint64_t TestCommandExecutor::getRunningElement() {
    LockableObjectWriteLock_t wr;
    map_id_command.getWriteLock(wr);
    return local_stat.queued_commands+local_stat.stacked_commands;
}

void TestCommandExecutor::resetMap() {
    LockableObjectWriteLock_t wr;
    map_id_command.getWriteLock(wr);
    map_id_command().clear();
}

void TestCommandExecutor::printStatistic() {
    std::cout << CHAOS_FORMAT("Test terminated with\nTerminated:%1% Killed:%2% Faulted:%3% Number of Pause:%4% total:%5%", %completed_count%killed_count%fault_count%paused_count%(completed_count+killed_count+fault_count)) << std::endl;
}
