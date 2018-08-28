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

#include "RpcClient.h"
#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#define RPCC_LAPP INFO_LOG(RpcClient)
#define RPCC_LDBG DBG_LOG(RpcClient)
#define RPCC_LERR ERR_LOG(RpcClient)

using namespace chaos;
using namespace chaos::common::data;

/*!
 Constructor di default per i
 */
RpcClient::RpcClient(const std::string& alias):
NamedService(alias),
syncrhonous_call(GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_RPC_SYNC_ENABLE)),
server_handler(NULL){}

/*!
 Forward to dispatcher the error during the forwarding of the request message
 */
void RpcClient::forwadSubmissionResult(NetworkForwardInfo *message_info,
                                       CDataWrapper *submission_result) {
    CHAOS_ASSERT(server_handler && submission_result)
    //! chec if it is a request
    if(message_info->sender_node_id.size() == 0) {
        DELETE_OBJ_POINTER(submission_result)
        return;
    }
    RPCC_LDBG << "ACK received:" <<submission_result->getJSONString();
    
    //set the request id
    submission_result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, message_info->sender_request_id);
    
    //! there is an error during submission so we need to answer to request with this error
    CDataWrapper *answer_to_send = new CDataWrapper();
    //set the domain and action name
    answer_to_send->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, message_info->sender_node_id);
    answer_to_send->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, "response");
    // add reuslt to answer
    if(submission_result) {answer_to_send->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *submission_result);}
    //forward answer to channel
    ChaosUniquePtr<chaos::common::data::CDataWrapper> to_delete(server_handler->dispatchCommand(answer_to_send));
    
    DELETE_OBJ_POINTER(submission_result)
}

/*!
 Forward to dispatcher the error during the forwarding of the request message
 */
void RpcClient::forwadSubmissionResultError(const std::string& channel_node_id,
                                            uint32_t message_request_id,
                                            CDataWrapper *submission_result) {
    CHAOS_ASSERT(server_handler && submission_result)
    //! chec if it is a request
    if(channel_node_id.size() == 0) {
        DELETE_OBJ_POINTER(submission_result)
        return;
    }
    
    RPCC_LDBG << "ACK received:" <<submission_result->getJSONString();
    
    if(submission_result->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {
        int err=submission_result->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
        if(err) {
            
            //set the request id
            submission_result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, message_request_id);
            
            //! there is an error during submission so we need to answer to request with this error
            CDataWrapper *answer_to_send = new CDataWrapper();
            //set the domain and action name
            answer_to_send->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, channel_node_id);
            answer_to_send->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, "response");
            // add reuslt to answer
            answer_to_send->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *submission_result);
            //forward answer to channel
            ChaosUniquePtr<chaos::common::data::CDataWrapper> to_delete(server_handler->dispatchCommand(answer_to_send));
        }
    } else{
        RPCC_LERR <<"NO "<<RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE;
    }
    
    DELETE_OBJ_POINTER(submission_result)
}

/*!
 Forward to dispatcher the error durngi the forwarding of the request message
 */
void RpcClient::forwadSubmissionResultError(NetworkForwardInfo *message_info,
                                            int32_t error_code,
                                            const std::string& error_message,
                                            const std::string& error_domain) {
    RPCC_LDBG << "Error on sending (code:" << error_code << " Message:" << error_message << ")";
    
    CDataWrapper *answer = new CDataWrapper();
    
    //set the domain and action name
    answer->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, message_info->sender_node_id);
    answer->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, "response");
    
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> submission_result(new CDataWrapper());
    //set the request id
    submission_result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID, message_info->sender_request_id);
    
    //! set the error information
    submission_result->addInt32Value("__internal_redirect__", 1);
    submission_result->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, (error_code));
    submission_result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, error_domain);
    submission_result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, error_message);
    submission_result->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_SERVER_ADDR, message_info->destinationAddr);
    //if(message_info->message.get())submission_result->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *message_info->message.get());
    
    answer->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *submission_result.get());
    //forward answer to channel
    ChaosUniquePtr<chaos::common::data::CDataWrapper> to_delete(server_handler->dispatchCommand(answer));
}


void RpcClient::setServerHandler(RpcServerHandler *_server_handler) {
    server_handler = _server_handler;
}

//! return the numebr of message that are waiting to be sent
uint64_t RpcClient::getMessageQueueSize() {
    return 0;
}
