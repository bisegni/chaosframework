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
#include "../global.h"
#include "DomainActionsScheduler.h"
#include <chaos/common/chaos_constants.h>

using namespace chaos;
using namespace chaos::common::data;

DomainActionsScheduler::DomainActionsScheduler(ChaosSharedPtr<DomainActions> _domainActionsContainer):
armed(false),
dispatcher(NULL){
    domainActionsContainer = _domainActionsContainer;
}

/*!
 Default destructor
 */
DomainActionsScheduler::~DomainActionsScheduler() {
    
}
/*
 Initialization method for output buffer
 */
void DomainActionsScheduler::init(int threadNumber) throw(CException) {
    LAPP_ << "Initializing Domain Actions Scheduler for domain:" << domainActionsContainer->getDomainName();
    CObjectProcessingQueue<CDataWrapper>::init(threadNumber);
    armed = true;
}

/*
 Deinitialization method for output buffer
 */
void DomainActionsScheduler::deinit() throw(CException) {
    LAPP_ << "Deinitializing Domain Actions Scheduler for domain:" << domainActionsContainer->getDomainName();
    //mutex::scoped_lock lockAction(actionAccessMutext);
    CObjectProcessingQueue<CDataWrapper>::clear();
    CObjectProcessingQueue<CDataWrapper>::deinit();
    armed = false;
}

/*
 override the push method for ObjectProcessingQueue<CDataWrapper> superclass
 */
bool DomainActionsScheduler::push(CDataWrapper *actionParam) throw(CException) {
    if(!armed) throw CException(-1, "Action can't be submitted, scheduler is not armed", "DomainActionsScheduler::push");
    if(!domainActionsContainer->hasActionName(actionParam->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))) throw CException(-2, "The action requested is not present in the domain", __PRETTY_FUNCTION__);
    return CObjectProcessingQueue<CDataWrapper>::push(actionParam);
}

/*
 
 */
const string& DomainActionsScheduler::getManagedDomainName() {
    return domainActionsContainer->getDomainName();
}

/*
 
 */
void DomainActionsScheduler::setDispatcher(AbstractCommandDispatcher *newDispatcher) {
    dispatcher = newDispatcher;
}

uint32_t DomainActionsScheduler::getQueuedActionSize() {
    return CObjectProcessingQueue<CDataWrapper>::queueSize();
}

void DomainActionsScheduler::synchronousCall(const std::string& action,
                                             chaos_data::CDataWrapper *message,
                                             chaos_data::CDataWrapper *result) {
    bool message_has_been_detached = false;
    ChaosUniquePtr<chaos::common::data::CDataWrapper>  action_message(message);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message_data(message->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
    if(!domainActionsContainer->hasActionName(action)) {
        LAPP_ << "The action " << action << " is not present for domain " << domainActionsContainer->getDomainName();
        result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, -1);
        result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, __PRETTY_FUNCTION__);
        result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Action is nto present in the domain");
        return;
    }
    //get the action reference
    AbstActionDescShrPtr action_desc_ptr = domainActionsContainer->getActionDescriptornFormActionName(action);
    
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
            ChaosUniquePtr<chaos::common::data::CDataWrapper> action_result(action_desc_ptr->call(message_data.get(), message_has_been_detached));
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

/*
 process the element action to be executed
 */
void DomainActionsScheduler::processBufferElement(CDataWrapper *actionDescription, ElementManagingPolicy& elementPolicy) throw(CException) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    CDataWrapper            *responsePack = NULL;
    CDataWrapper            *subCommand = NULL;
    ChaosUniquePtr<chaos::common::data::CDataWrapper>  actionMessage;
    ChaosUniquePtr<chaos::common::data::CDataWrapper>  remoteActionResult;
    ChaosUniquePtr<chaos::common::data::CDataWrapper>  actionResult;
    //keep track for the retain of the message of the aciton description
    ElementManagingPolicy               action_elementPolicy = {false};
    bool    needAnswer = false;
    //bool    detachParam = false;
    int     answerID;
    string  answerIP;
    string  answerDomain;
    string  answerAction;
    string  actionName = actionDescription->getStringValue( RpcActionDefinitionKey::CS_CMDM_ACTION_NAME );
    
    if(!domainActionsContainer->hasActionName(actionName)) {
        LAPP_ << "The action " << actionName << " is not present for domain " << domainActionsContainer->getDomainName();
        return;
    }
    //get the action reference
    AbstActionDescShrPtr actionDescriptionPtr = domainActionsContainer->getActionDescriptornFormActionName(actionName);
    
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
            actionMessage.reset(actionDescription->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE).release());
        }
        
        //get sub command if present
        //check if we need to submit a sub command
        if( actionDescription->hasKey( RpcActionDefinitionKey::CS_CMDM_SUB_CMD ) ) {
            //there is a subcommand to submit

            subCommand = actionDescription->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_SUB_CMD).release();
        }
        
        //check if request has the rigth key to let chaos lib can manage the answer send operation
        if(actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID) &&
           actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP) ) {
            //get infor for answer form the request
            answerID = actionDescription->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID);
            answerIP = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP);
            
            //we must check this only if we have a destination ip to send the answer
            if(actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN) &&
               actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION) ) {
                //fill the action doma and name for the answer
                answerDomain = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN);
                answerAction = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION);
                
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
            //synCronusly call the action in the current thread
            actionResult.reset(actionDescriptionPtr->call(actionMessage.get(), action_elementPolicy.elementHasBeenDetached));
            
            //check if we need to submit a sub command
            if( subCommand ) {
                //we can submit sub command
                ChaosUniquePtr<chaos::common::data::CDataWrapper> dispatchSubCommandResult(dispatcher->dispatchCommand(subCommand));
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
            CDWUniquePtr response_pack(new CDataWrapper());
            
            //fill answer with data for remote ip and request id
            remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, answerID);
            //set the answer host ip as remote ip where to send the answere
            response_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, answerIP);
            
            //check this only if we have a destinantion
            if(answerDomain.size() && answerAction.size()){
                //set the domain for the answer
                response_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, answerDomain);
                
                //set the name of the action for the answer
                response_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, answerAction);
            }
            
            //add the action message
            response_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *remoteActionResult.get());
            //in any case this result must be LOG
            //the result of the action action is sent using this thread
            if(!dispatcher->submitMessage(answerIP,
                                          ChaosMoveOperator(response_pack),
                                          false)){
                //the response has not been sent
                
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
