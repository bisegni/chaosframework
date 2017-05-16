/*
 *	SharedActionScheduler.cpp
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

#include <chaos/common/dispatcher/SharedActionScheduler.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;

/*!
 Default constructor
 */
SharedActionScheduler::SharedActionScheduler() {
    
}

/*!
 Default constructor
 */
SharedActionScheduler::~SharedActionScheduler() {
    
}

void SharedActionScheduler::addActionDomain(boost::shared_ptr<DomainActions> new_action_domain) {
    MapDomainActionsLockedWriteLock wr = map_domain_actions.getWriteLockObject();
    const std::string& domain_name = new_action_domain->getDomainName();
    map_domain_actions().insert(MapDomainActionsPair(domain_name, new_action_domain));
}

void SharedActionScheduler::removeActionDomain(boost::shared_ptr<DomainActions> new_action_domain) {
    MapDomainActionsLockedWriteLock wr = map_domain_actions.getWriteLockObject();
    const std::string& domain_name = new_action_domain->getDomainName();
    map_domain_actions().erase(domain_name);
    
}

void SharedActionScheduler::init(int) throw(CException) {
    CObjectProcessingQueue<chaos_data::CDataWrapper>::init(GlobalConfiguration::getInstance()->getConfiguration()->getUInt32Value(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD));
}

void SharedActionScheduler::deinit() throw(CException) {
    CObjectProcessingQueue<CDataWrapper>::clear();
    CObjectProcessingQueue<chaos_data::CDataWrapper>::deinit();
}

bool SharedActionScheduler::push(chaos_data::CDataWrapper *action_submission_pack) throw(CException) {
    if(!map_domain_actions().count(action_submission_pack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))) throw CException(-2, "The action requested is not present in the domain", __PRETTY_FUNCTION__);
    return CObjectProcessingQueue<CDataWrapper>::push(action_submission_pack);
}

void SharedActionScheduler::synchronousCall(chaos_data::CDataWrapper *message,
                                            chaos_data::CDataWrapper *result) {
    MapDomainActionsLockedReadLock wr = map_domain_actions.getReadLockObject();
    bool message_has_been_detached = false;
    unique_ptr<CDataWrapper>  action_message(message);
    
    const std::string domain_name = message->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
    const std::string action_name = message->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
    std::auto_ptr<CDataWrapper> message_data(message->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
    
    boost::shared_ptr<DomainActions> domain_action = map_domain_actions()[domain_name];
    if(domain_action.get() == NULL ||
       !domain_action->hasActionName(action_name)) {
        LAPP_ << "The action " << action_name << " is not present for domain " << domain_name;
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, -1);
        result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, __PRETTY_FUNCTION__);
        result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Action is nto present in the domain");
        return;
    }
    //get the action reference
    AbstActionDescShrPtr action_desc_ptr = domain_action->getActionDescriptornFormActionName(action_name);
    
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
            LAPP_ << "Error during action execution";
            DECODE_CHAOS_EXCEPTION(ex)
            //set error in response is it's needed
            DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(result, ex)
        } catch(...){
            LAPP_ << "General error during action execution";
            result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, -3);
        }
    }
    if(message_has_been_detached) {
        message_data.release();
    }
    //set hte action as no fired
    action_desc_ptr->setFired(false);
    
    //return the result
    return;
    
}

uint32_t SharedActionScheduler::getQueuedActionSize() {
    return 0;
}

/*
 process the element action to be executed
 */
void SharedActionScheduler::processBufferElement(CDataWrapper *actionDescription,
                                                 ElementManagingPolicy& elementPolicy) throw(CException) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    CDataWrapper            *responsePack = NULL;
    CDataWrapper            *subCommand = NULL;
    unique_ptr<CDataWrapper>  actionMessage;
    unique_ptr<CDataWrapper>  remoteActionResult;
    unique_ptr<CDataWrapper>  actionResult;
    //keep track for the retain of the message of the aciton description
    ElementManagingPolicy               action_elementPolicy = {false};
    bool    needAnswer = false;
    //bool    detachParam = false;
    int     answer_id;
    string  answer_ip;
    string  answer_domain;
    string  answer_action;
    string  domain_name = actionDescription->getStringValue( RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
    string  action_name = actionDescription->getStringValue( RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
    
    boost::shared_ptr<DomainActions> domain_action = map_domain_actions()[domain_name];
    //if(!domain_action->hasActionName(action_name)) {
    if(domain_action.get() == NULL ||
       !domain_action->hasActionName(action_name)) {
        LAPP_ << "The action " << action_name << " is not present for domain " << domain_name;
        return;
    }
    //get the action reference
    AbstActionDescShrPtr actionDescriptionPtr = domain_action->getActionDescriptornFormActionName(action_name);
    
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
        
        try {
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
                unique_ptr<CDataWrapper> dispatchSubCommandResult(dispatcher->dispatchCommand(subCommand));
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
            if(answer_domain.size() &&
               answer_action.size()){
                //set the domain for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, answer_domain);
                
                //set the name of the action for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, answer_action);
            }
            
            //add the action message
            responsePack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *remoteActionResult.get());
            //in any case this result must be LOG
            //the result of the action action is sent using this thread
            if(!dispatcher->submitMessage(answer_ip, responsePack, false)){
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
