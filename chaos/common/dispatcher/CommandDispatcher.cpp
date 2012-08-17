/*	
 *	CommandDispatcher.cpp
 *	!CHOAS
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
void CommandDispatcher::registerAction(DeclareAction* declareActionClass)  throw(CException) {
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
void CommandDispatcher::deregisterAction(DeclareAction* declareActionClass)  throw(CException) {
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
bool CommandDispatcher::submitMessage(string& serverAndPort,  CDataWrapper* messageToSend, bool onThisThread)  throw(CException) {
    CHAOS_ASSERT(messageToSend && rpcClientPtr)
    if(!messageToSend && serverAndPort.size()) return false;
    
        //messageToSend->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, serverAndPort);
    
    return rpcClientPtr->submitMessage(serverAndPort, messageToSend, onThisThread);
}

/*
 Update the configuration for the dispatcher
 */
CDataWrapper* CommandDispatcher::updateConfiguration(CDataWrapper*)  throw(CException) {
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
void CommandDispatcher::setRpcForwarder(RpcMessageForwarder *rpcClPtr){
    rpcClientPtr = rpcClPtr;
}
