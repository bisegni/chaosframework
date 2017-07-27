/*
 *	BatchCommand.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__MDSBatchCommand__
#define __CHAOSFramework__MDSBatchCommand__

#include "../persistence/data_access/DataAccess.h"

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos{
    namespace metadata_service {
        namespace batch {
            
            //forward declaration
            class MDSBatchExecutor;
            
            typedef enum RequestPhase {
                MESSAGE_PHASE_UNSENT,
                MESSAGE_PHASE_SENT,
                MESSAGE_PHASE_COMPLETED,
                MESSAGE_PHASE_TIMEOUT
            } MessagePhase;
            
            namespace mds_data_access = chaos::metadata_service::persistence::data_access;
            
            //! struct that permit to manage different request with phase for each one
            struct RequestInfo {
                unsigned int   retry;
                const std::string remote_address;
                const std::string remote_domain;
                std::string remote_action;
                RequestPhase   phase;
                ChaosUniquePtr<chaos::common::message::MessageRequestFuture> request_future;
                RequestInfo(const std::string& _remote_address,
                            const std::string& _remote_domain,
                            const std::string& _remote_action):
                retry(0),
                remote_address(_remote_address),
                remote_domain(_remote_domain),
                remote_action (_remote_action),
                phase(MESSAGE_PHASE_UNSENT){}
            };
            
            //! base class for all metadata service batch command
            /*!
             Represent the base class for all metadata server command. Every
             command is na instance of this class ans o many egual command can be launched
             with different parameter
             */
            class MDSBatchCommand:
            public chaos::common::batch_command::BatchCommand {
                friend class MDSBatchExecutor;
                
                //!message channel for communitcation with other node
                chaos::common::message::MessageChannel *message_channel;
                chaos::common::message::MultiAddressMessageChannel *multiaddress_message_channel;
                
                //executor used for submit command witin command
                MDSBatchExecutor *executor_instance;
                
                //dataaccess abstract driver
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver *abstract_persistance_driver;
            protected:
                //! default constructor
                MDSBatchCommand();
                
                //! default destructor
                ~MDSBatchCommand();
                
                //! return a raw message channel
                chaos::common::message::MessageChannel *getMessageChannel();
                
                //! return a raw multinode message channel
                chaos::common::message::MultiAddressMessageChannel *getMultiAddressMessageChannel();
                
                // return the implemented handler
                uint8_t implementedHandler();
                
                // inherited method
                void setHandler(chaos_data::CDataWrapper *data);
                
                // inherited method
                void acquireHandler();
                
                // inherited method
                void ccHandler();
                
                // inherited method
                bool timeoutHandler();
                
                //! create a request to a remote rpc action
                ChaosUniquePtr<RequestInfo> createRequest(const std::string& remote_address,
                                                          const std::string& remote_domain,
                                                          const std::string& remote_action) throw (chaos::CException);
                
                //! send a request to a remote rpc action
                /*!
                 \param message the message to send(the ownership of the memory belong to the caller)
                 */
                void sendRequest(RequestInfo& request_info,
                                 chaos::common::data::CDataWrapper *message) throw (chaos::CException);
                
                //! send a message toa remote rpc action
                void sendMessage(RequestInfo& request_info,
                                 chaos::common::data::CDataWrapper *message) throw (chaos::CException);
                
                ChaosUniquePtr<RequestInfo> sendRequest(const std::string& node_uid,
                                                        const std::string& rpc_action,
                                                        chaos::common::data::CDataWrapper *message) throw (chaos::CException);
                
                ChaosUniquePtr<RequestInfo> sendMessage(const std::string& node_uid,
                                                        const std::string& rpc_action,
                                                        chaos::common::data::CDataWrapper *message) throw (chaos::CException);
                
                void manageRequestPhase(RequestInfo& request_info) throw (chaos::CException);
                
                template<typename T>
                T* getDataAccess() {
                    CHAOS_ASSERT(abstract_persistance_driver)
                    return abstract_persistance_driver->getDataAccess<T>();
                }
                
                
                uint32_t getNextSandboxToUse();
                
                uint64_t submitCommand(const std::string& batch_command_alias,
                                       chaos_data::CDataWrapper *command_data,
                                       uint32_t sandbox_id,
                                       uint32_t priority = 50);
                
                uint64_t submitCommand(const std::string& batch_command_alias,
                                       chaos_data::CDataWrapper *command_data);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__MDSBatchCommand__) */
