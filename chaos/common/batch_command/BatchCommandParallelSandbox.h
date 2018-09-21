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

#ifndef __CHAOSFramework__A9CAC5A_8A69_4366_8E21_96FC95623C48_BatchCommandParallelSandbox_h
#define __CHAOSFramework__A9CAC5A_8A69_4366_8E21_96FC95623C48_BatchCommandParallelSandbox_h

#include <chaos/common/batch_command/AbstractSandbox.h>

#include <chaos/common/utility/LockableObject.h>

#include <chaos/common/thread/WaitSemaphore.h>

#include <list>

#include <boost/thread.hpp>

namespace chaos{
    namespace common {
        namespace batch_command {
            
            struct RunningCommandStat {
                SandboxStat stat;
                //!is the timestamp after with command can run another step
                uint64_t next_exec_timestamp;
                
                //!contains the command info
                CommandInfoAndImplementation command_info;
                
                
                RunningCommandStat(chaos::common::data::CDataWrapper *_cmd_data,
                                   BatchCommand *_cmd_impl);
                ~RunningCommandStat();
                
                void computeNextRun(uint64_t current_ts);
                
                bool canRun(uint64_t current_ts);
            };
            
            
            typedef ChaosSharedPtr<RunningCommandStat> RunningCommandStatShrdPtr;
            
            CHAOS_DEFINE_LOCKABLE_OBJECT(std::queue< RunningCommandStatShrdPtr >, LockableSubmissionQueue);
            
            class BatchCommandParallelSandbox:
            public AbstractSandbox {
                BatchCommandStat cmd_stat;
            protected:
                
                bool thread_run;
                boost::thread_group thread_group;
                
                //waith semaphore when no element are in execution list
                WaitSemaphore sem_waith_for_job;
                
                //contains command that are running in a cooperative scheduling way
                std::list< RunningCommandStatShrdPtr > execution_command_list;
                
                //contains command that are submited to be executed
                LockableSubmissionQueue submition_command_queue;
                
                //schedule command step
                void runCommand();
                
                bool processCommand(bool set_handler_call,
                                    RunningCommandStat& command_stat,
                                    uint64_t& current_timestamp);
            public:
                BatchCommandParallelSandbox();
                
                ~BatchCommandParallelSandbox();
                
                void init(void *init_data);
                
                void start();
                
                void stop();
                
                void deinit();
                
                void killCurrentCommand();
                
                void clearCommandQueue();
                
                void setCurrentCommandScheduerStepDelay(uint64_t scheduler_step_delay);
                
                void lockCurrentCommandFeature(bool lock);
                
                void setCurrentCommandFeatures(features::Features& features);
                
                void setDefaultStickyCommand(BatchCommand *sticky_command);
                
                bool enqueueCommand(chaos::common::data::CDataWrapper *command_data,
                                    BatchCommand *command_impl,
                                    uint32_t priority);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__A9CAC5A_8A69_4366_8E21_96FC95623C48_BatchCommandParallelSandbox_h */
