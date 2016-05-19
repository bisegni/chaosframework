/*
 *	node_monitor_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/03/16 INFN, National Institute of Nuclear Physics
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

MessageRequestDomain::MessageRequestDomain():
domain_id(UUIDUtil::generateUUIDLite()),
request_id_counter(0){
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
}

uint32_t MessageRequestDomain::getNextRequestID() {
    return request_id_counter++;
}

uint32_t MessageRequestDomain::getCurrentRequestID() {
    return request_id_counter;
}

const std::string& MessageRequestDomain::getDomainID() {
    return domain_id;
}

/*!
 called when a result of a message is received
 */
CDataWrapper *MessageRequestDomain::response(CDataWrapper *response_data, bool& detach) {
    if(response_data == NULL) return NULL;
    if(!response_data->hasKey(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID)) return NULL;
    uint32_t request_id = response_data->getInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID);
    try {
        //lock the map
        boost::lock_guard<boost::mutex> lock(mutext_answer_managment);
        
        DEBUG_CODE(MRDDBG_ << "Received answer with id:" << request_id;)
        
        //check if the requester is waith the answer
        MapPromisesIterator p_iter = map_request_id_promises.find(request_id);
        
        if((detach = (p_iter != map_request_id_promises.end()))) {
            DEBUG_CODE(MRDDBG_ << "We have promises for id:" << request_id);
            //set the value in promises
            p_iter->second->set_value(FuturePromiseData(response_data));
            
            //delete the promises after have been set the data
            map_request_id_promises.erase(p_iter);
        } else {
            DEBUG_CODE(MRDDBG_ << "No promises found for id:" << request_id;)
        }
    } catch (...) {
        DEBUG_CODE(MRDDBG_ << "An error occuring dispatching the response:" << request_id;)
    }
    return NULL;
}

std::auto_ptr<MessageRequestFuture> MessageRequestDomain::getNewRequestMessageFuture(CDataWrapper& new_request_datapack,
                                                                                     uint32_t& new_request_id) {
    //lock the map
    boost::lock_guard<boost::mutex> lock(mutext_answer_managment);
    //get new request id
    new_request_id = request_id_counter++;
    
    //create future and promises
    boost::shared_ptr<MessageFuturePromise> promise(new MessageFuturePromise());
    
    //insert into themap the promises
    map_request_id_promises.insert(make_pair(new_request_id, promise));
    
    new_request_datapack.addInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID, new_request_id);
    new_request_datapack.addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN, domain_id);
    new_request_datapack.addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION, "response");
    
    DEBUG_CODE(MRDDBG_ << "New MessageRequestFuture create with id " << new_request_id << " on answer domain " << domain_id;);
    //return future
    return std::auto_ptr<MessageRequestFuture>(new  MessageRequestFuture(new_request_id,
                                                                         promise->get_future()));
}

