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

#include "TestBatchCommand.h"
#include "TestBatchDefault.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>

using namespace chaos::common::batch_command;

CHAOS_DEFINE_LOCKABLE_OBJECT(BatchCommandStat, LBatchCommandStat);
/*!
 this class sis the implementation of the batch executor for
 the metadataserver batch job
 */
class TestCommandExecutor:
public BatchCommandExecutor {
public:
    uint64_t queued;
    uint64_t stacked;
    uint64_t last_end_time;
    uint64_t completed_count;
    uint64_t fault_count;
    uint64_t killed_count;
    uint64_t paused_count;
protected:
    //overload to permit the customization of newly created command instance
    BatchCommand *  instanceCommandInfo(const std::string& command_alias,
                                        uint32_t submission_rule = SubmissionRuleType::SUBMIT_NORMAL,
                                        uint32_t submission_retry_delay = 1000,
                                        uint64_t scheduler_step_delay = 1000000);
    
    //overlodaed command event handler
    void handleCommandEvent(const std::string& command_alias,
                            uint64_t command_seq,
                            BatchCommandEventType::BatchCommandEventType type,
                            chaos::common::batch_command::CommandInfoAndImplementation *command_info,
                            const BatchCommandStat& commands_stats);
    
    //! general sandbox event handler
    void handleSandboxEvent(const std::string& sandbox_id,
                            BatchSandboxEventType::BatchSandboxEventType type,
                            void* type_value_ptr,
                            uint32_t type_value_size);
    
public:
    bool has_default;
    TestCommandExecutor();
    ~TestCommandExecutor();
    
    void printStatistic();
    void resetStat();
    uint64_t getQueued();
    uint64_t getStacked();
};
