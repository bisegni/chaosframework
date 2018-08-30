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
#include <chaos/common/exception/CException.h>
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/dispatcher/DefaultCommandDispatcher.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
using namespace chaos;
using namespace chaos::common::data;
//namespace chaos_data = chaos::common::data;

using namespace std;
using namespace boost;

#define LDEF_CMD_DISPTC_APP_ LAPP_ << "[DefaultCommandDispatcher] - "
#define LDEF_CMD_DISPTC_DBG_ LDBG_ << "[DefaultCommandDispatcher] - ("<< __PRETTY_FUNCTION__
#define LDEF_CMD_DISPTC_ERR_ LERR_ << "[DefaultCommandDispatcher] ("<< __PRETTY_FUNCTION__<<":"<<__LINE__ <<") -"

DEFINE_CLASS_FACTORY(DefaultCommandDispatcher, AbstractCommandDispatcher);

DefaultCommandDispatcher::DefaultCommandDispatcher(string alias):
AbstractCommandDispatcher(alias),
deinitialized(false){}

DefaultCommandDispatcher::~DefaultCommandDispatcher(){}

/*
 Initialization method for output buffer
 */
void DefaultCommandDispatcher::init(void *initConfiguration) throw(CException) {
    LDEF_CMD_DISPTC_APP_ << "Initializing Default Command Dispatcher";
    AbstractCommandDispatcher::init(initConfiguration);
    
    deinitialized = true;
    LDEF_CMD_DISPTC_APP_ << "Initilized Default Command Dispatcher";
}


/*
 Deinitialization method for output buffer
 */
void DefaultCommandDispatcher::deinit() throw(CException) {
    LDEF_CMD_DISPTC_APP_ << "Deinitializing Default Command Dispatcher";
    //we need to stop all das
    chaos::common::thread::ReadLock r_lock(das_map_mutex);
    map<string, ChaosSharedPtr<DomainActionsScheduler> >::iterator dasIter = das_map.begin();
    for (; dasIter != das_map.end(); dasIter++) {
        LDEF_CMD_DISPTC_APP_ << "Deinitializing action scheduler for domain:"<< (*dasIter).second->getManagedDomainName();
        //th einitialization is enclosed into try/catch because we need to
        //all well cleaned
        try{
            (*dasIter).second->deinit();
        }catch(CException& cse){
            DECODE_CHAOS_EXCEPTION(cse)
        }catch(...){
            LDEF_CMD_DISPTC_APP_ << "-----------Exception------------";
            LDEF_CMD_DISPTC_APP_ << "Unmanaged error";
            LDEF_CMD_DISPTC_APP_ << "-----------Exception------------";
        }
        LDEF_CMD_DISPTC_APP_ << "Deinitialized action scheduler for domain:";
    }
    das_map.clear();
    deinitialized = false;
    AbstractCommandDispatcher::deinit();
    LDEF_CMD_DISPTC_APP_ << "Deinitialized Default Command Dispatcher";
}
/*
 return an isntance of DomainActions pointer in relation to name
 but if the name is not present initialized it and add it to map
 */
ChaosSharedPtr<DomainActions> DefaultCommandDispatcher::getDomainActionsFromName(const string& domain_name) {
    //check if is not preset, so we can allocate it
    if(!action_domain_executor_map.count(domain_name)){
        ChaosSharedPtr<DomainActions>  result(new DomainActions(domain_name));
        if(result){;
            action_domain_executor_map.insert(make_pair(domain_name, result));
            DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Allocated new  DomainActions:" << domain_name;);
        }
    }
    //return the domain executor for name
    return action_domain_executor_map[domain_name];
}

/*
 return an isntance of DomainActions pointer and remove
 it form the map
 */
void DefaultCommandDispatcher::removeDomainActionsFromName(const string& domainName) {
    if(!action_domain_executor_map.count(domainName)){
        action_domain_executor_map.erase(domainName);
    }
}

/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void DefaultCommandDispatcher::registerAction(DeclareAction *declareActionClass)  throw(CException)  {
    if(!declareActionClass) return;
    
    //we need to allocate the scheduler for every registered domain that doesn't exist
    chaos::common::thread::UpgradeableLock ur_lock(das_map_mutex);
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        //get the domain executor for this action descriptor
        ChaosSharedPtr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
        //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->addActionDescriptor(*actDescIter);
            DEBUG_CODE(LDEF_CMD_DISPTC_DBG_	<< "Registered action [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName)
                       << "] for domain [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain) << "]";);
        }
    }
    
    actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        //get the domain executor for this action descriptor
        string domainName = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
        if(!das_map.count(domainName)){
            ChaosSharedPtr<DomainActionsScheduler> das(new DomainActionsScheduler(getDomainActionsFromName(domainName)));
            das->init(1);
            DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Initialized actions scheduler for domain:" << domainName;)
            chaos::common::thread::UpgradeReadToWriteLock uw_lock(ur_lock);
            //add the domain scheduler to map
            das_map.insert(make_pair(domainName, das));
            
            //register this dispatcher into Action Scheduler
            //to permit it to manage the subcommand push
            das->setDispatcher(this);
        }
    }
    
    //initialing the scheduler
    //das->init(1);
}

/*
 Deregister actions for a determianted domain
 */
void DefaultCommandDispatcher::deregisterAction(DeclareAction *declareActionClass)  throw(CException) {
    if(!declareActionClass) return;
    
    chaos::common::thread::WriteLock w_lock(das_map_mutex);
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        //get the domain executor for this action descriptor
        ChaosSharedPtr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
        //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->removeActionDescriptor(*actDescIter);
            DEBUG_CODE(LDEF_CMD_DISPTC_DBG_	<< "Deregistered action [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName)
                       << "] for domain [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain) << "]";);
            
            if(!domainExecutor->registeredActions()) {
                std::string domain_name = domainExecutor->getDomainName();
                DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "No more action in domain " << domain_name << " so it will be destroyed";);
                action_domain_executor_map.erase(domain_name);
            }
        }
    }
    
    
    //scan all cation to check if we need to remove the scheduler for an empty domain
    actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        
        //get the domain executor for this action descriptor
        string domain_name = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        //try to check if we need to remove the domain scheduler
        if(das_map.count(domain_name) &&
           !action_domain_executor_map.count(domain_name)) {
            ChaosSharedPtr<DomainActionsScheduler> domain_pointer = das_map[domain_name];
            LDEF_CMD_DISPTC_DBG_ << "The domain scheduler no more needed for "<< domain_name << " so it it's going to be removed";
            das_map.erase(domain_name);
            try{
                domain_pointer->deinit();
            } catch(CException& ex) {
                LDEF_CMD_DISPTC_ERR_<< "Error on " << domain_name << " DomainActionsScheduler deinit operation" << ex.what();
            }catch(...){
                LDEF_CMD_DISPTC_ERR_ << "Unmanaged error on " << domain_name << " DomainActionsScheduler deinit operation";
            }
        }
    }
    
}

CDWUniquePtr DefaultCommandDispatcher::executeCommandSync(CDWUniquePtr message_data) {
    //allocate new Result Pack
    CreateNewDataWrapper(result,);
    try{
        
        if(!message_data) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -1, "Invalid action pack", __PRETTY_FUNCTION__)
            return result;
        }
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN)){
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -2, "Action call with no action domain", __PRETTY_FUNCTION__)
            return result;
        }
        string action_domain = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -3, "Action Call with no action name", __PRETTY_FUNCTION__)
            return result;
        }
        string action_name = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
        
        
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!das_map.count(action_domain)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result,
                                            -4,
                                            "Action Domain \""+action_domain+"\" not registered (data pack \""+message_data->getJSONString()+"\")",
                                            __PRETTY_FUNCTION__)
            return result;
        }
        
        //submit the action(Thread Safe)
        das_map[action_domain]->synchronousCall(action_name,
                                                MOVE(message_data),
                                                result);
        
        //tag message has submitted
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    }catch(CException& ex){
        LDEF_CMD_DISPTC_ERR_<<ex;
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
    } catch(...){
        MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -5, "General exception received", __PRETTY_FUNCTION__)
    }
    return result;
}

/*
 This method sub the pack received by RPC system to the execution queue accordint to the pack domain
 the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
 will ever return an allocated object. The deallocaiton is demanded to caller
 */
CDWUniquePtr DefaultCommandDispatcher::dispatchCommand(CDWUniquePtr command_pack) {
    //allocate new Result Pack
    CreateNewDataWrapper(result_pack,);
    bool sent = false;
    try{
        if(!command_pack) return result_pack;
        if(!command_pack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN))
            throw CException(ErrorRpcCoce::EC_RPC_NO_DOMAIN_FOUND_IN_MESSAGE, "Action Call with no action domain", __PRETTY_FUNCTION__);
        
        if(!command_pack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))
            throw CException(ErrorRpcCoce::EC_RPC_NO_ACTION_FOUND_IN_MESSAGE, "Action Call with no action name", __PRETTY_FUNCTION__);
        string actionDomain = command_pack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(das_map.count(actionDomain) == 0) throw CException(ErrorRpcCoce::EC_RPC_NO_DOMAIN_REGISTERED_ON_SERVER, "Action Domain \""+actionDomain+"\" not registered (cmd pack \""+command_pack->getJSONString()+"\")", __PRETTY_FUNCTION__);
        
        //DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Received the message content:-----------------------START\n"<<command_pack->getJSONString() << "\nReceived the message content:-------------------------END";)
        
        //submit the action(Thread Safe)
        if(!(sent = das_map[actionDomain]->push(MOVE(command_pack)))) {
            throw CException(ErrorRpcCoce::EC_RPC_NO_MORE_SPACE_ON_DOMAIN_QUEUE, "No more space in queue", __PRETTY_FUNCTION__);
        }
        
        //tag message has submitted
        result_pack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ErrorCode::EC_NO_ERROR);
    }catch(CException& ex){
        LDEF_CMD_DISPTC_ERR_<<ex;
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result_pack, ex)
    } catch(...){
        //tag message has not submitted
        result_pack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ErrorRpcCoce::EC_RPC_UNMANAGED_ERROR_DURING_FORWARDING);
        //set error to general exception error
        result_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Unmanaged error");
    }
    return result_pack;
}

uint32_t DefaultCommandDispatcher::domainRPCActionQueued(const std::string& domain_name) {
    chaos::common::thread::UpgradeableLock ur_lock(das_map_mutex);
    //check if we are started
    if(getServiceState() != 2) return -1;
    //return the size of the action queue
    if(das_map.count(domain_name) == 0) return -1;
    return das_map[domain_name]->getQueuedActionSize();
}

bool DefaultCommandDispatcher::hasDomain(const std::string& domain_name) {
    chaos::common::thread::ReadLock r_lock(das_map_mutex);
    return das_map.count(domain_name) != 0;
}
