/*
 *	DefaultCommandDispatcher.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include "../global.h"
#include <chaos/common/chaos_constants.h>
#include <chaos/common/dispatcher/DefaultCommandDispatcher.h>

using namespace chaos;
using namespace chaos::common::data;
//namespace chaos_data = chaos::common::data;

using namespace std;
using namespace boost;

#define LDEF_CMD_DISPTC_APP_ LAPP_ << "[DefaultCommandDispatcher] - "
#define LDEF_CMD_DISPTC_DBG_ LDBG_ << "[DefaultCommandDispatcher] - "
#define LDEF_CMD_DISPTC_ERR_ LERR_ << "[DefaultCommandDispatcher] ("<< __LINE__ <<") -"

DEFINE_CLASS_FACTORY(DefaultCommandDispatcher, AbstractCommandDispatcher);

DefaultCommandDispatcher::DefaultCommandDispatcher(string alias) : AbstractCommandDispatcher(alias) {
}

DefaultCommandDispatcher::~DefaultCommandDispatcher(){
    
}

/*
 Initialization method for output buffer
 */
void DefaultCommandDispatcher::init(CDataWrapper *initConfiguration) throw(CException) {
    LDEF_CMD_DISPTC_APP_ << "Initializing Default Command Dispatcher";
    AbstractCommandDispatcher::init(initConfiguration);
    
    deinitialized = true;
    LDEF_CMD_DISPTC_APP_ << "Initilized Default Command Dispatcher";
}


/*
 Deinitialization method for output buffer
 */
void DefaultCommandDispatcher::deinit() throw(CException) {
    LDEF_CMD_DISPTC_APP_ << "Deinitilizing Default Command Dispatcher";
    //we need to stop all das
    chaos::common::thread::ReadLock r_lock(das_map_mutex);
    map<string, boost::shared_ptr<DomainActionsScheduler> >::iterator dasIter = das_map.begin();
    for (; dasIter != das_map.end(); dasIter++) {
        LDEF_CMD_DISPTC_APP_ << "Deinitilizing action scheduler for domain:"<< (*dasIter).second->getManagedDomainName();
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
    LDEF_CMD_DISPTC_APP_ << "Deinitilized Default Command Dispatcher";
}


/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void DefaultCommandDispatcher::registerAction(DeclareAction *declareActionClass)  throw(CException)  {
    if(!declareActionClass) return;
    
    //register the action
    AbstractCommandDispatcher::registerAction(declareActionClass);
    
    //we need to allocate the scheduler for every registered domain that doesn't exist
    chaos::common::thread::UpgradeableLock ur_lock(das_map_mutex);
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        //get the domain executor for this action descriptor
        string domainName = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
        if(!das_map.count(domainName)){
            boost::shared_ptr<DomainActionsScheduler> das(new DomainActionsScheduler(getDomainActionsFromName(domainName)));
#if DEBUG
            LDEF_CMD_DISPTC_DBG_ << "Allocated new  actions scheduler for domain:" << domainName;
            LDEF_CMD_DISPTC_DBG_ << "Init actions scheduler for domain:" << domainName;
            LDEF_CMD_DISPTC_DBG_ << "WE MUST THING ABOUT GET GLOBAL CONF FOR INIT DomainActionsScheduler object";
#endif
            das->init(1);
#if DEBUG
            LDEF_CMD_DISPTC_DBG_ << "Initialized actions scheduler for domain:" << domainName;
#endif
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
    
    //call superclass method
    AbstractCommandDispatcher::deregisterAction(declareActionClass);
    
    chaos::common::thread::WriteLock w_lock(das_map_mutex);
    
    //scan all cation to check if we need to remove the scheduler for an empty domain
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        
        //get the domain executor for this action descriptor
        string domain_name = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        //try to check if we need to remove the domain scheduler
        if(das_map.count(domain_name) &&
           !actionDomainExecutorMap.count(domain_name)) {
            boost::shared_ptr<DomainActionsScheduler> domain_pointer = das_map[domain_name];
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

CDataWrapper* DefaultCommandDispatcher::executeCommandSync(CDataWrapper * message_data) {
    //allocate new Result Pack
    CDataWrapper *result = new CDataWrapper();
    try{
        
        if(!message_data) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -1, "Invalid action pack", __PRETTY_FUNCTION__)
			CHK_AND_DELETE_OBJ_POINTER(message_data)
            return result;
        }
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN)){
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -2, "Action call with no action domain", __PRETTY_FUNCTION__)
			CHK_AND_DELETE_OBJ_POINTER(message_data)
			return result;
        }
        string action_domain = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -3, "Action Call with no action name", __PRETTY_FUNCTION__)
            CHK_AND_DELETE_OBJ_POINTER(message_data)
			return result;
        }
        string action_name = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
        
        
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!das_map.count(action_domain)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -4, "Action Domain not registered", __PRETTY_FUNCTION__)
            CHK_AND_DELETE_OBJ_POINTER(message_data)
			return result;
        }
        
        //submit the action(Thread Safe)
        das_map[action_domain]->synchronousCall(action_name,
                                                message_data,
                                                result);
        
        //tag message has submitted
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    }catch(CException& ex){
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
    } catch(...){
        MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -5, "General exception received", __PRETTY_FUNCTION__)
    }
    return result;
}

CDataWrapper* DefaultCommandDispatcher::executeCommandSync(const std::string& domain,
                                                           const std::string& action,
                                                           chaos_data::CDataWrapper * message_data) {
    CDataWrapper *result = new CDataWrapper();
    try{
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!das_map.count(domain)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -1, "Action Domain not registered", __PRETTY_FUNCTION__)
			CHK_AND_DELETE_OBJ_POINTER(message_data)
            return result;
        }
        
        //submit the action(Thread Safe)
        das_map[domain]->synchronousCall(action,
                                         message_data,
                                         result);
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    }catch(CException& ex){
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
    } catch(...){
        MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -2, "General exception received", __PRETTY_FUNCTION__)
    }
    return result;
}

/*
 This method sub the pack received by RPC system to the execution queue accordint to the pack domain
 the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
 will ever return an allocated object. The deallocaiton is demanded to caller
 */
CDataWrapper *DefaultCommandDispatcher::dispatchCommand(CDataWrapper *commandPack) throw(CException)  {
    //allocate new Result Pack
    CDataWrapper *resultPack = new CDataWrapper();
    bool sent = false;
    try{
        
        if(!commandPack) return resultPack;
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN))
            throw CException(0, "Action Call with no action domain", __PRETTY_FUNCTION__);
        
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))
            throw CException(1, "Action Call with no action name", __PRETTY_FUNCTION__);
        
        string actionDomain = commandPack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!das_map.count(actionDomain)) throw CException(3, "Action Domain not registered", __PRETTY_FUNCTION__);
        
        DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Received the message content:-----------------------START";)
        DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << commandPack->getJSONString();)
        DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Received the message content:-------------------------END";)
        
        //submit the action(Thread Safe)
        if(!(sent = das_map[actionDomain]->push(commandPack))) {
            throw CException(1, "No more space in queue", __PRETTY_FUNCTION__);
        }
        
        //tag message has submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    }catch(CException& ex){
        if(!sent && commandPack) delete(commandPack);
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(resultPack, ex)
    } catch(...){
        if(!sent && commandPack) delete(commandPack);
        //tag message has not submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 1);
        //set error to general exception error
        resultPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Unmanaged error");
    }
    DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Send the message ack:-----------------------START";)
    DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << resultPack->getJSONString();)
    DEBUG_CODE(LDEF_CMD_DISPTC_DBG_ << "Send the message ack:-------------------------END";)
    return resultPack;
}
