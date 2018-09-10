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

#define SCELAPP_ INFO_LOG_1_P(SlowCommandExecutor, control_unit_instance->getDeviceID())
#define SCELDBG_ ERR_LOG_1_P(SlowCommandExecutor, control_unit_instance->getDeviceID())
#define SCELERR_ DBG_LOG_1_P(SlowCommandExecutor, control_unit_instance->getDeviceID())
using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;
using namespace chaos::common::metadata_logging;

SlowCommandExecutor::SlowCommandExecutor(const std::string& _executorID,
                                         SCAbstractControlUnit *_control_unit_instance):
BatchCommandExecutor(_executorID),
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
void SlowCommandExecutor::init(void *initData)  {
    error_logging_channel = (ErrorLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("ErrorLoggingChannel");
    if(error_logging_channel == NULL) {throw CException(-1, "No metadata logging error channel found", __PRETTY_FUNCTION__);}
    
    command_logging_channel = (BatchCommandLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("BatchCommandLoggingChannel");
    if(command_logging_channel == NULL) {throw CException(-2, "No metadata logging batch command channel found", __PRETTY_FUNCTION__);}
    //initialize superclass
    BatchCommandExecutor::init(initData);
}

// Start the implementation
void SlowCommandExecutor::start()  {
    //start superclass
    BatchCommandExecutor::start();
}


// Start the implementation
void SlowCommandExecutor::stop()  {
	SCELDBG_ << "stopping";
    //initialize superclass
    BatchCommandExecutor::stop();
}

// Deinit instance
void SlowCommandExecutor::deinit()  {
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
    SCELDBG_ << "deinitialized";
    
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
void SlowCommandExecutor::installCommand(ChaosSharedPtr<BatchCommandDescription> command_description) {
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
        result->abstract_control_unit = control_unit_instance;
        result->attribute_cache = attribute_cache;
    }
    return result;
}
//overlodaed command event handler
void SlowCommandExecutor::handleCommandEvent(const std::string& command_alias,
                                             uint64_t command_seq,
                                             BatchCommandEventType::BatchCommandEventType type,
                                             CDataWrapper *command_data,
                                             const BatchCommandStat& commands_stats) {
    
    //let the base class handle the event
    BatchCommandExecutor::handleCommandEvent(command_seq,
                                             type,
                                             command_data,
                                             commands_stats);
    switch(type) {
    case BatchCommandEventType::EVT_FATAL_FAULT:
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
                if(type==BatchCommandEventType::EVT_FATAL_FAULT){
                	CFatalException ex(code, message, domain);
                //async go into recoverable error
                	boost::thread(boost::bind(&AbstractControlUnit::_goInRecoverableError, control_unit_instance, ex)).detach();
                }
            }
            break;
        }

        default:
            break;
    }
    
    //log command event
    command_logging_channel->logCommandState(control_unit_instance->getCUID(),
                                             command_alias,
                                             command_seq,
                                             type,
                                             command_data);
    //update queued and stacked command on system dataset
    AttributeCache& sys_cache = getAttributeSharedCache()->getSharedDomain(DOMAIN_SYSTEM);
    sys_cache.getValueSettingByName(DataPackSystemKey::DP_SYS_QUEUED_CMD)->setValue(CDataVariant(commands_stats.queued_commands));
    sys_cache.getValueSettingByName(DataPackSystemKey::DP_SYS_STACK_CMD)->setValue(CDataVariant(commands_stats.stacked_commands));
    control_unit_instance->pushSystemDataset();
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
    last_ru_id_cache->setStringValue(sandbox_id);
    
    switch(type) {
        case BatchSandboxEventType::EVT_RUN_START: {
            uint64_t *hb = static_cast<uint64_t*>(type_value_ptr);
            control_unit_instance->_updateAcquistionTimestamp((uint64_t)*hb);
            
        }
            
        case BatchSandboxEventType::EVT_RUN_END: {
            //push output dataset specifingthat the ts has been already updated in casche directly
            control_unit_instance->pushInputDataset();
            control_unit_instance->pushOutputDataset();
            break;
        }
            
        case BatchSandboxEventType::EVT_UPDATE_RUN_DELAY:
            control_unit_instance->_updateRunScheduleDelay(*static_cast<uint64_t*>(type_value_ptr));
            control_unit_instance->pushSystemDataset();
            break;
    }
}
