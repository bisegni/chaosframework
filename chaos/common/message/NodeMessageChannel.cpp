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
nodeAddress(_nodeAddress){CHAOS_ASSERT(nodeAddress.get())}

NodeMessageChannel::~NodeMessageChannel(){}

void NodeMessageChannel::setNewAddress(CNodeNetworkAddress *_nodeAddress) {
    nodeAddress.reset(_nodeAddress);
}

void NodeMessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    MessageChannel::getRpcPublishedHostAndPort(rpc_published_host_port);
}

void NodeMessageChannel::sendMessage(const std::string& node_id,
                                     const std::string& action_name,
                                     CDWUniquePtr message_pack) {
    MessageChannel::sendMessage(nodeAddress->ip_port,
                                node_id,
                                action_name,
                                ChaosMoveOperator(message_pack));
}

CDWUniquePtr NodeMessageChannel::sendRequest(const std::string& node_id,
                                              const std::string& action_name,
                                              CDWUniquePtr request_pack,
                                              int32_t millisec_to_wait) {
    return MessageChannel::sendRequest(nodeAddress->ip_port,
                                       node_id,
                                       action_name,
                                       ChaosMoveOperator(request_pack),
                                       millisec_to_wait);
}

ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::sendRequestWithFuture(const std::string& node_id,
                                                                              const std::string& action_name,
                                                                              CDWUniquePtr request_pack) {
    return MessageChannel::sendRequestWithFuture(nodeAddress->ip_port,
                                                 node_id,
                                                 action_name,
                                                 ChaosMoveOperator(request_pack));
}

//! Send a request for receive RPC information
ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::checkRPCInformation(const std::string& node_id) {
    return MessageChannel::checkRPCInformation(nodeAddress->ip_port,
                                               node_id);
}

//! Send a request for an echo test
ChaosUniquePtr<MessageRequestFuture> NodeMessageChannel::echoTest(chaos::common::data::CDWUniquePtr echo_data) {
    return MessageChannel::echoTest(nodeAddress->ip_port,
                                    ChaosMoveOperator(echo_data));
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
