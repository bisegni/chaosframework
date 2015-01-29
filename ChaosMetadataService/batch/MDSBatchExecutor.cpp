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

using namespace chaos::metadata_service::batch;

MDSBatchExecutor::MDSBatchExecutor(const std::string& executor_id):
BatchCommandExecutor(executor_id){
    
}

MDSBatchExecutor::~MDSBatchExecutor() {
    
}

//allocate a new command
chaos::common::batch_command::BatchCommand *MDSBatchExecutor::instanceCommandInfo(const std::string& commandAlias) {
    return NULL;
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

//! Install a command associated with a type
void MDSBatchExecutor::installCommand(std::string alias,
                                      chaos::common::utility::NestedObjectInstancer<MDSBatchCommand, common::batch_command::BatchCommand> *instancer) {
    
}