/*
 *	MDSBatchExecutor.cpp
 *	!CHOAS
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
#include "MDSBatchExecutor.h"
#include "unit_server/unit_server_batch.h"

using namespace chaos::metadata_service::batch;
#define BCE_INFO INFO_LOG(BatchCommandExecutor)
#define BCE_DBG  INFO_DBG(BatchCommandExecutor)
#define BCE_ERR  INFO_ERR(BatchCommandExecutor)

#define MDS_BATCH_COMMAND_INSTANCER(BatchCommandClass) new chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, common::batch_command::BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, MDSBatchCommand>())

MDSBatchExecutor::MDSBatchExecutor(const std::string& executor_id,
                                   chaos::common::network::NetworkBroker *_network_broker):
BatchCommandExecutor(executor_id),
network_broker(_network_broker){
    //register unit server command
    installCommand(unit_server::UnitServerAckCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(unit_server::UnitServerAckCommand));
}

MDSBatchExecutor::~MDSBatchExecutor() {
    
}

//! Install a command associated with a type
void MDSBatchExecutor::installCommand(std::string alias,
                                      chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, common::batch_command::BatchCommand> *instancer) {
    //call superclass method
    BatchCommandExecutor::installCommand(alias, instancer);
}

//allocate a new command
chaos::common::batch_command::BatchCommand *MDSBatchExecutor::instanceCommandInfo(const std::string& command_alias) {
    //install command into the batch command executor root class
    MDSBatchCommand *result = (MDSBatchCommand*) BatchCommandExecutor::instanceCommandInfo(command_alias);
    
    //customize the newly create batch command
    if(result) {
        //allcoate new message channel
        result->network_broker = network_broker;
    }
    return result;
}

//overlodaed command event handler
void MDSBatchExecutor::handleCommandEvent(uint64_t command_seq,
                                          common::batch_command::BatchCommandEventType::BatchCommandEventType type,
                                          void* type_value_ptr,
                                          uint32_t type_value_size) {
    
}

//! general sandbox event handler
void MDSBatchExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                          common::batch_command::BatchSandboxEventType::BatchSandboxEventType type,
                                          void* type_value_ptr,
                                          uint32_t type_value_size) {
    
}
