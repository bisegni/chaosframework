/*
 *	SharedCommandDispatcher.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 05/01/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/global.h>
#include <chaos/common/dispatcher/SharedCommandDispatcher.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::data;

DEFINE_CLASS_FACTORY(SharedCommandDispatcher, AbstractCommandDispatcher);

SharedCommandDispatcher::SharedCommandDispatcher(const string& alias):
AbstractCommandDispatcher(alias){}

SharedCommandDispatcher::~SharedCommandDispatcher(){}

/*
 Initialization method for output buffer
 */
void SharedCommandDispatcher::init(void *initConfiguration) throw(CException) {
    AbstractCommandDispatcher::init(initConfiguration);
    CObjectProcessingQueue<chaos_data::CDataWrapper>::init(GlobalConfiguration::getInstance()->getConfiguration()->getUInt32Value(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD));
}


/*
 Deinitialization method for output buffer
 */
void SharedCommandDispatcher::deinit() throw(CException) {
    MapDomainActionsLockedWriteLock wl = map_domain_actions.getWriteLockObject();
    
    map_domain_actions().clear();
    AbstractCommandDispatcher::deinit();
}


/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void SharedCommandDispatcher::registerAction(DeclareAction *declareActionClass)  throw(CException)  {
    if(!declareActionClass) return;
    //we need to allocate the scheduler for every registered domain that doesn't exist
    MapDomainActionsLockedWriteLock wl = map_domain_actions.getWriteLockObject();
    
    for (vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
         actDescIter != declareActionClass->getActionDescriptors().end();
         actDescIter++) {
        const std::string domain_name = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        if(map_domain_actions().count(domain_name) == 0) {
            map_domain_actions().insert(MapDomainActionsPair(domain_name, boost::shared_ptr<DomainActions>(new DomainActions(domain_name))));
        }
        map_domain_actions()[domain_name]->addActionDescriptor(*actDescIter);
    }
}

/*
 Deregister actions for a determianted domain
 */
void SharedCommandDispatcher::deregisterAction(DeclareAction *declareActionClass)  throw(CException) {
    if(!declareActionClass) return;
    //we need to allocate the scheduler for every registered domain that doesn't exist
    MapDomainActionsLockedWriteLock wl = map_domain_actions.getWriteLockObject();
    for (vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
         actDescIter != declareActionClass->getActionDescriptors().end();
         actDescIter++) {
        const std::string domain_name = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        if(map_domain_actions().count(domain_name) == 0) {
            continue;
        }
        map_domain_actions()[domain_name]->removeActionDescriptor(*actDescIter);
        if(map_domain_actions()[domain_name]->registeredActions() == 0) {
            INFO_LOG(SharedCommandDispatcher)<<"Remove action container for domain " << domain_name;
            map_domain_actions().erase(domain_name);
        }
    }
}


CDataWrapper* SharedCommandDispatcher::executeCommandSync(CDataWrapper * message_data) {
    MapDomainActionsLockedReadLock wl = map_domain_actions.getReadLockObject();
    
    //allocate new Result Pack
    bool message_has_been_detached = false;
    CDataWrapper *result = new CDataWrapper();
    try{
        
        if(!message_data) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -1, "Invalid action pack", __PRETTY_FUNCTION__)
            DELETE_OBJ_POINTER(message_data)
            return result;
        }
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN)){
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -2, "Action call with no action domain", __PRETTY_FUNCTION__)
            DELETE_OBJ_POINTER(message_data)
            return result;
        }
        string action_domain = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
        if(!message_data->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -3, "Action Call with no action name", __PRETTY_FUNCTION__)
            DELETE_OBJ_POINTER(message_data)
            return result;
        }
        string action_name = message_data->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
        unique_ptr<CDataWrapper>  action_message(message_data);
        
        
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!map_domain_actions().count(action_domain)) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -4, "Action Domain \""+action_domain+"\" not registered (data pack \""+message_data->getJSONString()+"\")", __PRETTY_FUNCTION__)
            DELETE_OBJ_POINTER(message_data)
            return result;
        }
        
        if(map_domain_actions()[action_domain]->hasActionName(action_name) == false) {
            MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -4, "Action Domain \""+action_domain+"\" not registered (data pack \""+message_data->getJSONString()+"\")", __PRETTY_FUNCTION__)
            DELETE_OBJ_POINTER(message_data)
            return result;
        }
        
        //submit the action(Thread Safe)
        AbstActionDescShrPtr action_desc_ptr = map_domain_actions()[action_domain]->getActionDescriptornFormActionName(action_name);
        std::unique_ptr<CDataWrapper> message_data(action_message->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        
        //lock the action for write, so we can schedule it
        ActionReadLock read_lock_for_action_execution(action_desc_ptr->actionAccessMutext);
        
        //set hte action as fired
        bool can_fire = action_desc_ptr->setFired(true);
        
        //if we can't fire we exit
        if(!can_fire) {
            result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, -2);
            result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, __PRETTY_FUNCTION__);
            result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Action can't be fired");
        } else {
            //call and return
            try {
                unique_ptr<CDataWrapper> action_result(action_desc_ptr->call(message_data.get(), message_has_been_detached));
                if(action_result.get() &&
                   action_message->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN) &&
                   action_message->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION)) {
                    result->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *action_result.get());
                }
            } catch (CException& ex) {
                INFO_LOG(SharedCommandDispatcher) << "Error during action execution";
                DECODE_CHAOS_EXCEPTION(ex)
                //set error in response is it's needed
                DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
            } catch(...){
                INFO_LOG(SharedCommandDispatcher) << "General error during action execution";
                result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, -3);
            }
        }
        if(message_has_been_detached) {
            message_data.release();
        }
        //set hte action as no fired
        action_desc_ptr->setFired(false);
        
        //tag message has submitted
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    }catch(CException& ex){
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
    } catch(...){
        MANAGE_ERROR_IN_CDATAWRAPPERPTR(result, -5, "General exception received", __PRETTY_FUNCTION__)
    }
    return result;
}

void SharedCommandDispatcher::processBufferElement(chaos_data::CDataWrapper *actionDescription,
                                                   ElementManagingPolicy &policy) throw(CException) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    CDataWrapper            *responsePack = NULL;
    CDataWrapper            *subCommand = NULL;
    unique_ptr<CDataWrapper>  actionMessage;
    unique_ptr<CDataWrapper>  remoteActionResult;
    unique_ptr<CDataWrapper>  actionResult;
    MapDomainActionsLockedReadLock wl = map_domain_actions.getReadLockObject();
    
    //keep track for the retain of the message of the aciton description
    ElementManagingPolicy               action_elementPolicy = {false};
    bool    needAnswer = false;
    //bool    detachParam = false;
    int     answer_id;
    string  answer_ip;
    string  answer_domain;
    string  answer_action;
    string  action_domain = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
    string  action_name = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
    
    if(!map_domain_actions().count(action_domain)) {
        ERR_LOG(SharedCommandDispatcher) << "The rpc domain " << action_domain << " is not present";
        return;
    }
    
    if(!map_domain_actions()[action_domain]->hasActionName(action_name)) {
        ERR_LOG(SharedCommandDispatcher) << "The action " << action_name << " is not present for rpc domain " << action_domain;
        return;
    }

    
    //get the action reference
    AbstActionDescShrPtr actionDescriptionPtr = map_domain_actions()[action_domain]->getActionDescriptornFormActionName(action_name);
    
    //lock the action for write, so we can schedule it
    ActionReadLock readLockForActionExecution(actionDescriptionPtr->actionAccessMutext);
    
    //set hte action as fired
    bool canFire = actionDescriptionPtr->setFired(true);
    
    //if we can't fire we exit
    if(!canFire) return;
    
    try {
        //get the action message
        if( actionDescription->hasKey( RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE ) ) {
            //there is a subcommand to submit
            actionMessage.reset(actionDescription->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        }
        
        //get sub command if present
        //check if we need to submit a sub command
        if( actionDescription->hasKey( RpcActionDefinitionKey::CS_CMDM_SUB_CMD ) ) {
            //there is a subcommand to submit
            subCommand = actionDescription->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_SUB_CMD);
        }
        
        //check if request has the rigth key to let chaos lib can manage the answer send operation
        if(actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID) &&
           actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP) ) {
            //get infor for answer form the request
            answer_id = actionDescription->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID);
            answer_ip = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP);
            
            //we must check this only if we have a destination ip to send the answer
            if(actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN) &&
               actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION) ) {
                //fill the action doma and name for the answer
                answer_domain = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN);
                answer_action = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION);
                
                //answer can be sent
                needAnswer = true;
            }
        }
        
        try{
            //call function core part
            if(needAnswer){
                //we need a response, so allocate the memory for it
                remoteActionResult.reset(new CDataWrapper());
            }
            //syncronously call the action in the current thread
            actionResult.reset(actionDescriptionPtr->call(actionMessage.get(), action_elementPolicy.elementHasBeenDetached));
            
            //check if we need to submit a sub command
            if( subCommand ) {
                //we can submit sub command
                unique_ptr<CDataWrapper> dispatchSubCommandResult(dispatchCommand(subCommand));
            }
            
            if(needAnswer){
                //we need an answer so add the submition result
                //if(actionResult.get()) remoteActionResult->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT, *actionResult.get());
                //put the submissione result error to 0(all is gone well)
                if(actionResult.get()) remoteActionResult->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *actionResult.get());
                
                remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
            }
        } catch (CException& ex) {
            LAPP_ << "Error during action execution:"<<ex.what();
            DECODE_CHAOS_EXCEPTION(ex)
            //set error in response is it's needed
            if(needAnswer && remoteActionResult.get()) {
                DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(remoteActionResult, ex)
            }
        } catch(...){
            LAPP_ << "General error during action execution";
            //set error in response is it's needed
            if(needAnswer) remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 1);
        }
        
        
        if( needAnswer && remoteActionResult.get() ) {
            //we need to construct the response pack
            responsePack = new CDataWrapper();
            
            //fill answer with data for remote ip and request id
            remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, answer_id);
            //set the answer host ip as remote ip where to send the answere
            responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, answer_ip);
            
            //check this only if we have a destinantion
            if(answer_domain.size() && answer_action.size()){
                //set the domain for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, answer_domain);
                
                //set the name of the action for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, answer_action);
            }
            
            //add the action message
            responsePack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *remoteActionResult.get());
            //in any case this result must be LOG
            //the result of the action action is sent using this thread
            if(!submitMessage(answer_ip, responsePack, false)){
                //the response has not been sent
                DELETE_OBJ_POINTER(responsePack);
            }
        }
    } catch (CException& ex) {
        DELETE_OBJ_POINTER(responsePack);
        //these exception need to be logged
        DECODE_CHAOS_EXCEPTION(ex);
    }
    
    //check if we need to detach the action message
    if(action_elementPolicy.elementHasBeenDetached){
        actionMessage.release();
    }
    
    //set hte action as no fired
    actionDescriptionPtr->setFired(false);
}

/*
 This method sub the pack received by RPC system to the execution queue accordint to the pack domain
 the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
 will ever return an allocated object. The deallocaiton is demanded to caller
 */
CDataWrapper *SharedCommandDispatcher::dispatchCommand(CDataWrapper *commandPack) throw(CException)  {
    MapDomainActionsLockedWriteLock wl = map_domain_actions.getWriteLockObject();
    
    //allocate new Result Pack
    CDataWrapper *resultPack = new CDataWrapper();
    bool sent = false;
    try{
        
        if(!commandPack) return resultPack;
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN))
            throw CException(ErrorRpcCoce::EC_RPC_NO_DOMAIN_FOUND_IN_MESSAGE, "Action Call with no action domain", __PRETTY_FUNCTION__);
        
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))
            throw CException(ErrorRpcCoce::EC_RPC_NO_ACTION_FOUND_IN_MESSAGE, "Action Call with no action name", __PRETTY_FUNCTION__);
        const string actionDomain = commandPack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        const string actionName = commandPack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
        //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(map_domain_actions().count(actionDomain) == 0) throw CException(ErrorRpcCoce::EC_RPC_NO_DOMAIN_REGISTERED_ON_SERVER, "Action Domain \""+actionDomain+"\" not registered (cmd pack \""+commandPack->getJSONString()+"\")", __PRETTY_FUNCTION__);
        
        if(map_domain_actions()[actionDomain]->hasActionName(actionName) == false) throw CException(ErrorRpcCoce::EC_RPC_NO_ACTION_FOUND_IN_MESSAGE, "Action \""+actionName+"\" not found (cmd pack \""+commandPack->getJSONString()+"\")", __PRETTY_FUNCTION__);

        DEBUG_CODE(DBG_LOG(SharedCommandDispatcher)  << "Received the message content:-----------------------START\n"<<commandPack->getJSONString() << "\nReceived the message content:-------------------------END";)
        
        //submit the action(Thread Safe)
        if(!(sent = CObjectProcessingQueue<chaos::common::data::CDataWrapper>::push(commandPack))) {
            throw CException(ErrorRpcCoce::EC_RPC_NO_MORE_SPACE_ON_DOMAIN_QUEUE, "No more space in queue", __PRETTY_FUNCTION__);
        }
        
        //tag message has submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ErrorCode::EC_NO_ERROR);
    }catch(CException& ex){
        if(!sent && commandPack) delete(commandPack);
        DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(resultPack, ex)
    } catch(...){
        if(!sent && commandPack) delete(commandPack);
        //tag message has not submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, ErrorRpcCoce::EC_RPC_UNMANAGED_ERROR_DURING_FORWARDING);
        //set error to general exception error
        resultPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Unmanaged error");
    }
    DEBUG_CODE(DBG_LOG(SharedCommandDispatcher) << "Send the message ack:-----------------------START";)
    DEBUG_CODE(DBG_LOG(SharedCommandDispatcher) << resultPack->getJSONString();)
    DEBUG_CODE(DBG_LOG(SharedCommandDispatcher) << "Send the message ack:-------------------------END";)
    return resultPack;
}

uint32_t SharedCommandDispatcher::domainRPCActionQueued(const std::string& domain_name) {
    return 0;
}

bool SharedCommandDispatcher::hasDomain(const std::string& domain_name) {
    MapDomainActionsLockedReadLock wl = map_domain_actions.getReadLockObject();
    return map_domain_actions().count(domain_name) != 0;
}
