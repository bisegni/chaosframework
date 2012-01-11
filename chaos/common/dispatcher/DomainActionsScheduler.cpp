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
void DomainActionsScheduler::processBufferElement(CDataWrapper *actionDescription) throw(CException) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    CDataWrapper *actionResult = NULL;
    string actionName = actionDescription->getStringValue( CommandManagerConstant::CS_CMDM_ACTION_NAME );
    
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
            //syncronously call the action in the current thread 
        actionResult = actionDescriptionPtr->call(actionDescription);
 
            //check if we need to submit a sub command
        if( actionDescription->hasKey( CommandManagerConstant::CS_CMDM_SUB_CMD ) ) {
                //there is a subcommand to submit
            CDataWrapper *subCommand = actionDescription->getCSDataValue(CommandManagerConstant::CS_CMDM_SUB_CMD);
            CHAOS_ASSERT(subCommand);
                //dipatch the subcommand
            auto_ptr<CDataWrapper> dispatchSubCommandResult(dispatcher->dispatchCommand(subCommand));
        }
        
        if( actionResult ) {
                //in any case this result must be LOG
                //the result of the action action is sent using this thread
            if(!dispatcher->sendActionResult(actionDescription, actionResult, false)){
                    //the response has not been sent
                DELETE_OBJ_POINTER(actionResult);
            }
        }
    } catch (CException& ex) {
        CHK_AND_DELETE_OBJ_POINTER(actionResult);
            //these exception need to be logged
        DECODE_CHAOS_EXCEPTION(ex);
    }

    //set hte action as no fired
    actionDescriptionPtr->setFired(false);

} 