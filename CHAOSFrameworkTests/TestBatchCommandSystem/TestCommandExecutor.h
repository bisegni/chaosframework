/*
 *	TestCommandExecutor.h
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

#ifndef __CHAOSFrameworkTests__EFDDB0A_68A8_49F1_9F35_99A49A968421_TestCommandExecutor_h
#define __CHAOSFrameworkTests__EFDDB0A_68A8_49F1_9F35_99A49A968421_TestCommandExecutor_h

#include "TestBatchCommand.h"
#include "TestBatchDefault.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>

namespace chaos{
    namespace common {
        namespace batch_command {
            namespace test {
                
                struct TestElement {
                    TestBatchCommand *command_instance;
                    common::batch_command::BatchCommandEventType::BatchCommandEventType last_event;
                    bool has_completed;
                    bool has_faulted;
                    
                    TestElement():
                    command_instance(NULL),
                    has_completed(false),
                    has_faulted(0){}
                    
                    TestElement(TestBatchCommand *_command_instance):
                    command_instance(_command_instance),
                    has_completed(false),
                    has_faulted(0){}
                };
                
                CHAOS_DEFINE_MAP_FOR_TYPE(uint64_t, TestElement, IDCommandMap);
                
                /*!
                 this class sis the implementation of the batch executor for
                 the metadataserver batch job
                 */
                class TestCommandExecutor:
                public BatchCommandExecutor {
                    chaos::common::utility::LockableObject<IDCommandMap> map_id_command;
                    
                    uint64_t last_end_time;
                    uint64_t completed_count;
                    uint64_t fault_count;
                    uint64_t killed_count;
                    uint64_t paused_count;
                protected:
                    //overload to permit the customization of newly created command instance
                    BatchCommand *  instanceCommandInfo(const std::string& command_alias,
                                                        uint32_t submission_rule = common::batch_command::SubmissionRuleType::SUBMIT_NORMAL,
                                                        uint32_t submission_retry_delay = 1000,
                                                        uint64_t scheduler_step_delay = 1000000);
                    
                    //overlodaed command event handler
                    void handleCommandEvent(const std::string& command_alias,
                                            uint64_t command_seq,
                                            BatchCommandEventType::BatchCommandEventType type,
                                            void* type_value_ptr,
                                            uint32_t type_value_size);
                    
                    //! general sandbox event handler
                    void handleSandboxEvent(const std::string& sandbox_id,
                                            BatchSandboxEventType::BatchSandboxEventType type,
                                            void* type_value_ptr,
                                            uint32_t type_value_size);
                    
                public:
                    bool has_default;
                    TestCommandExecutor();
                    ~TestCommandExecutor();
                    
                    uint64_t getRunningElement();
                    
                    void resetMap();
                    
                    void printStatistic();
                };
            }
        }
    }
}


#endif /* __CHAOSFrameworkTests__EFDDB0A_68A8_49F1_9F35_99A49A968421_TestCommandExecutor_h */
