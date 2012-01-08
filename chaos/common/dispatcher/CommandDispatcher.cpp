    //
    //  CommandDispatcher.cpp
    //  ChaosFramework
    //
    //  Created by bisegni on 08/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "../global.h"
#include "CommandDispatcher.h"
using namespace chaos;
using namespace std;

CommandDispatcher::CommandDispatcher(string *alias){
    typeName = alias;
}

/*
 init the rpc adapter
 */
void CommandDispatcher::init(CDataWrapper *initConfiguration) throw(CException) {
}


/*
 deinit the rpc adapter
 */
void CommandDispatcher::deinit() throw(CException) {
    
}

/*
 return an isntance of DomainActions pointer in relation to name
 but if the name is not present initialized it and add it to map
 */
shared_ptr<DomainActions> CommandDispatcher::getDomainActionsFromName(string& domainName) {
        //check if is not preset, so we can allocate it
    if(!actionDomainExecutorMap.count(domainName)){
        shared_ptr<DomainActions>  result(new DomainActions(domainName));
        if(result){;
            actionDomainExecutorMap.insert(make_pair(domainName, result));
#if DEBUG
            LDBG_ << "Allocated new  DomainActions:" << domainName;
#endif
        }
    }
        //return the domain executor for name
    return actionDomainExecutorMap[domainName];
}

/*
 return an isntance of DomainActions pointer and remove
 it form the map
 */
void CommandDispatcher::removeDomainActionsFromName(string& domainName) {
    if(!actionDomainExecutorMap.count(domainName)){
        actionDomainExecutorMap.erase(domainName);
    }
}

/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void CommandDispatcher::registerAction(DeclareAction* declareActionClass) {
    if(!declareActionClass) return;
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        
        LAPP_   << "Registering action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
        << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
            //get the domain executor for this action descriptor
        shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
            //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->addActionDescriptor(*actDescIter);
            LAPP_ << "Registered action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
            << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        }
    }
}

/*
 Deregister actions for a determianted domain
 */
void CommandDispatcher::deregisterAction(DeclareAction* declareActionClass) {
    if(!declareActionClass) return;
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        LAPP_   << "Deregistering action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
        << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
            //get the domain executor for this action descriptor
        shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
            //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->removeActionDescriptor(*actDescIter);
            LAPP_   << "Deregistered action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
            << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
            
        }
    }
}

/*
 Send a message
 */
bool CommandDispatcher::sendMessage(CDataWrapper* messageToSend, string& serverAndPort,  bool onThisThread) {
    CHAOS_ASSERT(messageToSend && rpcClientPtr)
    if(!messageToSend && serverAndPort.size()) return false;
    
    messageToSend->addStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP, serverAndPort);
    
    return rpcClientPtr->submitMessage(messageToSend, onThisThread);
}

/*
 Submit the action answer to rpc system
 */
bool  CommandDispatcher::sendActionResult(CDataWrapper *actionRequest, CDataWrapper *actionResult, bool onThisThread) {
    CHAOS_ASSERT(actionRequest && actionResult)
        //check if request has the rigth key to let chaos lib can manage the answer send operation
    if(!actionRequest->hasKey(CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID) ||
       !actionRequest->hasKey(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP) ) return false;
    
        //get infor for answer form the request
    string responseID = actionRequest->getStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID); 
    string remoteIP = actionRequest->getStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP); 
    
        //fill answer with data for remote ip and request id
    actionResult->addStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID, responseID);
    actionResult->addStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP, remoteIP);
    
    return rpcClientPtr->submitMessage(actionResult, onThisThread);
}

/*
 Update the configuration for the dispatcher
 */
CDataWrapper* CommandDispatcher::updateConfiguration(CDataWrapper*) {
    return NULL;
}

/*
 Return the adapter alias
 */
const char * CommandDispatcher::getName() const {
    return typeName->c_str();
}

/*
 
 */
void CommandDispatcher::setRpcClient(shared_ptr<RpcClient> rpcClPtr){
    rpcClientPtr = rpcClPtr;
}
