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
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

//using namespace chaos;
using namespace chaos::common::network;
using namespace chaos::common::message;
using namespace chaos::common::data;
using namespace chaos::common::utility;
#define MessageChannel_LOG_HEAD "[MessageChannel] - "

#define MCAPP_ INFO_LOG(MessageChannel)
#define MCDBG_ DBG_LOG(MessageChannel)
#define MCERR_ ERR_LOG(MessageChannel)


MessageChannel::MessageChannel(NetworkBroker *_broker,
                               ChaosSharedPtr<MessageRequestDomain> _new_message_request_domain):
broker(_broker),
last_error_code(0),
last_error_message(),
last_error_domain(),
message_request_domain(_new_message_request_domain),
channel_uuid(UUIDUtil::generateUUIDLite()),
safe_promises_handler_caller(PromisesHandlerSharedPtr(new PromisesHandler(boost::bind(&MessageChannel::_callHandler, this, _1)))){}

MessageChannel::~MessageChannel() {}

void MessageChannel::init() throw(CException) {
    //not implemented
}


void MessageChannel::deinit() throw(CException) {
    //invalidate safety handler
    safe_promises_handler_caller.reset();
}

void MessageChannel::_callHandler(const FuturePromiseData& response_data) {
    requestPromisesHandler(response_data);
}

NetworkBroker* MessageChannel::getBroker() {
    return broker;
}

int32_t MessageChannel::getLastErrorCode() {
    return last_error_code;
}

const std::string& MessageChannel::getLastErrorMessage() {
    return last_error_message;
}

const std::string& MessageChannel::getLastErrorDomain() {
    return last_error_domain;
}

const std::string& MessageChannel::getChannelUUID() {
    return channel_uuid;
}

ChaosSharedPtr<MessageRequestDomain> MessageChannel::getMessageRequestDomain() {
    return message_request_domain;
}

void MessageChannel::sendMessage(const std::string& remote_host,
                                 const std::string& node_id,
                                 const std::string& action_name,
                                 CDWUniquePtr message_pack) {
    CDWUniquePtr data_pack(new CDataWrapper());
    //add the action and dommain name
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    if(message_pack.get()){data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *message_pack);}
    //send the request
    broker->submitMessage(remote_host,
                          MOVE(data_pack));
}

/*
 */
CDWUniquePtr MessageChannel::sendRequest(const std::string& remote_host,
                                         const std::string& node_id,
                                         const std::string& action_name,
                                         CDWUniquePtr request_pack,
                                         int32_t millisec_to_wait) {
    CHAOS_ASSERT(broker)
    ChaosUniquePtr<MessageRequestFuture> request_future(sendRequestWithFuture(remote_host,
                                                                              node_id,
                                                                              action_name,
                                                                              MOVE(request_pack)));
    
    
    
    //wait for some time or, if == -1, forever
    if(request_future->wait(millisec_to_wait)) {
        CDWUniquePtr result = request_future->detachResult();
        last_error_code = request_future->getError();
        last_error_domain = request_future->getErrorDomain();
        last_error_message = request_future->getErrorMessage();
        return result;
    } else {
        return CDWUniquePtr();
    }
    
}

void MessageChannel::requestPromisesHandler(const FuturePromiseData& response_data) {}

//!send an rpc request to a remote node
/*!
 send a synCronus request and can wait for a determinated number of milliseconds the answer. If it has not
 been received the method return with a NULL pointer
 \param remote_host is the host:port string that identify the remote server where send the rpc request
 \param node_id id of the node into remote chaos rpc system
 \param action_name name of the actio to call
 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
 \return the future object to inspec and whait the result
 */
ChaosUniquePtr<MessageRequestFuture> MessageChannel::sendRequestWithFuture(const std::string& remote_host,
                                                                           const std::string& node_id,
                                                                           const std::string& action_name,
                                                                           CDWUniquePtr request_pack) {
    CHAOS_ASSERT(broker)
    uint32_t new_request_id = 0;
    ChaosUniquePtr<MessageRequestFuture> result;
    CDWUniquePtr data_pack(new CDataWrapper());
    
    //lock lk(waith_asnwer_mutex);
    if(request_pack.get())data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *request_pack);
    
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    
    
    //complete datapack for request and get the unique future
    result = message_request_domain->getNewRequestMessageFuture(*data_pack,
                                                                new_request_id,
                                                                safe_promises_handler_caller);
    
    //if(async) return result;
    //submit the request
    broker->submiteRequest(remote_host,
                           MOVE(data_pack),
                           message_request_domain->getDomainID(),
                           new_request_id);
    return result;
}

//! get the rpc published host and port
void MessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    return broker->getPublishedHostAndPort(rpc_published_host_port);
}

ChaosUniquePtr<MessageRequestFuture> MessageChannel::checkRPCInformation(const std::string& remote_host,
                                                                         const std::string& node_id) {
    CDWUniquePtr data_pack(new CDataWrapper());
    data_pack->addStringValue("domain_name", node_id);
    return sendRequestWithFuture(remote_host,
                                 NodeDomainAndActionRPC::RPC_DOMAIN,
                                 NodeDomainAndActionRPC::ACTION_CHECK_DOMAIN,
                                 MOVE(data_pack));
}

ChaosUniquePtr<MessageRequestFuture> MessageChannel::echoTest(const std::string& remote_host,
                                                              CDWUniquePtr echo_data) {
    return sendRequestWithFuture(remote_host,
                                 NodeDomainAndActionRPC::RPC_DOMAIN,
                                 NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                 MOVE(echo_data));
}
