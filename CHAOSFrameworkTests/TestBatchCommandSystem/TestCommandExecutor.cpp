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

#define BATCH_COMMAND_INSTANCER(BatchCommandClass) new NestedObjectInstancer<TestBatchCommand, BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, TestBatchCommand>())

TestCommandExecutor::TestCommandExecutor():
BatchCommandExecutor("exec_id"),
create_ts_total(0),
set_ts_total(0),
end_ts_total(0),
cicle_count_total(0),
completed_count(0),
fault_count(0){
    installCommand("TestCommandSetOnly", BATCH_COMMAND_INSTANCER(TestCommandSetOnly));
    installCommand("TestCommandComplete", BATCH_COMMAND_INSTANCER(TestCommandComplete));
}

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
        map_id_command.insert(IDCommandMapPair(result->getUID(), result));
    }
    return result;
}

void TestCommandExecutor::handleCommandEvent(const std::string& command_alias,
                                             uint64_t command_seq,
                                             common::batch_command::BatchCommandEventType::BatchCommandEventType type,
                                             void* type_value_ptr,
                                             uint32_t type_value_size) {
    switch(type){
        case common::batch_command::BatchCommandEventType::EVT_QUEUED:
            break;
        case common::batch_command::BatchCommandEventType::EVT_WAITING:
            break;
        case common::batch_command::BatchCommandEventType::EVT_RUNNING:
            break;
        case common::batch_command::BatchCommandEventType::EVT_PAUSED:
            break;
        case common::batch_command::BatchCommandEventType::EVT_COMPLETED: {
            TestBatchCommand *instance = map_id_command[command_seq];
            completed_count++;
            create_ts_total += instance->create_ts;
            set_ts_total += instance->set_ts-instance->create_ts;
            end_ts_total += instance->end_ts-instance->create_ts;
            cicle_count_total += instance->cicle_count;
            std::cout << CHAOS_FORMAT("[Completed]Command with id %1% completed %5% with st:%2% ed:%3% count:%4%",%command_seq
                                      %(set_ts_total/command_seq)
                                      %(end_ts_total/command_seq)
                                      %(cicle_count_total/command_seq)
                                      %completed_count) << std::endl;
            break;
        }
        case common::batch_command::BatchCommandEventType::EVT_FAULT:{
            TestBatchCommand *instance = map_id_command[command_seq];
            fault_count++;
            create_ts_total += instance->create_ts;
            set_ts_total += instance->set_ts-instance->create_ts;
            end_ts_total += instance->end_ts-instance->create_ts;
            cicle_count_total += instance->cicle_count;
            std::cout << CHAOS_FORMAT("[Fault]Command with id %1% faulted %5% with st:%2% ed:%3% count:%4%",%command_seq
                                      %(set_ts_total/command_seq)
                                      %(end_ts_total/command_seq)
                                      %(cicle_count_total/command_seq)
                                      %completed_count) << std::endl;
            break;
        }
        case common::batch_command::BatchCommandEventType::EVT_KILLED:
            break;
    }
}

void TestCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                             common::batch_command::BatchSandboxEventType::BatchSandboxEventType type,
                                             void* type_value_ptr,
                                             uint32_t type_value_size) {
}

