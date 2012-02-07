    //
    //  DomainActionsScheduler.cpp
    //  ChaosFramework
    //
    //  Created by bisegni on 08/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "../global.h"
#include "DomainActionsScheduler.h"
#include "../cconstants.h"

using namespace chaos;

DomainActionsScheduler::DomainActionsScheduler(shared_ptr<DomainActions> _domainActionsContainer):armed(false){
    domainActionsContainer = _domainActionsContainer;
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
    if(!armed) throw CException(0, "Action can't be submitted, scheduler is not armed", "DomainActionsScheduler::push");
    return CObjectProcessingQueue<CDataWrapper>::push(actionParam);
}

/*
 
 */
string& DomainActionsScheduler::getManagedDomainName() {
    return domainActionsContainer->getDomainName();
}

/*
 
 */
void DomainActionsScheduler::setDispatcher(CommandDispatcher *newDispatcher) {
    dispatcher = newDispatcher;
}

/*
 process the element action to be executed
 */
void DomainActionsScheduler::processBufferElement(CDataWrapper *actionDescription, ElementManagingPolicy& elementPolicy) throw(CException) {
        //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    CDataWrapper            *responsePack = NULL;
    CDataWrapper            *subCommand = NULL;
    auto_ptr<CDataWrapper>  actionMessage;
    auto_ptr<CDataWrapper>  remoteActionResult;
    auto_ptr<CDataWrapper>  actionResult;
    
    bool    needAnswer = false;
    bool    detachParam = false;
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
            needAnswer = true;
                //get infor for answer form the request
            answerID = actionDescription->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID); 
            answerIP = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP); 
            
                //we must check this only if we have a destination ip to send the answer
            if(actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN) &&
               actionDescription->hasKey(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION) ) {
                    //fill the action doma and name for the answer
                answerDomain = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN); 
                answerAction = actionDescription->getStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION); 
            }
        }
        
        try{
                //call function core part
            if(needAnswer){
                    //we need a response, so allocate the memory for it
                remoteActionResult.reset(new CDataWrapper());
            }
                //syncronously call the action in the current thread 
            actionResult.reset(actionDescriptionPtr->call(actionMessage.get(), detachParam));
            
                //check if we need to submit a sub command
            if( subCommand ) {
                    //we can submit sub command
                auto_ptr<CDataWrapper> dispatchSubCommandResult(dispatcher->dispatchCommand(subCommand));
            }
            
            if(needAnswer){
                    //we need an answer so add the submition result
                if(actionResult.get()) remoteActionResult->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT, *actionResult.get());
                    //put the submissione result error to 0(all i sgone well)
                remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
            }
        } catch (CException& ex) {
            LAPP_ << "Error during action execution";
            DECODE_CHAOS_EXCEPTION(ex)
                //set error in response is it's needed
            if(needAnswer) DECODE_CHAOS_EXCEPTION_IN_CDATAWRAPPERPTR(remoteActionResult, ex)
                } catch(...){
                    LAPP_ << "General error during action execution";
                        //set error in response is it's needed
                    if(needAnswer) remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 1);
                }
        
        
        if( needAnswer && remoteActionResult.get() ) {
                //we need to construct the response pack
            responsePack = new CDataWrapper();
            
                //fill answer with data for remote ip and request id
            remoteActionResult->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, answerID);
                //set the answer host ip as remote ip where to send the answere
            responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, answerIP);
            
                //check this only if we have a destinantion
            if(answerDomain.size() && answerAction.size()){
                    //set the domain for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, answerDomain);
                
                    //set the name of the action for the answer
                responsePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, answerAction);
            }
            
                //add the action message
            responsePack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *remoteActionResult.get());
                //in any case this result must be LOG
                //the result of the action action is sent using this thread
            if(!dispatcher->sendActionResult(responsePack, false)){
                    //the response has not been sent
                DELETE_OBJ_POINTER(responsePack);
            }
        }
    } catch (CException& ex) {
        CHK_AND_DELETE_OBJ_POINTER(responsePack);
            //these exception need to be logged
        DECODE_CHAOS_EXCEPTION(ex);
    }
        //check if we need to detach the action message
    if(detachParam){
        actionMessage.release();
    }
    
        //set hte action as no fired
    actionDescriptionPtr->setFired(false);
    
} 