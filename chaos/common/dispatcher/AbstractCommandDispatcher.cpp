/*
 *	AbstractCommandDispatcher.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace boost;
using namespace std;

#define ACDLAPP_ LAPP_ << "[AbstractCommandDispatcher] - "
#define ACDLDBG_ LDBG_ << "[AbstractCommandDispatcher] - "
#define ACDLERR_ LERR_ << "[AbstractCommandDispatcher] - "

//test echo action
EchoRpcAction::EchoRpcAction() {
    //register the action for the response
    DeclareAction::addActionDescritionInstance<EchoRpcAction>(this,
                                                              &EchoRpcAction::echoAction,
                                                              NodeDomainAndActionRPC::RPC_DOMAIN,
                                                              NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                                              "Echo rpc action");
}
//!echo function return the data sent as parameter
CDataWrapper *EchoRpcAction::echoAction(CDataWrapper *action_data, bool& detach) {
    detach = true;
    return action_data;
}
//-----------------------------------------------------------------

CheckDomainRpcAction::CheckDomainRpcAction(AbstractCommandDispatcher *_dispatcher):
dispatcher(_dispatcher) {
    //register the action for the response
    AbstActionDescShrPtr
    actionDescription = DeclareAction::addActionDescritionInstance<CheckDomainRpcAction>(this,
                                                                     &CheckDomainRpcAction::checkDomain,
                                                                     NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                     NodeDomainAndActionRPC::ACTION_CHECK_DOMAIN,
                                                                     "Return information about rpc domain");
    actionDescription->addParam("domain_name",
                                DataType::TYPE_STRING,
                                "Is the name of the rpc domain to ckeck");
}
//!echo function return the data sent as parameter
CDataWrapper *CheckDomainRpcAction::checkDomain(CDataWrapper *action_data, bool& detach) {
    CHAOS_ASSERT(dispatcher);
    CHECK_CDW_THROW_AND_LOG(action_data, ACDLERR_, -1, "Input parameter as mandatory");
    CHECK_KEY_THROW_AND_LOG(action_data, "domain_name", ACDLERR_, -2, "domain_name key, representing the domain to ckeck, is mandatory!");
    CHECK_ASSERTION_THROW_AND_LOG(action_data->isStringValue("domain_name"), ACDLERR_, -3, "domain_name key need to be a string");
    
    const std::string domain_name = action_data->getStringValue("domain_name");
    
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    bool alive = dispatcher->hasDomain(domain_name);
    uint32_t queued_action_in_domain = dispatcher->domainRPCActionQueued(domain_name);
    //create the result data pack
    result->addBoolValue("alive", alive);
    result->addInt32Value("queued_actions", queued_action_in_domain);
    return result.release();
}
//-----------------------------------------------------------------

AbstractCommandDispatcher::AbstractCommandDispatcher(string alias):
NamedService(alias),
checkDomainAction(this){
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
    registerAction(&echoActionClass);
    registerAction(&checkDomainAction);
}

void AbstractCommandDispatcher::stop() throw(CException) {
    deregisterAction(&checkDomainAction);
    deregisterAction(&echoActionClass);
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
boost::shared_ptr<DomainActions> AbstractCommandDispatcher::getDomainActionsFromName(const string& domain_name) {
    //check if is not preset, so we can allocate it
    if(!actionDomainExecutorMap.count(domain_name)){
        boost::shared_ptr<DomainActions>  result(new DomainActions(domain_name));
        if(result){;
            actionDomainExecutorMap.insert(make_pair(domain_name, result));
            DEBUG_CODE(ACDLDBG_ << "Allocated new  DomainActions:" << domain_name;);
        }
    }
    //return the domain executor for name
    return actionDomainExecutorMap[domain_name];
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
        //get the domain executor for this action descriptor
        boost::shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
        //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->addActionDescriptor(*actDescIter);
            DEBUG_CODE(ACDLDBG_	<< "Registered action [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName)
                       << "] for domain [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain) << "]";);
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
        //get the domain executor for this action descriptor
        boost::shared_ptr<DomainActions> domainExecutor = getDomainActionsFromName((*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain));
        
        //if the domain executor has been returned, add this action to it
        if(domainExecutor) {
            domainExecutor->removeActionDescriptor(*actDescIter);
            DEBUG_CODE(ACDLDBG_	<< "Deregistered action [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionName)
                       << "] for domain [" << (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain) << "]";);
            
            if(!domainExecutor->registeredActions()) {
                std::string domain_name = domainExecutor->getDomainName();
                DEBUG_CODE(ACDLDBG_ << "No more action in domain " << domain_name << " so it will be destroyed";);
                actionDomainExecutorMap.erase(domain_name);
            }
        }
    }
}

/*
 Send a message
 */
bool AbstractCommandDispatcher::submitMessage(string& server_port,
                                              chaos::common::data::CDataWrapper* message,
                                              bool onThisThread)  throw(CException) {
    CHAOS_ASSERT(message && rpcForwarderPtr)
    if(!message && server_port.size()) return false;
    common::network::NetworkForwardInfo *nfi = new NetworkForwardInfo(false);
    nfi->destinationAddr = server_port;
    nfi->setMessage(message);
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

bool AbstractCommandDispatcher::hasDomain(const std::string& domain_name) {
    return actionDomainExecutorMap.count(domain_name);
}

uint32_t AbstractCommandDispatcher::domainRPCActionQueued(const std::string& domain_name) {
    return 0;
}
