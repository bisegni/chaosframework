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
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
    bool alive = dispatcher->hasDomain(domain_name);
    uint32_t queued_action_in_domain = 0;
    //create the result data pack
    result->addBoolValue("alive", alive);
    result->addInt32Value("queued_actions", queued_action_in_domain);
    return result.release();
}

AbstractCommandDispatcher::AbstractCommandDispatcher(string alias):
NamedService(alias),
check_domain_action(this),
rpc_forwarder_ptr(NULL){
}

AbstractCommandDispatcher::~AbstractCommandDispatcher() {}

void AbstractCommandDispatcher::init(void *init_data)  throw(CException) {}

void AbstractCommandDispatcher::start() throw(CException) {
    registerAction(&echo_action_class);
    registerAction(&check_domain_action);
}

void AbstractCommandDispatcher::stop() throw(CException) {
    deregisterAction(&check_domain_action);
    deregisterAction(&echo_action_class);
}

void AbstractCommandDispatcher::deinit()  throw(CException) {}

bool AbstractCommandDispatcher::submitMessage(string& server_port,
                                              chaos::common::data::CDataWrapper* message,
                                              bool onThisThread)  throw(CException) {
    CHAOS_ASSERT(message && rpc_forwarder_ptr)
    if(!message && server_port.size()) return false;
    common::network::NetworkForwardInfo *nfi = new NetworkForwardInfo(false);
    nfi->destinationAddr = server_port;
    nfi->setMessage(message);
    return rpc_forwarder_ptr->submitMessage(nfi, onThisThread);
}

chaos::common::data::CDataWrapper* AbstractCommandDispatcher::updateConfiguration(chaos::common::data::CDataWrapper*)  throw(CException) {
    return NULL;
}

void AbstractCommandDispatcher::setRpcForwarder(RpcMessageForwarder *newRpcForwarderPtr){
    rpc_forwarder_ptr = newRpcForwarderPtr;
}

