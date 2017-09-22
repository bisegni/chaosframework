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
ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::sendRequestWithFuture(const std::string& node_id,
                                                                              const std::string& action_name,
                                                                              CDataWrapper *request_pack) {
    return MessageChannel::sendRequestWithFuture(nodeAddress->ip_port,
                                                 node_id,
                                                 action_name,
                                                 request_pack);
}

//! Send a request for receive RPC information
ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::checkRPCInformation(const std::string& node_id) {
    return MessageChannel::checkRPCInformation(nodeAddress->ip_port,
                                               node_id);
}

//! Send a request for an echo test
ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::echoTest(chaos::common::data::CDataWrapper *echo_data) {
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
