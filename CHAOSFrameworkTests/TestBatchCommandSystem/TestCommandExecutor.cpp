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
queued(0),
stacked(0),
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
    return result;
}

void TestCommandExecutor::handleCommandEvent(const std::string& command_alias,
                                             uint64_t command_seq,
                                             BatchCommandEventType::BatchCommandEventType type,
                                             chaos::common::data::CDataWrapper *command_info,
                                             const BatchCommandStat& commands_stats) {
    queued = commands_stats.queued_commands;
    stacked = commands_stats.stacked_commands;
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

            completed_count++;
            break;
        }
        case common::batch_command::BatchCommandEventType::EVT_FAULT: {
            fault_count++;
            break;
        }
            
        case common::batch_command::BatchCommandEventType::EVT_KILLED: {
            killed_count++;
            break;
        }
    }
}

void TestCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                             common::batch_command::BatchSandboxEventType::BatchSandboxEventType type,
                                             void* type_value_ptr,
                                             uint32_t type_value_size) {
}

void TestCommandExecutor::resetStat() {
    last_end_time = 0;
    completed_count = 0;
    fault_count = 0;
    killed_count = 0;
    paused_count = 0;
    queued = 0;
    stacked = 0;
}

void TestCommandExecutor::printStatistic() {
    std::cout << CHAOS_FORMAT("Test terminated with\nTerminated:%1% Killed:%2% Faulted:%3% Number of Pause:%4% total:%5%", %completed_count%killed_count%fault_count%paused_count%(completed_count+killed_count+fault_count)) << std::endl;
}

uint64_t TestCommandExecutor::getQueued() {
    return queued;
}

uint64_t TestCommandExecutor::getStacked() {
    return stacked;
}
