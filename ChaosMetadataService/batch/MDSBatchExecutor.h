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

#ifndef __CHAOSFramework__BatchExecutor__
#define __CHAOSFramework__BatchExecutor__

#include "MDSBatchCommand.h"
//#include "../ChaosMetadataService.h"

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>

#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

#include <boost/thread/mutex.hpp>

namespace chaos{
    namespace metadata_service {
        class ChaosMetadataService;
        namespace batch {
            
            /*!
             this class sis the implementation of the batch executor for
             the metadataserver batch job
             */
            class MDSBatchExecutor:
            public chaos::common::utility::Singleton<MDSBatchExecutor>,
            public chaos::common::batch_command::BatchCommandExecutor {
                friend class chaos::metadata_service::ChaosMetadataService;
                friend class chaos::common::utility::Singleton<MDSBatchExecutor>;

                chaos::common::message::MessageChannel *message_channel_for_job;
                chaos::common::message::MultiAddressMessageChannel *multiaddress_message_channel_for_job;
                
                //dataaccess abstract driver
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver *abstract_persistance_driver;
                
                //! used to track what sandbox has been used last time
                uint32_t        last_used_sb_idx;
                boost::mutex    mutex_sandbox_id;
            protected:
                //overload to permit the customization of newly created command instance
                chaos::common::batch_command::BatchCommand *  instanceCommandInfo(const std::string& command_alias,
                                                                                  uint32_t submission_rule = chaos::common::batch_command::SubmissionRuleType::SUBMIT_NORMAL,
                                                                                  uint32_t submission_retry_delay = 1000,
                                                                                  uint64_t scheduler_step_delay = 1000000);
                
                //overlodaed command event handler
                void handleCommandEvent(const std::string& command_alias,
                                        uint64_t command_seq,
                                        chaos::common::batch_command::BatchCommandEventType::BatchCommandEventType type,
                                        chaos::common::batch_command::CommandInfoAndImplementation *command_info,
                                        const ::chaos::common::batch_command::BatchCommandStat& commands_stat);
                
                //! general sandbox event handler
                void handleSandboxEvent(const std::string& sandbox_id,
                                        chaos::common::batch_command::BatchSandboxEventType::BatchSandboxEventType type,
                                        void* type_value_ptr,
                                        uint32_t type_value_size);
                
            public:
                MDSBatchExecutor();
                ~MDSBatchExecutor();
                
                // Initialize instance
                void init(void *init_data);
                
                // start instance
                void start();
                
                // stop instance
                void stop();
                
                // Deinitialize instance
                void deinit();
                
                //! return the number of the sandbox
                uint32_t getNextSandboxToUse();
                
                //! Install a command associated with a type
                void installCommand(const std::string& alias,
                                    chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, ::chaos::common::batch_command::BatchCommand> *instancer);
                //! submit a command using all sandbox in round robin way
                uint64_t submitCommand(const std::string& batch_command_alias,
                                       chaos::common::data::CDataWrapper * command_data);
                //! submit a command forcing a sandbox
                uint64_t submitCommand(const std::string& batch_command_alias,
                                       chaos::common::data::CDataWrapper * command_data,
                                       uint32_t sandbox_id,
                                       uint32_t priority = 50);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchExecutor__) */
