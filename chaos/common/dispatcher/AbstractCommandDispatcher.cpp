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

AbstractCommandDispatcher::AbstractCommandDispatcher(const string& alias):
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

bool AbstractCommandDispatcher::submitMessage(const string& server_port,
                                              CDWUniquePtr message,
                                              bool onThisThread)  throw(CException) {
    CHAOS_ASSERT(message.get() && rpc_forwarder_ptr)
    if(!message.get() && server_port.size()) return false;
    common::network::NetworkForwardInfo *nfi = new NetworkForwardInfo(false);
    nfi->destinationAddr = server_port;
    nfi->setMessage(ChaosMoveOperator(message));
    return rpc_forwarder_ptr->submitMessage(nfi, onThisThread);
}

chaos::common::data::CDataWrapper* AbstractCommandDispatcher::updateConfiguration(chaos::common::data::CDataWrapper*)  throw(CException) {
    return NULL;
}

void AbstractCommandDispatcher::setRpcForwarder(RpcMessageForwarder *newRpcForwarderPtr){
    rpc_forwarder_ptr = newRpcForwarderPtr;
}

