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

#include <boost/lexical_cast.hpp>

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/batch_command/BatchCommandExecutor.h>
#include <chaos/common/batch_command/BatchCommandConstants.h>


using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::batch_command;
using namespace chaos::common::data::cache;
#define PURGE_TS_DELAY	500

#define LOG_HEAD_SBE "[BatchCommandExecutor-" << executorID << "] "

#define BCELAPP_ LAPP_ << LOG_HEAD_SBE
#define BCELDBG_ LDBG_ << LOG_HEAD_SBE
#define BCELERR_ LERR_ << LOG_HEAD_SBE

BatchCommandExecutor::BatchCommandExecutor(const std::string& _executorID,
                                           bool serialized_sandbox):
serialized_sandbox(serialized_sandbox),
executorID(_executorID),
default_command_stickyness(true),
default_command_sandbox_instance(COMMAND_BASE_SANDOXX_ID),
command_state_queue_max_size(COMMAND_STATE_QUEUE_DEFAULT_SIZE) {
    // this need to be removed from here need to be implemented the def undef services
    // register the public rpc api
    std::string rpcActionDomain = executorID; //+ BatchCommandExecutorRpcActionKey::COMMAND_EXECUTOR_POSTFIX_DOMAIN;
    // add executor has event handler
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::getCommandState,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE,
                                                                     "Return the state of the specified command");
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::setCommandFeatures,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES,
                                                                     "Set the features of the running command");
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::killCurrentCommand,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND,
                                                                     "Kill the running command");
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::clearCommandQueue,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_CLEAR_COMMAND_QUEUE,
                                                                     "Clear the command queue removing all pending command");
    
    //if not sandbox has been added force to create one
    addNewSandboxInstance();
}

void BatchCommandExecutor::addNewSandboxInstance() {
    ChaosSharedPtr<AbstractSandbox> tmp_ptr;
    if(serialized_sandbox == true) {
        tmp_ptr = ChaosMakeSharedPtr<BatchCommandSandbox>();
    } else {
        tmp_ptr = ChaosMakeSharedPtr<BatchCommandParallelSandbox>();
    }
    tmp_ptr->event_handler = this;
    tmp_ptr->identification.append(executorID);
    tmp_ptr->identification.append("-sandbox-");
    
    unsigned int new_size = ((unsigned int) sandbox_map.size() + COMMAND_BASE_SANDOXX_ID);
    tmp_ptr->identification.append(lexical_cast<std::string>(new_size));
    
    
    sandbox_map.insert(make_pair(new_size, tmp_ptr));
    BCELDBG_ << "Add new sandbox to the executor with identification ->" << tmp_ptr->identification;
}

unsigned int BatchCommandExecutor::getNumberOfSandboxInstance() {
    return (unsigned int)sandbox_map.size();
}

void BatchCommandExecutor::getSandboxID(std::vector<std::string> & sandbox_id){
    for(std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> >::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++){
        sandbox_id.push_back(it->second->identification);
    }
}

AbstractSharedDomainCache *BatchCommandExecutor::getAttributeSharedCache() {
    return &global_attribute_cache;
}

//! Add a number of sandobx to this instance of executor
void BatchCommandExecutor::addSandboxInstance(unsigned int _sandbox_number) {
    //lock for map modification
    WriteLock lock(sandbox_map_mutex);
    
    //! add new instances
    for(unsigned int idx = 0; idx < _sandbox_number; idx++) {
        addNewSandboxInstance();
    }
}

BatchCommandExecutor::~BatchCommandExecutor() {
    
    //   BCELAPP_ << "Removing all the instance of sandbox";
    //   for(std::map<unsigned int, ChaosSharedPtr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
    //       it != sandbox_map.end();
    //      it++) {
    //      BCELAPP_ << "Dispose instance " << it->first;
    //      if(it->second) delete(it->second);
    //  }
    
    // the instancer of the command can't be uninstalled at deinit step
    // because the executor live  one o one with the control unit.
    // In teh CU the commadn are installed at the definition step
    // this mean that until executor live the command remain installed
    // and the CU is not redefined unti it is reloaded but startup
    // a and so new executor will be used
    BCELAPP_ << "Removing all the instances of the command";
    for(MapCommandDescriptionIterator it = map_command_description.begin();
        it != map_command_description.end();
        it++) {
        BCELAPP_ << "Dispose instance " << it->first;
        // if(it->second) delete(it->second);
    }
    
    //clear the instancer command map
    map_command_description.clear();
    
    //clear the queue of the command state
    command_state_queue.clear();
}

// Initialize instance
void BatchCommandExecutor::init(void *initData)  {
    
    //reset the command sequence on initialization(id 0 is used for stiky command by sandboxes)
    //command_sequence_id = 0;
    
    //broadcast init sequence to base class
    StartableService::init(initData);
    
    //initialize the shared channel setting
    InizializableService::initImplementation(global_attribute_cache,
                                             initData,
                                             "global_attribute_cache",
                                             __PRETTY_FUNCTION__);
    
    BCELAPP_ << "Initializing all the instance of sandbox";
    {
        ReadLock       lock(sandbox_map_mutex);
        for(std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> >::iterator it = sandbox_map.begin();
            it != sandbox_map.end();
            it++) {
            ChaosSharedPtr<AbstractSandbox> tmp_ptr =  it->second;
            //init the sand box
            BCELAPP_ << "Initialize instance " << tmp_ptr->identification;
            StartableService::initImplementation(tmp_ptr.get(),
                                                 initData,
                                                 "BatchCommandSandbox",
                                                 __PRETTY_FUNCTION__);
        }
    }
    BCELAPP_ << "Check if we need to use the default command or we have pause instance";
}

// Start the implementation
void BatchCommandExecutor::start()  {
    try {
        // set thread run flag for work
        StartableService::start();
        
        BCELAPP_ << "Starting all the instance of sandbox";
        {
            ReadLock       lock(sandbox_map_mutex);
            for(std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> >::iterator it = sandbox_map.begin();
                it != sandbox_map.end();
                it++) {
                ChaosSharedPtr<AbstractSandbox> tmp_ptr =  it->second;
                BCELAPP_ << "Starting instance " << tmp_ptr->identification;
                //starting the sand box
                StartableService::startImplementation(tmp_ptr.get(),
                                                      "SlowCommandSandbox",
                                                      __PRETTY_FUNCTION__);
            }
        }
        //start capper timer
        AsyncCentralManager::getInstance()->addTimer(this, PURGE_TS_DELAY, PURGE_TS_DELAY);
    } catch (...) {
        BCELAPP_ << "Error starting";
        throw CException(1000, "Generic Error", "BatchCommandExecutor::start");
    }
}

// Start the implementation
void BatchCommandExecutor::stop()  {
    
    //!remove capper timer
    AsyncCentralManager::getInstance()->removeTimer(this);
    
    //lock for queue access
    BCELAPP_ << "Stopping all the instance of sandbox";
    {
        ReadLock       lock(sandbox_map_mutex);
        for(std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> >::iterator it = sandbox_map.begin();
            it != sandbox_map.end();
            it++) {
            ChaosSharedPtr<AbstractSandbox> tmp_ptr =  it->second;
            BCELAPP_ << "Stop instance " << tmp_ptr->identification;
            
            //stopping the sand box
            StartableService::stopImplementation(tmp_ptr.get(),
                                                 "SlowCommandSandbox",
                                                 __PRETTY_FUNCTION__);
        }
    }
    StartableService::stop();
}

// Deinit the implementation
void BatchCommandExecutor::deinit()  {
    {
        ReadLock       lock(sandbox_map_mutex);
        
        BCELAPP_ << "Deinitializing all the instance of sandbox";
        for(std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> >::iterator it = sandbox_map.begin();
            it != sandbox_map.end();
            it++) {
            ChaosSharedPtr<AbstractSandbox> tmp_ptr =  it->second;
            BCELAPP_ << "Deinitializing instance " << tmp_ptr->identification;
            //deinit the sand box
            StartableService::deinitImplementation(tmp_ptr.get(),
                                                   "SlowCommandSandbox",
                                                   __PRETTY_FUNCTION__);
        }
    }
    //initialize the shared channel setting
    InizializableService::deinitImplementation(global_attribute_cache,
                                               "AttributeCache",
                                               __PRETTY_FUNCTION__);
    
    StartableService::deinit();
}

//command event handler
void BatchCommandExecutor::handleCommandEvent(uint64_t command_id,
                                              BatchCommandEventType::BatchCommandEventType type,
                                              CDataWrapper *command_info,
                                              const BatchCommandStat& commands_stats) {
    DEBUG_CODE(BCELDBG_ << "Received event of type->" << type << " on command id -> "<<command_id;)
    switch(type) {
        case BatchCommandEventType::EVT_QUEUED: {
            // get upgradable access
            boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
            
            // get exclusive access
            boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
            DEBUG_CODE(BCELDBG_ << "Received event of type-> Command QUEUED" << " command id -> "<<command_id);
            
            addComamndState(command_id);
            
            return;
        }
            
        case BatchCommandEventType::EVT_FAULT: {
            DEBUG_CODE(BCELDBG_ << "Received event of type-> Command FAULT" << " command id -> "<<command_id);
            
            if(command_info != NULL) {
                ReadLock lock(command_state_rwmutex);
                ChaosSharedPtr<CommandState>  cmd_state = getCommandState(command_id);
                if(cmd_state.get()) {
                    cmd_state->last_event = type;
                    cmd_state->fault_description.code = command_info->getInt32Value(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE);
                    cmd_state->fault_description.description = command_info->getStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE);
                    cmd_state->fault_description.domain = command_info->getStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN);
                }
            }
            break;
        }
            
        default:{
            ReadLock lock(command_state_rwmutex);
            ChaosSharedPtr<CommandState>  cmd_state = getCommandState(command_id);
            if(cmd_state.get()) {
                DEBUG_CODE(BCELAPP_ << "Set running type event on command id:"<<command_id);
                cmd_state->last_event = type;
            } else {
                DEBUG_CODE(BCELERR_ << "No command found for id:"<<command_id);
            }
            break;
        }
            
            
    }

}

//! general sandbox event handler
void BatchCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
                                              BatchSandboxEventType::BatchSandboxEventType type,
                                              void* type_value_ptr,
                                              uint32_t type_value_size) {
    
}


//! Add a new command state structure to the queue (checking the alredy presence)
void BatchCommandExecutor::addComamndState(uint64_t command_id) {
    //WriteLock write_lock(command_state_rwmutex);
    ChaosSharedPtr<CommandState> cmd_state(new CommandState());
    cmd_state->command_id = command_id;
    cmd_state->last_event = BatchCommandEventType::EVT_QUEUED;
    
    //add to the queue
    command_state_queue.push_back(cmd_state);
    //add also to the map for a fast access
    command_state_fast_access_map.insert(std::make_pair(command_id, cmd_state));
}

//! Thanke care to limit the size of the queue to the max size permitted
void BatchCommandExecutor::capCommanaQueue() {
    
    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
    if(command_state_queue.size() <= command_state_queue_max_size) return;
    // get exclusive access
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    std::vector< ChaosSharedPtr<CommandState> > cmd_state_to_reinsert;
    
    //we need to cap the queue
    size_t idx = command_state_queue.size()-1;
    for (; idx >= command_state_queue_max_size; ) {
        if(command_state_queue.empty()) break;
        //get the state
        ChaosSharedPtr<CommandState> cmd_state = command_state_queue.front();
        //remove it from the from
        command_state_queue.pop_front();
        
        //chec if the command can be removed it need to be terminate (complete, fault or killed)
        if(cmd_state->last_event < BatchCommandEventType::EVT_COMPLETED) {
            //the command state need to be reinsert at the end of
            cmd_state_to_reinsert.push_back(cmd_state);
            continue;
        }
        //remove from the map
        command_state_fast_access_map.erase(cmd_state->command_id);
        
        //decremente the index
        idx--;
    }
    
    //reinsert the element to preserv
    for(std::vector< ChaosSharedPtr<CommandState> >::iterator iter = cmd_state_to_reinsert.begin();
        iter != cmd_state_to_reinsert.end();
        iter++) {
        command_state_queue.push_front(*iter);
    }
    
    cmd_state_to_reinsert.clear();
}

void BatchCommandExecutor::timeout() {
    //capping the queue
    capCommanaQueue();
}

//! Add a new command state structure to the queue (checking the alredy presence)
ChaosSharedPtr<CommandState> BatchCommandExecutor::getCommandState(uint64_t command_sequence) {
    ChaosSharedPtr<CommandState> result;
    if(command_state_fast_access_map.count(command_sequence) > 0 ) {
        result = command_state_fast_access_map[command_sequence];
    }
    return result;
}

//! return the state of a command
ChaosUniquePtr<CommandState> BatchCommandExecutor::getStateForCommandID(uint64_t command_id) {
    // get upgradable access
    ChaosUniquePtr<CommandState> result;
    ReadLock lock(command_state_rwmutex);
    ChaosSharedPtr<CommandState> _internal_state = getCommandState(command_id);
    if(_internal_state.get()) {
        result.reset(new CommandState());
        *result.get() = *_internal_state.get();
        
    }
    return result;
}

//! Perform a command registration
void BatchCommandExecutor::setDefaultCommand(const string& command_alias,
                                             unsigned int sandbox_instance) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(StartableService::serviceState ==
       CUStateKey::START) {
        throw CException(1, "The command infrastructure is in running state", "BatchCommandExecutor::setDefaultCommand");
    }
    
    default_command_alias = command_alias;
    ReadLock       lock(sandbox_map_mutex);
    BCELAPP_ << "Install the default command ->"<<"\""<<default_command_alias<<"\"";
    BatchCommand * def_cmd_impl = instanceCommandInfo(default_command_alias, (CDataWrapper*)NULL);
    if(def_cmd_impl) {
        sandbox_map[default_command_sandbox_instance]->setDefaultStickyCommand(def_cmd_impl);
        DEBUG_CODE(BCELDBG_ << "Command \"" << default_command_alias << "\" successfully installed";)
    }else {
        DEBUG_CODE(BCELERR_ << "Command \"" << default_command_alias << "\" Not found";)
    }
}

const std::string& BatchCommandExecutor::getDefaultCommand() {
    return default_command_alias;
}

//! return all the command description
void BatchCommandExecutor::getCommandsDescriptions(std::vector< ChaosSharedPtr<BatchCommandDescription> >& descriptions) {
    for(MapCommandDescriptionIterator it = map_command_description.begin();
        it != map_command_description.end();
        it++) {
        //add shared ptr with the vector
        descriptions.push_back(it->second);
    }
}

//! Install a command associated with a type
void BatchCommandExecutor::installCommand(const string& alias, chaos::common::utility::ObjectInstancer<BatchCommand> *instancer) {
    BCELAPP_ << "Install new command with alias -> " << alias;
    ChaosSharedPtr<BatchCommandDescription> description(new BatchCommandDescription(alias,
                                                                                       "OLD unsupportd command style"));
    description->setInstancer(instancer);
    map_command_description.insert(make_pair(alias, description));
}

//! Install a command by his description
void BatchCommandExecutor::installCommand(ChaosSharedPtr<BatchCommandDescription> command_description) {
    BCELAPP_ << "Install new command with alias -> \"" << command_description->getAlias()<<"\"";
    map_command_description.insert(make_pair(command_description->getAlias(), command_description));
}

void BatchCommandExecutor::getAllCommandAlias(std::vector<std::string>& commands_alias) {
    for(MapCommandDescriptionIterator it = map_command_description.begin();
        it != map_command_description.end();
        it++) {
        commands_alias.push_back(it->first);
    }
}

//! Check if the waithing command can be installed
BatchCommand *BatchCommandExecutor::instanceCommandInfo(const std::string& command_alias, CDataWrapper *submissionInfo) {
    uint32_t submission_rule = SubmissionRuleType::SUBMIT_NORMAL;
    uint32_t submission_retry_delay = 1000000;
    uint64_t scheduler_step_delay = 1000000;
    
    if(submissionInfo) {
        if(submissionInfo->hasKey(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32)) {
            submission_rule = submissionInfo->getInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32);
        }
        
        //check if the comamnd pack has some feature to setup in the command instance
        if(submissionInfo->hasKey(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64)) {
            scheduler_step_delay = submissionInfo->getInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64);
        }
        
        if(submissionInfo->hasKey(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32)) {
            submission_retry_delay = submissionInfo->getInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32);
        }
    }
    return instanceCommandInfo(command_alias,
                               submission_rule,
                               submission_retry_delay,
                               scheduler_step_delay);
}

//! Check if the waithing command can be installed
BatchCommand *BatchCommandExecutor::instanceCommandInfo(const std::string& command_alias,
                                                        uint32_t submission_rule,
                                                        uint32_t submission_retry_delay,
                                                        uint64_t scheduler_step_delay) {
    BatchCommand *instance = NULL;
    if(map_command_description.count(command_alias)) {
        ChaosSharedPtr<BatchCommandDescription> description = map_command_description[command_alias];
        instance = description->getInstance();
        DEBUG_CODE(BCELDBG_ << "Instancing command \"" << command_alias<<"\" sticky/default:"<<instance->sticky;)
        
        //forward the pointer of the driver accessor
        //result->driverAccessorsErogator = driverAccessorsErogator;
        if(instance) {
            instance->setCommandAlias(description->getAlias());
            instance->sharedAttributeCachePtr = &global_attribute_cache;
            //set the alias for this command
            //instance->setCommandAlias(command_alias);
            
            instance->submissionRule = submission_rule;
            DEBUG_CODE(BCELDBG_ << "Submission rule for command \"" << command_alias << "\" is: " << ((uint16_t)instance->submissionRule);)
            
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY;
            instance->commandFeatures.featureSchedulerStepsDelay = scheduler_step_delay;
            DEBUG_CODE(BCELDBG_ << "Set custom  SCHEDULER_STEP_TIME_INTERVALL to " << instance->commandFeatures.featureSchedulerStepsDelay << " microseconds";)
            
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY;
            instance->commandFeatures.featureSubmissionRetryDelay = submission_retry_delay;
            DEBUG_CODE(BCELDBG_ << "Set custom  SUBMISSION_RETRY_DELAY to " << instance->commandFeatures.featureSubmissionRetryDelay << " milliseconds";)
        } else {
            DEBUG_CODE(BCELDBG_ << "Error instantiating the command " << command_alias;)
        }
    } else {
        DEBUG_CODE(BCELDBG_ << "No command found for alias " << command_alias;)
    }
    return instance;
}


// Submite a batch command
void BatchCommandExecutor::submitCommand(const std::string& batch_command_alias,
                                         CDataWrapper *commandDescription,
                                         uint64_t& command_id)   {
    if(!commandDescription)
        throw CException(-1, "Invalid parameter", "BatchCommandExecutor::setCommandFeatures");
    
    if(serviceState !=
       CUStateKey::START)
        throw CException(-2, "Slow command executor is not started", "BatchCommandExecutor::submitCommand");
    
    WriteLock lock(sandbox_map_mutex);
    
    //get execution channel if submitted
    uint32_t execution_channel = commandDescription->hasKey(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? commandDescription->getUInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;
    
    //check if the channel is present
    if(sandbox_map.count(execution_channel) == 0)
        throw CException(-3, "Execution channel not found", "BatchCommandExecutor::submitCommand");
    
    ChaosSharedPtr<AbstractSandbox> tmp_ptr = sandbox_map[execution_channel];
    
    //get priority if submitted
    uint32_t priority = commandDescription->hasKey(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32) ? commandDescription->getUInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32):50;
    BCELDBG_ << "Submit new command "<<batch_command_alias << "with info:" << commandDescription->getJSONString();
    //queue the command
    BatchCommand *cmd_instance = instanceCommandInfo(batch_command_alias, commandDescription);
    if(cmd_instance) {
        //enqueue command insandbox
        tmp_ptr->enqueueCommand(commandDescription, cmd_instance, priority);
        
        //report unique id
        command_id = cmd_instance->unique_id;
    } else {
        throw CException(-4, "Command instantiation failed", "BatchCommandExecutor::submitCommand");
    }
}

void BatchCommandExecutor::submitCommand(const std::string& batch_command_alias,
                                         chaos_data::CDataWrapper *command_data,
                                         uint64_t& command_id,
                                         uint32_t execution_channel,
                                         uint32_t priority,
                                         uint32_t submission_rule,
                                         uint32_t submission_retry_delay,
                                         uint64_t scheduler_step_delay)   {
    if(sandbox_map.count(execution_channel) == 0)
        throw CException(-3, "Execution channel not found", "BatchCommandExecutor::submitCommand");
    
    WriteLock lock(sandbox_map_mutex);
    
    ChaosSharedPtr<AbstractSandbox> sandbox_ptr = sandbox_map[execution_channel];
    
    BCELDBG_ << "Submit new command "<< batch_command_alias <<
    "with execution_channel:" << execution_channel <<
    " priority:"<<priority<<
    " submission_rule:"<<submission_rule<<
    " submission_retry_delay:"<<submission_retry_delay<<
    " scheduler_step_delay:"<<scheduler_step_delay;
    
    //queue the command
    BatchCommand *cmd_instance = instanceCommandInfo(batch_command_alias,
                                                     submission_rule,
                                                     submission_retry_delay,
                                                     scheduler_step_delay);
    if(cmd_instance) {
        //enqueue command insandbox
        sandbox_ptr->enqueueCommand(command_data, cmd_instance, priority);
        
        //report unique id
        command_id = cmd_instance->unique_id;
    } else {
        throw CException(-4, "Command instantiation failed", "BatchCommandExecutor::submitCommand");
        
    }
}
//----------------------------public rpc command---------------------------

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDWUniquePtr BatchCommandExecutor::getCommandState(CDWUniquePtr params)  {
    BCELAPP_ << "Get command state from the executor with id: " << executorID;
    //boost::mutex::scoped_lock lock(mutextQueueManagment);
    ReadLock lock(command_state_rwmutex);
    uint64_t command_id = params->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    ChaosSharedPtr<CommandState> cmd_state = getCommandState(command_id);
    if(!cmd_state.get()) throw CException(1, "The command requested is not present", "BatchCommandExecutor::getCommandSandboxStatistics");
    
    CreateNewDataWrapper(result,);
    //add statistic to result
    result->addInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_LAST_EVENT_UI32, cmd_state->last_event);
    if(cmd_state->last_event == BatchCommandEventType::EVT_FAULT) {
        result->addInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_CODE_UI32, cmd_state->fault_description.code);
        result->addStringValue(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DESC_STR, cmd_state->fault_description.description);
        result->addStringValue(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR, cmd_state->fault_description.domain);
    }
    return result;
}

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDWUniquePtr BatchCommandExecutor::setCommandFeatures(CDWUniquePtr params)  {
    if(!params.get() || sandbox_map.size()==0)
        throw CException(-1, "Invalid parameter", "BatchCommandExecutor::setCommandFeatures");
    
    ReadLock   lock(sandbox_map_mutex);
    
    BCELAPP_ << "Set command feature on current command into the executor with id: " << executorID;
    //get execution channel if submitted
    uint32_t execution_channel = params->hasKey(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? params->getUInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;
    
    ChaosSharedPtr<AbstractSandbox> tmp_ptr = sandbox_map[execution_channel];
    
    //check wath feature we need to setup
    if(params->hasKey(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL)) {
        //has lock information to setup
        tmp_ptr->lockCurrentCommandFeature(params->getBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL));
    }
    
    if(params->hasKey(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64)) {
        //has scheduler step wait
        tmp_ptr->setCurrentCommandScheduerStepDelay(params->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64));
    }
    return CDWUniquePtr();
}

//! Command features modification rpc action
/*!
 Updat ethe modiable features of the running command
 */
void BatchCommandExecutor::setCommandFeatures(features::Features& features)  {
    ReadLock       lock(sandbox_map_mutex);
    
    ChaosSharedPtr<AbstractSandbox> tmp_ptr = sandbox_map[0];
    tmp_ptr->setCurrentCommandFeatures(features);
}

//! Kill current command rpc action
CDWUniquePtr BatchCommandExecutor::killCurrentCommand(CDWUniquePtr params)  {
    ReadLock       lock(sandbox_map_mutex);
    ChaosSharedPtr<AbstractSandbox> tmp_ptr = sandbox_map[0];
    tmp_ptr->killCurrentCommand();
    return CDWUniquePtr();
}

//! Kill current command rpc action
CDWUniquePtr BatchCommandExecutor::clearCommandQueue(CDWUniquePtr params)  {
    ReadLock       lock(sandbox_map_mutex);
    ChaosSharedPtr<AbstractSandbox> tmp_ptr = sandbox_map[0];
    tmp_ptr->clearCommandQueue();
    return CDWUniquePtr();
}

//! Flush the command state history
CDWUniquePtr BatchCommandExecutor::flushCommandStates(CDWUniquePtr params)  {
    BCELAPP_ << "Flushing all endend command state history for executr id:" << executorID;
    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
    
    while (!command_state_queue.empty() )  {
        //remove command form
        ChaosSharedPtr<CommandState> cmd_state = command_state_queue.back();
        
        //chec if the command can be removed it need to be terminate (complete, fault or killed)
        if(cmd_state->last_event < BatchCommandEventType::EVT_COMPLETED) break;
        BCELAPP_ << "Flushing command with id:" << cmd_state->command_id ;
        // get exclusive access
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
        //remove from the map
        command_state_fast_access_map.erase(cmd_state->command_id);
        
        //delete it
        command_state_queue.pop_back();
    }
    return CDWUniquePtr();
}
