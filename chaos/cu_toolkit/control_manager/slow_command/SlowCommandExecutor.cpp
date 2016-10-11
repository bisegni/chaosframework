/*
 *	SlowCommandExecutor.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <vector>
#include <string>

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/control_manager/SCAbstractControlUnit.h>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommandExecutor.h>



using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::common::batch_command;


CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, CDataWrapperKeyList);

#define LOG_HEAD_SBE "[SlowCommandExecutor-" << dataset_attribute_db_ptr->getDeviceID() << "] "

#define SCELAPP_ LAPP_ << LOG_HEAD_SBE
#define SCELDBG_ LDBG_ << LOG_HEAD_SBE
#define SCELERR_ LERR_ << LOG_HEAD_SBE
using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;
using namespace chaos::common::metadata_logging;

SlowCommandExecutor::SlowCommandExecutor(const std::string& _executorID,
                                         DatasetDB *_dataset_attribute_db_ptr,
                                         SCAbstractControlUnit *_control_unit_instance):
BatchCommandExecutor(_executorID),
dataset_attribute_db_ptr(_dataset_attribute_db_ptr),
attribute_cache(new AttributeSharedCacheWrapper(getAttributeSharedCache())),
control_unit_instance(_control_unit_instance),
last_ru_id_cache(NULL),
last_error_code(NULL),
last_error_message(NULL),
last_error_domain(NULL),
driverAccessorsErogator(NULL),
error_logging_channel(NULL),
command_logging_channel(NULL){}

SlowCommandExecutor::~SlowCommandExecutor(){
    //delete the wrapper
    if(attribute_cache) {
        delete(attribute_cache);
    }
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    error_logging_channel = (ErrorLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("ErrorLoggingChannel");
    if(error_logging_channel == NULL) {throw CException(-1, "No metadata logging error channel found", __PRETTY_FUNCTION__);}
    
    command_logging_channel = (BatchCommandLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("BatchCommandLoggingChannel");
    if(command_logging_channel == NULL) {throw CException(-2, "No metadata logging batch command channel found", __PRETTY_FUNCTION__);}
    //initialize superclass
    BatchCommandExecutor::init(initData);
}

// Start the implementation
void SlowCommandExecutor::start() throw(chaos::CException) {
    //start superclass
    BatchCommandExecutor::start();
}


// Start the implementation
void SlowCommandExecutor::stop() throw(chaos::CException) {
    LDBG_ << "No implementation on stop";
    //initialize superclass
    BatchCommandExecutor::stop();
}

// Deinit instance
void SlowCommandExecutor::deinit() throw(chaos::CException) {
    //initialize superclass
    BatchCommandExecutor::deinit();
    
    if(error_logging_channel != NULL) {
        MetadataLoggingManager::getInstance()->releaseChannel(error_logging_channel);
        error_logging_channel = NULL;
    }
    
    if(command_logging_channel != NULL) {
        MetadataLoggingManager::getInstance()->releaseChannel(command_logging_channel);
        command_logging_channel = NULL;
    }
    LDBG_ << "No implementation on deinit";
    
    last_ru_id_cache = NULL;
    last_error_code = NULL;
    last_error_message = NULL;
    last_error_domain = NULL;
}

//! Install a command associated with a type
void SlowCommandExecutor::installCommand(const string& alias,
                                         chaos::common::utility::NestedObjectInstancer<SlowCommand, chaos_batch::BatchCommand> *instancer) {
    //call superclss method
    BatchCommandExecutor::installCommand(alias, instancer);
}

//! Install a command
void SlowCommandExecutor::installCommand(boost::shared_ptr<BatchCommandDescription> command_description) {
    //call superclss method
    BatchCommandExecutor::installCommand(command_description);
}

//! Check if the waithing command can be installed
BatchCommand *SlowCommandExecutor::instanceCommandInfo(const std::string& command_alias,
                                                      	uint32_t submission_rule,
                                                       uint32_t submission_retry_delay,
                                                       uint64_t scheduler_step_delay) {
    //install command into the batch command executor root class
    SlowCommand *result = (SlowCommand*) BatchCommandExecutor::instanceCommandInfo(command_alias,
                                                                                   submission_rule,
                                                                                   submission_retry_delay,
                                                                                   scheduler_step_delay);
    
    //cusotmize slow command sublcass
    if(result) {
        //forward the pointers of the needed data
        result->driverAccessorsErogator = driverAccessorsErogator;
        result->dataset_attribute_db_ptr = dataset_attribute_db_ptr;
        result->attribute_cache = attribute_cache;
    }
    return result;
}
//overlodaed command event handler
void SlowCommandExecutor::handleCommandEvent(const std::string& command_alias,
                                             uint64_t command_seq,
                                             BatchCommandEventType::BatchCommandEventType type,
                                             CDataWrapper *command_data) {
    
    //let the base class handle the event
    BatchCommandExecutor::handleCommandEvent(command_seq,
                                             type,
                                             command_data);
    switch(type) {
        case BatchCommandEventType::EVT_COMPLETED:{
            if(command_data) {
                std::string cached_parameter_name;
                CDataWrapperKeyList all_input_parameter;
                
                
                //in this case we need to set the attribute into the dataset for the state reached
                //(for now we update the inptu dataset only)
                
                //lock the input domain cache
                boost::shared_ptr<SharedCacheLockDomain> w_lock = getAttributeSharedCache()->getLockOnDomain(DOMAIN_INPUT, true);
                w_lock->lock();
                
                //get all parameter in command data
                command_data->getAllKey(all_input_parameter);
                
                //iterate all elemento found
                for(CDataWrapperKeyListIterator it = all_input_parameter.begin(), end = all_input_parameter.end();
                    it!=end;
                    it++) {
                    cached_parameter_name = command_alias + "/" +*it;
                    
                    //chec if the parameter is present in cache
                    if(getAttributeSharedCache()->hasAttribute(DOMAIN_INPUT,
                                                               cached_parameter_name) == false) continue;
                    
                    //get the cached element
                    AttributeValue *cached_element = getAttributeSharedCache()->getAttributeValue(DOMAIN_INPUT, cached_parameter_name);
                    
                    //update cached value with the input parameter of the command
                    cached_element->setValue(command_data->getRawValuePtr(*it), command_data->getValueSize(*it));
                }
            }
            //in case we have changed something, the dataset will be pushed
            control_unit_instance->pushInputDataset();
            break;
        }
            
        case BatchCommandEventType::EVT_FAULT: {
            
            if(command_data &&
               command_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE) &&
               command_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE) &&
               command_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN)) {
                const int32_t code = command_data->getInt32Value(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE);
                const std::string message = command_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE);
                const std::string domain = command_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN);
                
                //log error on metadata server
                error_logging_channel->logError(control_unit_instance->getCUID(),
                                                command_alias,
                                                code,
                                                message,
                                                domain);
                CException ex(code, message, domain);
                //async go into recoverable error
                boost::thread(boost::bind(&AbstractControlUnit::_goInRecoverableError, control_unit_instance, ex)).detach();
            } else {
                SCELERR_ << "Command id " << command_seq << " gone in fault without exception";
            }
        }
            break;
        default:
            break;
    }
    
    //log command event
    command_logging_channel->logCommandState(control_unit_instance->getCUID(),
                                             command_alias,
                                             command_seq,
                                             type,
                                             command_data);
}

//! general sandbox event handler
void SlowCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                             BatchSandboxEventType::BatchSandboxEventType type,
                                             void* type_value_ptr,
                                             uint32_t type_value_size) {
    //let the base class handle the event
    BatchCommandExecutor::handleSandboxEvent(sandbox_id, type, type_value_ptr, type_value_size);
    
    if(!last_ru_id_cache) {
        last_ru_id_cache = getAttributeSharedCache()->getAttributeValue(DOMAIN_SYSTEM,
                                                                        DataPackSystemKey::DP_SYS_RUN_UNIT_ID);
        if(!last_ru_id_cache) {
            SCELERR_ << "Error getting cache slot for unit id";
            return;
        }
    }
    
    //set the last system event sandbox id
    last_ru_id_cache->setValue(sandbox_id.c_str(), (uint32_t)sandbox_id.size());
    
    switch(type) {
        case BatchSandboxEventType::EVT_RUN_START: {
            uint64_t *hb = static_cast<uint64_t*>(type_value_ptr);
            control_unit_instance->_updateAcquistionTimestamp((uint64_t)(*hb/1000));
            
        }
            
        case BatchSandboxEventType::EVT_RUN_END: {
            //push output dataset specifingthat the ts has been already updated in casche directly
            control_unit_instance->pushOutputDataset(true);
            break;
        }
            
        case BatchSandboxEventType::EVT_UPDATE_RUN_DELAY:
            control_unit_instance->_updateRunScheduleDelay(*static_cast<uint64_t*>(type_value_ptr));
            control_unit_instance->pushSystemDataset();
            break;
    }
}
