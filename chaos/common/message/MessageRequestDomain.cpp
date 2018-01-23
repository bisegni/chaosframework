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

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MessageRequestFuture.h>
#include <chaos/common/message/MessageRequestDomain.h>

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::message;

#define MRDAPP_ INFO_LOG(MessageRequestDomain)
#define MRDDBG_ DBG_LOG(MessageRequestDomain)
#define MRDERR_ ERR_LOG(MessageRequestDomain)

ChaosMessagePromises::ChaosMessagePromises(PromisesHandlerWeakPtr _promises_handler_weak):
promises_handler_weak(_promises_handler_weak){}

void ChaosMessagePromises::set_value(const FuturePromiseData& received_data) {
    PromisesHandlerSharedPtr shr_ptr = promises_handler_weak.lock();
    if(shr_ptr.get() != NULL) {
        shr_ptr->function(received_data);
    }
   MessageFuturePromise::set_value(received_data);
}

MessageRequestDomain::MessageRequestDomain():
domain_id(UUIDUtil::generateUUIDLite()){
    future_helper.init(NULL);
    //register the action for the response
    DeclareAction::addActionDescritionInstance<MessageRequestDomain>(this,
                                                                     &MessageRequestDomain::response,
                                                                     domain_id,
                                                                     "response",
                                                                     "Receive the reponse for a request");
    
    //register to network broker for receive messages
    NetworkBroker::getInstance()->registerAction(this);
}

MessageRequestDomain::~MessageRequestDomain() {
    //deregister to network broker no don't need more messages
    NetworkBroker::getInstance()->deregisterAction(this);
    future_helper.deinit();
}

const std::string& MessageRequestDomain::getDomainID() {
    return domain_id;
}

/*!
 called when a result of a message is received
 */
CDataWrapper *MessageRequestDomain::response(CDataWrapper *response_data, bool& detach) {
    if(response_data == NULL) return NULL;
    detach = true;
    CDWShrdPtr response_data_sp(response_data);
    if(!response_data->hasKey(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID)) return NULL;
    uint32_t request_id = response_data->getInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID);
    future_helper.setDataForPromiseID(request_id, response_data_sp);
    return NULL;
}

ChaosUniquePtr<MessageRequestFuture> MessageRequestDomain::getNewRequestMessageFuture(CDataWrapper& new_request_datapack,
                                                                                      uint32_t& new_request_id,
                                                                                      PromisesHandlerWeakPtr promises_handler_weak) {
    
    //create future and promises
    ChaosSharedPtr<ChaosMessagePromises> promise(new ChaosMessagePromises(promises_handler_weak));
    MessageRequestDomainFutureHelper::Future request_future;
    future_helper.addNewPromise(new_request_id, request_future);
    
    new_request_datapack.addInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID, new_request_id);
    new_request_datapack.addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN, domain_id);
    new_request_datapack.addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION, "response");
    
    DEBUG_CODE(MRDDBG_ << "New MessageRequestFuture create with id " << new_request_id << " on answer domain " << domain_id;);
    //return future
    return ChaosUniquePtr<MessageRequestFuture>(new  MessageRequestFuture(new_request_id,
                                                                          request_future));
}

