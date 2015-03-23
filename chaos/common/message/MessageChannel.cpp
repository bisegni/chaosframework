/*
 *	MessageChannel.cpp
 *	!CHOAS
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

#define MCAPP_ INFO_LOG()
#define MCDBG_ DBG_LOG()
#define MCERR_ ERR_LOG()


MessageChannel::MessageChannel(NetworkBroker *_broker):
broker(_broker){
    
}

MessageChannel::~MessageChannel() {
    
}

/*!
 Initialization phase
 */
void MessageChannel::init() throw(CException) {
    //reset request id
    channelRequestIDCounter= 0;
    //create a new channel id
    channel_reponse_domain = UUIDUtil::generateUUIDLite();
    
    //register the action for the response
    DeclareAction::addActionDescritionInstance<MessageChannel>(this,
                                                               &MessageChannel::response,
                                                               channel_reponse_domain.c_str(),
                                                               "response",
                                                               "Receive the reponse for a request");
    
    broker->registerAction(this);
}

/*!
 Deinitialization phase
 */
void MessageChannel::deinit() throw(CException) {
    
    //create a new channel id
    broker->deregisterAction(this);
}

/*!
 called when a result of a message is received
 */
CDataWrapper *MessageChannel::response(CDataWrapper *response_data, bool& detach) {
    DEBUG_CODE(MCDBG_ << "Received answer";)
    if(!response_data->hasKey(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID)) return NULL;
    atomic_int_type request_id = response_data->getInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID);
    try {
        //lock the map
        boost::shared_lock< boost::shared_mutex > lock(mutext_answer_managment);
        
        DEBUG_CODE(MCDBG_ << "Received answer with id:" << request_id;)
        
        //check if the requester is waith the answer
        MapPromisesIterator p_iter = map_request_id_promises.find(request_id);
        
        if((detach = (p_iter != map_request_id_promises.end()))) {
            DEBUG_CODE(MCDBG_ << "We have promises for id:" << request_id);
            
            //set the valu ein promises
            p_iter->second->set_value(response_data);
            
            //delete the promises after have been set the data
            map_request_id_promises.erase(p_iter);
            
            //we have the promises
            lock.unlock();
        } else {
            DEBUG_CODE(MCDBG_ << "No promises found for id:" << request_id;)
        }
    } catch (...) {
        DEBUG_CODE(MCDBG_ << "An error occuring dispatching the response:" << request_id;)
    }
    return NULL;
}


/*!
 Return the broker for that channel
 */
NetworkBroker* MessageChannel::getBroker(){
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

/*!
 called when a result of an
 */
void MessageChannel::sendMessage(const std::string& remote_host,
                                 const std::string& node_id,
                                 const std::string& action_name,
                                 CDataWrapper *message_pack,
                                 bool on_this_thread) {
    CDataWrapper *data_pack = new CDataWrapper();
    //add the action and dommain name
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    if(message_pack)data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *message_pack);
    //send the request
    broker->submitMessage(remote_host.c_str(), data_pack, NULL, NULL, 0, on_this_thread);
}

/*
 */
CDataWrapper* MessageChannel::sendRequest(const std::string& remote_host,
                                          const std::string& node_id,
                                          const std::string& action_name,
                                          CDataWrapper *request_pack,
                                          int32_t millisec_to_wait,
                                          bool async,
                                          bool on_this_thread) {
    CHAOS_ASSERT(broker)    auto_ptr<MessageRequestFuture> request_future(sendRequestWithFuture(remote_host,
                                                                                                node_id,
                                                                                                action_name,
                                                                                                request_pack));
    
    
    
    //wait for some time or, if == 0, forever
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
    auto_ptr<MessageRequestFuture> result;
    CDataWrapper *data_pack = new CDataWrapper();
    
    //lock lk(waith_asnwer_mutex);
    if(request_pack)data_pack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *request_pack);
    atomic_int_type current_request_id = atomic_increment(&channelRequestIDCounter);
    
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, node_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, action_name);
    
    //add current server as
    data_pack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID, current_request_id);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN, channel_reponse_domain);
    data_pack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION, "response");
    //prepare the promises and future
    boost::shared_ptr<boost::promise<common::data::CDataWrapper*> > promises_ptr(new boost::promise<common::data::CDataWrapper*>());
    
    //lock the map
    boost::shared_lock< boost::shared_mutex > lock(mutext_answer_managment);
    //insert into themap the promises
    map_request_id_promises.insert(make_pair(current_request_id, promises_ptr));
    //get the future
    result.reset(new  MessageRequestFuture(current_request_id,
                                           promises_ptr->get_future()));
    //if(async) return result;
    //submit the request
    broker->submiteRequest(remote_host, data_pack, NULL, NULL, 0, true);
    return result;
}

//! get the rpc published host and port
void MessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    return broker->getPublishedHostAndPort(rpc_published_host_port);
}
