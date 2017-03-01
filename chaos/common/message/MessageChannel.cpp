/*
 *	MessageChannel.cpp
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
                               boost::shared_ptr<MessageRequestDomain> _new_message_request_domain):
broker(_broker),
last_error_code(0),
last_error_message(),
last_error_domain(),
message_request_domain(_new_message_request_domain),
channel_uuid(UUIDUtil::generateUUIDLite()){}

MessageChannel::~MessageChannel() {
    
}

/*!
 Initialization phase
 */
void MessageChannel::init() throw(CException) {
}

/*!
 Deinitialization phase
 */
void MessageChannel::deinit() throw(CException) {
}

/*!
 Return the broker for that channel
 */
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

boost::shared_ptr<MessageRequestDomain> MessageChannel::getMessageRequestDomain() {
    return message_request_domain;
}

/*!
 called when a result of an
 */
void MessageChannel::sendMessage(const std::string& remote_host,
                                 const std::string& node_id,
                                 const std::string& action_name,
                                 CDataWrapper *message_pack) {
    CDataWrapper *data_pack = new CDataWrapper();
    //add the action and dommain name
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    if(message_pack)data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *message_pack);
    //send the request
    broker->submitMessage(remote_host.c_str(), data_pack);
}

/*
 */
CDataWrapper* MessageChannel::sendRequest(const std::string& remote_host,
                                          const std::string& node_id,
                                          const std::string& action_name,
                                          CDataWrapper *request_pack,
                                          int32_t millisec_to_wait) {
    CHAOS_ASSERT(broker)    auto_ptr<MessageRequestFuture> request_future(sendRequestWithFuture(remote_host,
                                                                                                node_id,
                                                                                                action_name,
                                                                                                request_pack));
    
    
    
    //wait for some time or, if == -1, forever
    if(request_future->wait(millisec_to_wait)) {
        CDataWrapper *result = request_future->detachResult();
        last_error_code = request_future->getError();
        last_error_domain = request_future->getErrorDomain();
        last_error_message = request_future->getErrorMessage();
        return result;
    } else {
        return NULL;
    }
    
}

void MessageChannel::requestPromisesHandler(const FuturePromiseData& response_data) {}

//!send an rpc request to a remote node
/*!
 send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
 been received the method return with a NULL pointer
 \param remote_host is the host:port string that identify the remote server where send the rpc request
 \param node_id id of the node into remote chaos rpc system
 \param action_name name of the actio to call
 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
 \return the future object to inspec and whait the result
 */
std::auto_ptr<MessageRequestFuture> MessageChannel::sendRequestWithFuture(const std::string& remote_host,
                                                                          const std::string& node_id,
                                                                          const std::string& action_name,
                                                                          CDataWrapper *request_pack) {
    CHAOS_ASSERT(broker)
    uint32_t new_request_id = 0;
    auto_ptr<MessageRequestFuture> result;
    CDataWrapper *data_pack = new CDataWrapper();
    
    //lock lk(waith_asnwer_mutex);
    if(request_pack)data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *request_pack);
    
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    
    //complete datapack for request and get the unique future
    result = message_request_domain->getNewRequestMessageFuture(*data_pack,
                                                                new_request_id,
                                                                boost::bind(&MessageChannel::requestPromisesHandler, this, _1));

    //if(async) return result;
    //submit the request
    broker->submiteRequest(remote_host,
                           data_pack,
                           message_request_domain->getDomainID(),
                           new_request_id);
    return result;
}

//! get the rpc published host and port
void MessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    return broker->getPublishedHostAndPort(rpc_published_host_port);
}

std::auto_ptr<MessageRequestFuture> MessageChannel::checkRPCInformation(const std::string& remote_host,
                                                                        const std::string& node_id) {
    CDataWrapper data_pack;
    data_pack.addStringValue("domain_name", node_id);
    return sendRequestWithFuture(remote_host,
                                 NodeDomainAndActionRPC::RPC_DOMAIN,
                                 NodeDomainAndActionRPC::ACTION_CHECK_DOMAIN,
                                 &data_pack);
}

std::auto_ptr<MessageRequestFuture> MessageChannel::echoTest(const std::string& remote_host,
                                                             CDataWrapper *echo_data) {
    return sendRequestWithFuture(remote_host,
                                 NodeDomainAndActionRPC::RPC_DOMAIN,
                                 NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                 echo_data);
}
