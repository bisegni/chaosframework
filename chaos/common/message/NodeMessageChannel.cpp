/*
 *	NodeMessageChannel.cpp
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

#include <chaos/common/message/NodeMessageChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::message;
//!Base constructor
NodeMessageChannel::NodeMessageChannel(NetworkBroker *msgBroker,
                                       CNodeNetworkAddress *_nodeAddress,
                                       MessageRequestDomainSHRDPtr _new_message_request_domain):
MessageChannel(msgBroker,
               _new_message_request_domain),
nodeAddress(_nodeAddress){
    CHAOS_ASSERT(nodeAddress.get())
}

NodeMessageChannel::~NodeMessageChannel(){
}

//
void NodeMessageChannel::setNewAddress(CNodeNetworkAddress *_nodeAddress) {
    nodeAddress.reset(_nodeAddress);
}

//! get the rpc published host and port
void NodeMessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    MessageChannel::getRpcPublishedHostAndPort(rpc_published_host_port);
}

/*!
 \brief send a message
 \param node_id id of the remote node within a network broker interface
 \param action_name the name of the action to call
 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
 \param on_this_thread notify when the message need to be sent syncronously or in async  way
 */
void NodeMessageChannel::sendMessage(const std::string& node_id,
                                     const std::string& action_name,
                                     CDataWrapper *message_pack) {
    MessageChannel::sendMessage(nodeAddress->ip_port,
                                node_id,
                                action_name,
                                message_pack);
}

/*!
 \brief send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
 been received the method return with a NULL pointer
 \param node_id id of the node into remote chaos rpc system
 \param action_name name of the actio to call
 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
 \param millisec_to_wait waith the response for onli these number of millisec then return
 \param on_this_thread notify when the message need to be sent syncronously or in async  way
 \return the answer of the request, a null value mean that the wait time is expired
 */
CDataWrapper* NodeMessageChannel::sendRequest(const std::string& node_id,
                                              const std::string& action_name,
                                              CDataWrapper *request_pack,
                                              int32_t millisec_to_wait) {
    return MessageChannel::sendRequest(nodeAddress->ip_port,
                                       node_id,
                                       action_name,
                                       request_pack,
                                       millisec_to_wait);
}

//!send an rpc request to a remote node
std::auto_ptr<MessageRequestFuture> NodeMessageChannel::sendRequestWithFuture(const std::string& node_id,
                                                                              const std::string& action_name,
                                                                              CDataWrapper *request_pack) {
    return MessageChannel::sendRequestWithFuture(nodeAddress->ip_port,
                                                 node_id,
                                                 action_name,
                                                 request_pack);
}

//! Send a request for receive RPC information
std::auto_ptr<MessageRequestFuture> NodeMessageChannel::checkRPCInformation(const std::string& node_id) {
    return MessageChannel::checkRPCInformation(nodeAddress->ip_port,
                                               node_id);
}

//! Send a request for an echo test
std::auto_ptr<MessageRequestFuture> NodeMessageChannel::echoTest(chaos::common::data::CDataWrapper *echo_data) {
    return MessageChannel::echoTest(nodeAddress->ip_port,
                                    echo_data);
}

//! return last sendxxx error code
int32_t NodeMessageChannel::getLastErrorCode() {
    return MessageChannel::getLastErrorCode();
}

//! return last sendxxx error message
const std::string& NodeMessageChannel::getLastErrorMessage() {
    return MessageChannel::getLastErrorMessage();
}

//! return last sendxxx error domain
const std::string& NodeMessageChannel::getLastErrorDomain() {
    return MessageChannel::getLastErrorDomain();
}
