/*
 *	BatchCommandExecutor.cpp
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

#define PURGE_TS_DELAY	500

#define LOG_HEAD_SBE "[BatchCommandExecutor-" << executorID << "] "

#define BCELAPP_ LAPP_ << LOG_HEAD_SBE
#define BCELDBG_ LDBG_ << LOG_HEAD_SBE
#define BCELERR_ LERR_ << LOG_HEAD_SBE

BatchCommandExecutor::BatchCommandExecutor(const std::string& _executorID):
executorID(_executorID),
default_command_sandbox_instance(COMMAND_BASE_SANDOXX_ID),
command_state_queue_max_size(COMMAND_STATE_QUEUE_DEFAULT_SIZE) {
    // this need to be removed from here need to be implemented the def undef services
    // register the public rpc api
    std::string rpcActionDomain = executorID; //+ BatchCommandExecutorRpcActionKey::COMMAND_EXECUTOR_POSTFIX_DOMAIN;
    // add executor has event handler
    
    
    BCELAPP_ << "Register getCommandSandboxStatistics action";
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::getCommandState,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE,
                                                                     "Return the state of the specified command");
    BCELAPP_ << "Register setCommandFeatures action";
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::setCommandFeatures,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES,
                                                                     "Set the features of the running command");
    BCELAPP_ << "Register killCurrentCommand action";
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::killCurrentCommand,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND,
                                                                     "Set the features of the running command");
    BCELAPP_ << "Register flushCommandStates action";
    DeclareAction::addActionDescritionInstance<BatchCommandExecutor>(this,
                                                                     &BatchCommandExecutor::flushCommandStates,
                                                                     rpcActionDomain.c_str(),
                                                                     BatchCommandExecutorRpcActionKey::RPC_FLUSH_COMMAND_HISTORY,
                                                                     "Flush the non active command history state");
    
    //if not sandbox has been added force to create one
    addNewSandboxInstance();
}

void BatchCommandExecutor::addNewSandboxInstance() {
    boost::shared_ptr<BatchCommandSandbox> tmp_ptr(new BatchCommandSandbox());
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
    for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
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
    WriteLock       lock(sandbox_map_mutex);
    
    //! add new instances
    for(unsigned int idx = 0; idx < _sandbox_number; idx++) {
        addNewSandboxInstance();
    }
}

BatchCommandExecutor::~BatchCommandExecutor() {
    
    //   BCELAPP_ << "Removing all the instance of sandbox";
    //   for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
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
    BCELAPP_ << "Removing all the instacer of the command";
    for(MapCommandDescriptionIterator it = map_command_description.begin();
        it != map_command_description.end();
        it++) {
        BCELAPP_ << "Dispose instancer " << it->first;
        // if(it->second) delete(it->second);
    }
    
    //clear the instancer command map
    map_command_description.clear();
    
    //clear the queue of the command state
    command_state_queue.clear();
}

// Initialize instance
void BatchCommandExecutor::init(void *initData) throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    //reset the command sequence on initialization
    command_sequence_id = 0;
    
    //broadcast init sequence to base class
    StartableService::init(initData);
    
    //initialize the shared channel setting
    InizializableService::initImplementation(global_attribute_cache,
                                             initData,
                                             "global_attribute_cache",
                                             __PRETTY_FUNCTION__);
    
    BCELAPP_ << "Initializing all the instance of sandbox";
    for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        boost::shared_ptr<BatchCommandSandbox> tmp_ptr =  it->second;
        //init the sand box
        BCELAPP_ << "Initilize instance " << tmp_ptr->identification;
        StartableService::initImplementation(tmp_ptr.get(),
                                             initData,
                                             "BatchCommandSandbox",
                                             __PRETTY_FUNCTION__);
    }
    
    BCELAPP_ << "Check if we need to use the dafult command or we have pause instance";
    if(default_command_alias.size()) {
        BCELAPP_ << "Set the default command ->"<<default_command_alias;
        BatchCommand * def_cmd_impl = instanceCommandInfo(default_command_alias, (CDataWrapper*)NULL);
        def_cmd_impl->unique_id = ++command_sequence_id;
        sandbox_map[default_command_sandbox_instance]->enqueueCommand(NULL, def_cmd_impl, 50);
        DEBUG_CODE(BCELDBG_ << "Command " << default_command_alias << " successfull installed";)
    }
}

// Start the implementation
void BatchCommandExecutor::start() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    try {
        // set thread run flag for work
        StartableService::start();
        
        BCELAPP_ << "Starting all the instance of sandbox";
        for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
            it != sandbox_map.end();
            it++) {
            boost::shared_ptr<BatchCommandSandbox> tmp_ptr =  it->second;
            BCELAPP_ << "Starting instance " << tmp_ptr->identification;
            //starting the sand box
            StartableService::startImplementation(tmp_ptr.get(),
                                                  "SlowCommandSandbox",
                                                  __PRETTY_FUNCTION__);
        }
        
        //start capper timer
        AsyncCentralManager::getInstance()->addTimer(this, PURGE_TS_DELAY, PURGE_TS_DELAY);
    } catch (...) {
        BCELAPP_ << "Error starting";
        throw CException(1000, "Generic Error", "BatchCommandExecutor::start");
    }
}

// Start the implementation
void BatchCommandExecutor::stop() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);

    //!remove capper timer
    AsyncCentralManager::getInstance()->removeTimer(this);
    
    //lock for queue access
    BCELAPP_ << "Stopping all the instance of sandbox";
    for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        boost::shared_ptr<BatchCommandSandbox> tmp_ptr =  it->second;
        BCELAPP_ << "Stop instance " << tmp_ptr->identification;
        
        //stopping the sand box
        StartableService::stopImplementation(tmp_ptr.get(),
                                             "SlowCommandSandbox",
                                             __PRETTY_FUNCTION__);
    }
    StartableService::stop();
}

// Deinit the implementation
void BatchCommandExecutor::deinit() throw(chaos::CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    BCELAPP_ << "Deinitializing all the instance of sandbox";
    for(std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> >::iterator it = sandbox_map.begin();
        it != sandbox_map.end();
        it++) {
        boost::shared_ptr<BatchCommandSandbox> tmp_ptr =  it->second;
        BCELAPP_ << "Deinitializing instance " << tmp_ptr->identification;
        //deinit the sand box
        StartableService::deinitImplementation(tmp_ptr.get(),
                                               "SlowCommandSandbox",
                                               __PRETTY_FUNCTION__);
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
                                              void* type_value_ptr,
                                              uint32_t type_value_size) {
    DEBUG_CODE(BCELDBG_ << "Received event of type->" << type << " on command id -> "<<command_id;)
    switch(type) {
        case BatchCommandEventType::EVT_QUEUED: {
            // get upgradable access
            boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
            
            // get exclusive access
            boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
            
            addComamndState(command_id);
            break;
        }
            
        case BatchCommandEventType::EVT_FAULT: {
            ReadLock lock(command_state_rwmutex);
            boost::shared_ptr<CommandState>  cmd_state = getCommandState(command_id);
            if(cmd_state.get()) {
                cmd_state->last_event = type;
                cmd_state->fault_description = *static_cast<FaultDescription*>(type_value_ptr);
            }
            break;
        }
        default: {
            ReadLock lock(command_state_rwmutex);
            boost::shared_ptr<CommandState>  cmd_state = getCommandState(command_id);
            if(cmd_state.get()) {
                cmd_state->last_event = type;
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
    boost::shared_ptr<CommandState> cmd_state(new CommandState());
    cmd_state->command_id = command_id;
    cmd_state->last_event = BatchCommandEventType::EVT_QUEUED;
    
    //add to the queue
    command_state_queue.push_back(cmd_state);
    //add also to the map for a fast access
    command_state_fast_access_map.insert(std::make_pair(command_id, cmd_state));
}

//! Thanke care to limit the size of the queue to the max size permitted
void BatchCommandExecutor::capCommanaQueue() {
    if(command_state_queue.size() <= command_state_queue_max_size) return;
    std::vector< boost::shared_ptr<CommandState> > cmd_state_to_reinsert;
    
    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
    
    // get exclusive access
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    
    //we need to cap the queue
    size_t idx = command_state_queue.size()-1;
    for (; idx >= command_state_queue_max_size; ) {
        if(command_state_queue.empty()) break;
        //get the state
        boost::shared_ptr<CommandState> cmd_state = command_state_queue.front();
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
    for(std::vector< boost::shared_ptr<CommandState> >::iterator iter = cmd_state_to_reinsert.begin();
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
boost::shared_ptr<CommandState> BatchCommandExecutor::getCommandState(uint64_t command_sequence) {
    boost::shared_ptr<CommandState> result;
    if(command_state_fast_access_map.count(command_sequence) > 0 ) {
        result = command_state_fast_access_map[command_sequence];
    }
    return result;
}

//! Perform a command registration
void BatchCommandExecutor::setDefaultCommand(const string& command_alias, unsigned int sandbox_instance) {
    // check if we can set the default, the condition are:
    // the executor and the sandbox are in the init state or in stop state
    if(StartableService::serviceState ==
       CUStateKey::START) {
        throw CException(1, "The command infrastructure is in running state", "BatchCommandExecutor::setDefaultCommand");
    }
    
    default_command_alias = command_alias;
    default_command_sandbox_instance = sandbox_instance;
    BCELAPP_ << "Install the default command with alias: " << default_command_alias;
}

const std::string& BatchCommandExecutor::getDefaultCommand() {
    return default_command_alias;
}

//! return all the command description
void BatchCommandExecutor::getCommandsDescriptions(std::vector< boost::shared_ptr<BatchCommandDescription> >& descriptions) {
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
    boost::shared_ptr<BatchCommandDescription> description(new BatchCommandDescription(alias,
                                                                                       "OLD unsupportd command style"));
    description->setInstancer(instancer);
    map_command_description.insert(make_pair<string,
                                   boost::shared_ptr<BatchCommandDescription> >(alias, description));
}

//! Install a command by his description
void BatchCommandExecutor::installCommand(boost::shared_ptr<BatchCommandDescription> command_description) {
    BCELAPP_ << "Install new command with alias -> " << command_description->getAlias();
    map_command_description.insert(make_pair<string,
                                   boost::shared_ptr<BatchCommandDescription> >(command_description->getAlias(), command_description));
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
                                                        uint32_t submission_rule,//SubmissionRuleType::SUBMIT_NORMAL
                                                        uint32_t submission_retry_delay,
                                                        uint64_t scheduler_step_delay) {
    DEBUG_CODE(BCELDBG_ << "Instancing command " << command_alias;)
    BatchCommand *instance = NULL;
    if(map_command_description.count(command_alias)) {
        boost::shared_ptr<BatchCommandDescription> description = map_command_description[command_alias];
        instance = description->instancer->getInstance();
        //forward the pointer of the driver accessor
        //result->driverAccessorsErogator = driverAccessorsErogator;
        if(instance) {
            instance->setCommandAlias(description->getAlias());
            instance->sharedAttributeCachePtr = &global_attribute_cache;
            //set the alias for this command
            //instance->setCommandAlias(command_alias);
            
            instance->submissionRule = submission_rule;
            DEBUG_CODE(BCELDBG_ << "Submission rule for command " << command_alias << " is: " << ((uint16_t)instance->submissionRule);)
            
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY;
            instance->commandFeatures.featureSchedulerStepsDelay = scheduler_step_delay;
            DEBUG_CODE(BCELDBG_ << "Set custom  SCHEDULER_STEP_TIME_INTERVALL to " << instance->commandFeatures.featureSchedulerStepsDelay << " microseconds";)
            
            instance->commandFeatures.featuresFlag |= features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY;
            instance->commandFeatures.featureSubmissionRetryDelay = submission_retry_delay;
            DEBUG_CODE(BCELDBG_ << "Set custom  SUBMISSION_RETRY_DELAY to " << instance->commandFeatures.featureSubmissionRetryDelay << " milliseconds";)
            
            //get the assigned id
            instance->unique_id = ++command_sequence_id;
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
                                         uint64_t& command_id)  throw (CException) {
    if(!commandDescription)
        throw CException(-1, "Invalid parameter", "BatchCommandExecutor::setCommandFeatures");
    
    if(serviceState !=
       CUStateKey::START)
        throw CException(-2, "Slow command executor is not started", "BatchCommandExecutor::submitCommand");
    
    WriteLock       lock(sandbox_map_mutex);
    
    //get execution channel if submitted
    uint32_t execution_channel = commandDescription->hasKey(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? commandDescription->getUInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;
    
    //check if the channel is present
    if(sandbox_map.count(execution_channel) == 0)
        throw CException(-3, "Execution channel not found", "BatchCommandExecutor::submitCommand");
    
    boost::shared_ptr<BatchCommandSandbox> tmp_ptr = sandbox_map[execution_channel];
    
    //get priority if submitted
    uint32_t priority = commandDescription->hasKey(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32) ? commandDescription->getUInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32):50;
    
    BCELDBG_ << "Submit new command "<<batch_command_alias << "with info:" << commandDescription->getJSONString();
    
    //add unique id for command
    //command_id = ++command_sequence_id;
    //commandDescription->addInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_id);
    
    //queue the command
    BatchCommand *cmd_instance = instanceCommandInfo(batch_command_alias, commandDescription);
    if(cmd_instance) {
        //report unique id
        command_id = cmd_instance->unique_id;
        
        //enqueue command insandbox
        tmp_ptr->enqueueCommand(commandDescription, cmd_instance, priority);
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
                                         uint64_t scheduler_step_delay)  throw (CException) {
    if(sandbox_map.count(execution_channel) == 0)
        throw CException(-3, "Execution channel not found", "BatchCommandExecutor::submitCommand");
    
    boost::shared_ptr<BatchCommandSandbox> sandbox_ptr = sandbox_map[execution_channel];

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
        //report unique id
        command_id = cmd_instance->unique_id;
        
        //enqueue command insandbox
        sandbox_ptr->enqueueCommand(command_data, cmd_instance, priority);
    } else {
        throw CException(-4, "Command instantiation failed", "BatchCommandExecutor::submitCommand");
        
    }
}
//----------------------------public rpc command---------------------------

//! Get queued command via rpc command
/*
 Return the number and the infromation of the queued command via RPC
 */
CDataWrapper* BatchCommandExecutor::getCommandState(CDataWrapper *params, bool& detachParam) throw (CException) {
    BCELAPP_ << "Get command state fromthe executor with id: " << executorID;
    //boost::mutex::scoped_lock lock(mutextQueueManagment);
    ReadLock lock(command_state_rwmutex);
    uint64_t command_id = params->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    boost::shared_ptr<CommandState> cmd_state = getCommandState(command_id);
    if(!cmd_state.get()) throw CException(1, "The command requested is not present", "BatchCommandExecutor::getCommandSandboxStatistics");
    
    CDataWrapper *result = new CDataWrapper();
    
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
CDataWrapper* BatchCommandExecutor::setCommandFeatures(CDataWrapper *params, bool& detachParam) throw (CException) {
    if(!params || sandbox_map.size()==0)
        throw CException(-1, "Invalid parameter", "BatchCommandExecutor::setCommandFeatures");
    
    ReadLock       lock(sandbox_map_mutex);
    
    BCELAPP_ << "Set command feature on current command into the executor with id: " << executorID;
    //get execution channel if submitted
    uint32_t execution_channel = params->hasKey(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL) ? params->getUInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL):COMMAND_BASE_SANDOXX_ID;
    
    boost::shared_ptr<BatchCommandSandbox> tmp_ptr = sandbox_map[execution_channel];
    
    //lock the scheduler
    boost::mutex::scoped_lock lockForCurrentCommand(tmp_ptr->mutextAccessCurrentCommand);
    
    if(!tmp_ptr->currentExecutingCommand)
        throw CException(2, "No Current running command", "BatchCommandExecutor::setCommandFeatures");
    
    //check wath feature we need to setup
    if(params->hasKey(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL)) {
        //has lock information to setup
        tmp_ptr->currentExecutingCommand->element->cmdImpl->lockFeaturePropertyFlag[0] = params->getBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL);
    }
    
    if(params->hasKey(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64)) {
        //has scheduler step wait
        tmp_ptr->currentExecutingCommand->element->cmdImpl->commandFeatures.featureSchedulerStepsDelay = params->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64);
    }
    
    //recheck current command
    if(!tmp_ptr->currentExecutingCommand) return NULL;
    
    
    lockForCurrentCommand.unlock();
    if(tmp_ptr->threadSchedulerPauseCondition.isInWait())
        tmp_ptr->threadSchedulerPauseCondition.unlock();
    return NULL;
}

//! Command features modification rpc action
/*!
 Updat ethe modiable features of the running command
 */
void BatchCommandExecutor::setCommandFeatures(features::Features& features) throw (CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    boost::shared_ptr<BatchCommandSandbox> tmp_ptr = sandbox_map[0];
    tmp_ptr->setCommandFeatures(features);
}

//! Kill current command rpc action
CDataWrapper* BatchCommandExecutor::killCurrentCommand(CDataWrapper *params, bool& detachParam) throw (CException) {
    ReadLock       lock(sandbox_map_mutex);
    
    boost::shared_ptr<BatchCommandSandbox> tmp_ptr = sandbox_map[0];
    
    if(!tmp_ptr->currentExecutingCommand) return NULL;
    BCELAPP_ << "Kill current command into the executor id: " << executorID;
    tmp_ptr->killCurrentCommand();
    return NULL;
}

//! Flush the command state history
CDataWrapper* BatchCommandExecutor::flushCommandStates(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException) {
    BCELAPP_ << "Flushing all endend command state history for executr id:" << executorID;
    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(command_state_rwmutex);
    
    while (!command_state_queue.empty() )  {
        //remove command form
        boost::shared_ptr<CommandState> cmd_state = command_state_queue.back();
        
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
    return NULL;
}