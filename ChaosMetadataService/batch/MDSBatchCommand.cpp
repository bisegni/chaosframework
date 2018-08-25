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
#include "MDSBatchCommand.h"
#include "MDSBatchExecutor.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::batch_command;

using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::persistence::data_access;

#define MDSBC_INFO INFO_LOG(MDSBatchCommand)
#define MDSBC_DBG  DBG_LOG(MDSBatchCommand)
#define MDSBC_ERR  ERR_LOG(MDSBatchCommand)

//! default constructor
MDSBatchCommand::MDSBatchCommand():
BatchCommand(),
message_channel(NULL),
multiaddress_message_channel(NULL),
executor_instance(NULL),
abstract_persistance_driver(NULL){}

//! default destructor
MDSBatchCommand::~MDSBatchCommand() {}

chaos::common::message::MessageChannel*
MDSBatchCommand::getMessageChannel() {
    CHAOS_ASSERT(message_channel)
    return message_channel;
}

chaos::common::message::MultiAddressMessageChannel*
MDSBatchCommand::getMultiAddressMessageChannel() {
    CHAOS_ASSERT(multiaddress_message_channel)
    return multiaddress_message_channel;
}

// return the implemented handler
uint8_t MDSBatchCommand::implementedHandler() {
    return  HandlerType::HT_Set|
    HandlerType::HT_Correlation|
    HandlerType::HT_Acquisition;
}

// inherited method
void MDSBatchCommand::setHandler(CDataWrapper *data) {
    //set default scheduler delay 0,5 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)50000);
    //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)20000000);
    //set in exclusive running property
    BC_EXCLUSIVE_RUNNING_PROPERTY;
}

// inherited method
void MDSBatchCommand::acquireHandler() {
    
}

// inherited method
void MDSBatchCommand::ccHandler() {
    
}

// inherited method
bool MDSBatchCommand::timeoutHandler() {
    return true;
}

//! create a request to a remote rpc action
ChaosUniquePtr<RequestInfo> MDSBatchCommand::createRequest(const std::string& remote_address,
                                                           const std::string& remote_domain,
                                                           const std::string& remote_action) throw (chaos::CException) {
    return ChaosUniquePtr<RequestInfo> (new RequestInfo(remote_address,
                                                        remote_domain,
                                                        remote_action));
}

void MDSBatchCommand::sendRequest(RequestInfo& request_info,
                                  CDWUniquePtr message) throw (chaos::CException) {
    CHAOS_ASSERT(message_channel)
    
    request_info.request_future = message_channel->sendRequestWithFuture(request_info.remote_address,
                                                                         request_info.remote_domain,
                                                                         request_info.remote_action,
                                                                         ChaosMoveOperator(message));
    request_info.phase = MESSAGE_PHASE_SENT;
}

void MDSBatchCommand::sendMessage(RequestInfo& request_info,
                                  CDWUniquePtr message) throw (chaos::CException) {
    CHAOS_ASSERT(message_channel)
    message_channel->sendMessage(request_info.remote_address,
                                 request_info.remote_domain,
                                 request_info.remote_action,
                                 ChaosMoveOperator(message));
    request_info.phase = MESSAGE_PHASE_COMPLETED;
}

ChaosUniquePtr<RequestInfo> MDSBatchCommand::sendRequest(const std::string& node_uid,
                                                         const std::string& rpc_action,
                                                         chaos::common::data::CDataWrapper *message) throw (chaos::CException) {
    CDataWrapper *tmp_ptr = NULL;
    int err = 0;
    bool alive = false;
    ChaosUniquePtr<RequestInfo> new_request;
    if((err = getDataAccess<NodeDataAccess>()->getNodeDescription(node_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(MDSBC_ERR, err, "Error loading infomation for node '%1%'", %node_uid);
    } else if(!tmp_ptr) {
        LOG_AND_TROW_FORMATTED(MDSBC_ERR, -1, "No description found for node '%1%'", %node_uid);
    }
    try{
        ChaosUniquePtr<CDataWrapper> node_description(tmp_ptr);
        CHECK_ASSERTION_THROW_AND_LOG(node_description->hasKey(NodeDefinitionKey::NODE_RPC_ADDR), MDSBC_ERR, -2, CHAOS_FORMAT("%1% key has not been found on node description", %NodeDefinitionKey::NODE_RPC_ADDR));
        CHECK_ASSERTION_THROW_AND_LOG(node_description->isStringValue(NodeDefinitionKey::NODE_RPC_ADDR), MDSBC_ERR, -2, CHAOS_FORMAT("%1% key need to be a string", %NodeDefinitionKey::NODE_RPC_ADDR));
        CHECK_ASSERTION_THROW_AND_LOG(node_description->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN), MDSBC_ERR, -3, CHAOS_FORMAT("%1% key ha nots been found on node description", %NodeDefinitionKey::NODE_RPC_DOMAIN));
        CHECK_ASSERTION_THROW_AND_LOG(node_description->isStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN), MDSBC_ERR, -4, CHAOS_FORMAT("%1% key need to be a string", %NodeDefinitionKey::NODE_RPC_DOMAIN));
        const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
        const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
        new_request = createRequest(node_rpc_address,
                                    node_rpc_domain,
                                    rpc_action);
        if(getDataAccess<NodeDataAccess>()->isNodeAlive(node_uid,
                                                        alive) ||
           alive == false) {
            new_request->phase = MESSAGE_PHASE_TIMEOUT;
        } else {
            sendRequest(*new_request,
                        message);
        }
    }catch(...){
        throw CException(-1, "erroro using bson", __PRETTY_FUNCTION__);
    }
    return new_request;
}

ChaosUniquePtr<RequestInfo> MDSBatchCommand::sendMessage(const std::string& node_uid,
                                                         const std::string& rpc_action,
                                                         chaos::common::data::CDataWrapper *message) throw (chaos::CException) {
    CDataWrapper *tmp_ptr = NULL;
    int err = 0;
    bool alive = false;
    if((err = getDataAccess<NodeDataAccess>()->getNodeDescription(node_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(MDSBC_ERR, err, "Error loading infomation for node '%1%'", %node_uid);
    } else if(!tmp_ptr) {
        LOG_AND_TROW_FORMATTED(MDSBC_ERR, -1, "No description found for node '%1%'", %node_uid);
    }
    
    ChaosUniquePtr<CDataWrapper> node_description(tmp_ptr);
    CHECK_ASSERTION_THROW_AND_LOG(node_description->hasKey(NodeDefinitionKey::NODE_RPC_ADDR), MDSBC_ERR, -2, CHAOS_FORMAT("%1% key has not been found on node description", %NodeDefinitionKey::NODE_RPC_ADDR));
    CHECK_ASSERTION_THROW_AND_LOG(node_description->isStringValue(NodeDefinitionKey::NODE_RPC_ADDR), MDSBC_ERR, -2, CHAOS_FORMAT("%1% key need to be a string", %NodeDefinitionKey::NODE_RPC_ADDR));
    CHECK_ASSERTION_THROW_AND_LOG(node_description->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN), MDSBC_ERR, -3, CHAOS_FORMAT("%1% key ha nots been found on node description", %NodeDefinitionKey::NODE_RPC_DOMAIN));
    CHECK_ASSERTION_THROW_AND_LOG(node_description->isStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN), MDSBC_ERR, -4, CHAOS_FORMAT("%1% key need to be a string", %NodeDefinitionKey::NODE_RPC_DOMAIN));
    const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
    
    ChaosUniquePtr<RequestInfo> new_request = createRequest(node_rpc_address,
                                                            node_rpc_domain,
                                                            rpc_action);
    if(getDataAccess<NodeDataAccess>()->isNodeAlive(node_uid,
                                                    alive) ||
       alive == false) {
        new_request->phase = MESSAGE_PHASE_TIMEOUT;
    } else {
        sendMessage(*new_request,
                    message);
    }
    return new_request;
}

void MDSBatchCommand::manageRequestPhase(RequestInfo& request_info) throw (chaos::CException) {
    switch(request_info.phase) {
        case MESSAGE_PHASE_UNSENT:
            throw chaos::CException(-1, "Request is unsent", __PRETTY_FUNCTION__);
            break;
        case MESSAGE_PHASE_SENT: {
            MDSBC_DBG << "Waith for "<< request_info.remote_action <<
            " action ack from the control unit:" << request_info.remote_address <<
            " on rpc[" << request_info.remote_domain <<":"<<request_info.remote_action<<"]";
            if(!request_info.request_future.get()) {
                MDSBC_ERR << "request with no future";
                throw chaos::CException(-2, "request with no future", __PRETTY_FUNCTION__);
            }
            if(request_info.request_future->wait(1000)) {
                //we have hd answer
                if(request_info.request_future->getError()) {
                    MDSBC_ERR << "We have had answer with error"
                    << request_info.request_future->getError() << " \n and message "
                    << request_info.request_future->getErrorMessage() << "\n on domain "
                    << request_info.request_future->getErrorDomain();
                } else {
                    MDSBC_DBG << "Request send with success";
                }
                request_info.phase = MESSAGE_PHASE_COMPLETED;
            } else {
                if((++request_info.retry % 3) == 0) {
                    request_info.phase = MESSAGE_PHASE_TIMEOUT;
                }
            }
            break;
        }
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
            break;
    }
}
uint32_t MDSBatchCommand::getNextSandboxToUse() {
    CHAOS_ASSERT(executor_instance)
    return executor_instance->getNextSandboxToUse();
}

uint64_t MDSBatchCommand::submitCommand(const std::string& batch_command_alias,
                                        CDataWrapper *command_data,
                                        uint32_t sandbox_id,
                                        uint32_t priority) {
    CHAOS_ASSERT(executor_instance)
    return executor_instance->submitCommand(batch_command_alias,
                                            command_data,
                                            sandbox_id,
                                            priority);
}

uint64_t MDSBatchCommand::submitCommand(const std::string& batch_command_alias,
                                        CDataWrapper *command_data) {
    CHAOS_ASSERT(executor_instance)
    return executor_instance->submitCommand(batch_command_alias,
                                            command_data);
}
