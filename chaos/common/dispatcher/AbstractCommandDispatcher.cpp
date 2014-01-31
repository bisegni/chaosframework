/*	
 *	AbstractCommandDispatcher.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>

using namespace chaos;
using namespace boost;
using namespace std;


AbstractCommandDispatcher::AbstractCommandDispatcher(string alias):NamedService(alias){
}

AbstractCommandDispatcher::~AbstractCommandDispatcher() {
    
}

/*
 init the rpc adapter
 */
void AbstractCommandDispatcher::init(void *initConfiguration) throw(CException) {
}

    //-----------------------
void AbstractCommandDispatcher::start() throw(CException) {
    
}

/*
 deinit the rpc adapter
 */
void AbstractCommandDispatcher::deinit() throw(CException) {
    
}

/*
 return an isntance of DomainActions pointer in relation to name
 but if the name is not present initialized it and add it to map
 */
boost::shared_ptr<DomainActions> AbstractCommandDispatcher::getDomainActionsFromName(string& domainName) {
        //check if is not preset, so we can allocate it
    if(!actionDomainExecutorMap.count(domainName)){
        boost::shared_ptr<DomainActions>  result(new DomainActions(domainName));
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
void AbstractCommandDispatcher::removeDomainActionsFromName(string& domainName) {
    if(!actionDomainExecutorMap.count(domainName)){
        actionDomainExecutorMap.erase(domainName);
    }
}

/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void AbstractCommandDispatcher::registerAction(DeclareAction* declareActionClass)  throw(CException) {
    if(!declareActionClass) return;
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        
        LAPP_   << "Registering action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
        << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
            //get the domain executor for this action descriptor
        boost::shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
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
void AbstractCommandDispatcher::deregisterAction(DeclareAction* declareActionClass)  throw(CException) {
    if(!declareActionClass) return;
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
        LAPP_   << "Deregistering action " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName) 
        << " for domain " << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
            //get the domain executor for this action descriptor
        boost::shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
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
bool AbstractCommandDispatcher::submitMessage(string& serverAndPort,  chaos::common::data::CDataWrapper* messageToSend, bool onThisThread)  throw(CException) {
    CHAOS_ASSERT(messageToSend && rpcForwarderPtr)
    if(!messageToSend && serverAndPort.size()) return false;
    NetworkForwardInfo *nfi = new NetworkForwardInfo();
    nfi->destinationAddr = serverAndPort;
    nfi->message = messageToSend;
    return rpcForwarderPtr->submitMessage(nfi, onThisThread);
}

/*
 Update the configuration for the dispatcher
 */
chaos::common::data::CDataWrapper* AbstractCommandDispatcher::updateConfiguration(chaos::common::data::CDataWrapper*)  throw(CException) {
    return NULL;
}

/*
 
 */
void AbstractCommandDispatcher::setRpcForwarder(RpcMessageForwarder *newRpcForwarderPtr){
    rpcForwarderPtr = newRpcForwarderPtr;
}
