/*
 *	MDSBatchExecutor.cpp
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
#include "MDSBatchExecutor.h"
#include "unit_server/unit_server_batch.h"
#include "control_unit/control_unit_batch.h"
#include "node/node_batch.h"

using namespace chaos::metadata_service::batch;
#define BCE_INFO INFO_LOG(BatchCommandExecutor)
#define BCE_DBG  INFO_DBG(BatchCommandExecutor)
#define BCE_ERR  INFO_ERR(BatchCommandExecutor)

#define MDS_BATCH_COMMAND_INSTANCER(BatchCommandClass) new chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, common::batch_command::BatchCommand>(\
new chaos::common::utility::TypedObjectInstancer<BatchCommandClass, MDSBatchCommand>())

MDSBatchExecutor::MDSBatchExecutor(const std::string& executor_id,
                                   chaos::common::network::NetworkBroker *_network_broker):
BatchCommandExecutor(executor_id),
network_broker(_network_broker),
message_channel_for_job(NULL),
multiaddress_message_channel_for_job(NULL),
abstract_persistance_driver(NULL){
        //node server command
    installCommand(node::UpdatePropertyCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(node::UpdatePropertyCommand));
    installCommand(node::SubmitBatchCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(node::SubmitBatchCommand));
    
        //unit server command
    installCommand(unit_server::UnitServerAckCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(unit_server::UnitServerAckCommand));
    installCommand(unit_server::LoadUnloadControlUnit::command_alias, MDS_BATCH_COMMAND_INSTANCER(unit_server::LoadUnloadControlUnit));

        //control unit command
    installCommand(control_unit::ApplyChangeSet::command_alias, MDS_BATCH_COMMAND_INSTANCER(control_unit::ApplyChangeSet));
    installCommand(control_unit::RecoverError::command_alias, MDS_BATCH_COMMAND_INSTANCER(control_unit::RecoverError));
    installCommand(control_unit::RegistrationAckBatchCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(control_unit::RegistrationAckBatchCommand));
    installCommand(control_unit::IDSTControlUnitBatchCommand::command_alias, MDS_BATCH_COMMAND_INSTANCER(control_unit::IDSTControlUnitBatchCommand));
}

MDSBatchExecutor::~MDSBatchExecutor(){}

//! Install a command associated with a type
void MDSBatchExecutor::installCommand(const std::string& alias,
                                      chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, common::batch_command::BatchCommand> *instancer) {
    //call superclass method
    BatchCommandExecutor::installCommand(alias, instancer);
}

// Initialize instance
void MDSBatchExecutor::init(void *init_data) throw(chaos::CException) {
    //initilize superclass
    BatchCommandExecutor::init(init_data);
}

// start instance
void MDSBatchExecutor::start() throw(chaos::CException) {
    BatchCommandExecutor::start();
    //allocate channels
    message_channel_for_job = network_broker->getRawMessageChannel();
    if(!message_channel_for_job) throw chaos::CException(-1, "Error allcoating Message channel", __PRETTY_FUNCTION__);
    
    multiaddress_message_channel_for_job = network_broker->getRawMultiAddressMessageChannel();
    if(!multiaddress_message_channel_for_job) throw chaos::CException(-2, "Error allcoating Multiaddress Message channel", __PRETTY_FUNCTION__);
}

// stop instance
void MDSBatchExecutor::stop() throw(chaos::CException) {
    if(message_channel_for_job) {
        network_broker->disposeMessageChannel(message_channel_for_job);
        message_channel_for_job = NULL;
    }
    if(multiaddress_message_channel_for_job) {
        network_broker->disposeMessageChannel(multiaddress_message_channel_for_job);
        multiaddress_message_channel_for_job = NULL;
    }
    //deallcoate channels
    BatchCommandExecutor::stop();
}

// Deinitialize instance
void MDSBatchExecutor::deinit() throw(chaos::CException) {
    
    //deinitilize superclass
    BatchCommandExecutor::deinit();
}

//allocate a new command
chaos::common::batch_command::BatchCommand *MDSBatchExecutor::instanceCommandInfo(const std::string& command_alias,
                                                                                  CDataWrapper *command_info) {
    //install command into the batch command executor root class
    MDSBatchCommand *result = (MDSBatchCommand*) BatchCommandExecutor::instanceCommandInfo(command_alias,
                                                                                           command_info);
    
    //customize the newly create batch command
    if(result) {
        //allocoate new message channel
        result->message_channel = message_channel_for_job;
        result->multiaddress_message_channel = multiaddress_message_channel_for_job;
        result->abstract_persistance_driver = abstract_persistance_driver;
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
