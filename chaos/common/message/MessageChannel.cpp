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

#define MessageChannel_LOG_HEAD "[MessageChannel] - "

#define MCAPP_ LAPP_ << MessageChannel_LOG_HEAD
#define MCDBG_ LDBG_ << MessageChannel_LOG_HEAD << __FUNCTION__ << " - "
#define MCERR_ LERR_ << MessageChannel_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "


MessageChannel::MessageChannel(NetworkBroker *_broker,
							   const std::string& _remote_host):
broker(_broker),
response_handler(NULL) {
    //create the network address
    remoteNodeAddress = _remote_host;
}

MessageChannel::MessageChannel(NetworkBroker *_broker):
broker(_broker){

}

MessageChannel::~MessageChannel() {
	for(map<atomic_int_type, common::data::CDataWrapper* >::iterator it =  response_id_sync_map.begin();
		it != response_id_sync_map.end();
		it++){
		MCAPP_ << "Cleanup unforwarded message responf for id " << it->first;
		delete(it->second);
		
	}
}

/*!
  Initialization phase
 */
void MessageChannel::init() throw(CException) {
    //reset request id
    channelRequestIDCounter= 0;
    //create a new channel id
    channelID = UUIDUtil::generateUUIDLite();

        //register the action for the response
    DeclareAction::addActionDescritionInstance<MessageChannel>(this, 
                                                               &MessageChannel::response, 
                                                               channelID.c_str(), 
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
CDataWrapper *MessageChannel::response(CDataWrapper *responseData, bool& detachData) {
    if(!responseData->hasKey(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID)) return NULL;
    atomic_int_type requestID = 0;
    try {
        
        boost::shared_lock< boost::shared_mutex > lock(mutext_answer_managment);
        
            //lock lk(waith_asnwer_mutex);
        atomic_int_type requestID = responseData->getInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID);
            //check if the requester is waith the answer
        if(!(detachData = sem.setWaithedObjectForKey(requestID, responseData))){
            //call the handler
            if((detachData = response_handler)) {
                response_handler(requestID, responseData);
            } else {
                LDBG_ << "No one has consumed the response of id " << requestID;
            }
        }
    } catch (...) {
        LDBG_ << "An error occuring dispatching the response:" << requestID;
    }
    return NULL;
}

/*
 */
chaos::atomic_int_type MessageChannel::prepareRequestPackAndSend(bool async, const char * const nodeID, const char * const actionName, CDataWrapper *requestPack, bool onThisThread) {
    CHAOS_ASSERT(nodeID && actionName && requestPack)
        //get new reqeust id
    atomic_int_type currentRequestID = atomic_increment(&channelRequestIDCounter);

    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);

    //register the handler
    if(!async) {
        sem.initKey(currentRequestID);
    }
    
        //add current server as
    requestPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID, currentRequestID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN, channelID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION, "response");
    
    //the message need to be put in a subobject with the current key: CS_CMDM_ACTION_MESSAGE
    
    broker->submiteRequest(remoteNodeAddress, requestPack, NULL, NULL, 0, onThisThread);
    
    return currentRequestID;
}

/*!
 Update the address of the channel
 */
void MessageChannel::setRemoteNodeAddress(const std::string& remote_addr) {
	remoteNodeAddress = remote_addr;
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
void MessageChannel::sendMessage(const char * const nodeID,const char * const actionName, CDataWrapper * const messagePack,  bool onThisThread) {
    CHAOS_ASSERT(nodeID && actionName)
    CDataWrapper *dataPack = new CDataWrapper();
    //add the action and dommain name
    dataPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    dataPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);
    if(messagePack)dataPack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *messagePack);
    //send the request
    broker->submitMessage(remoteNodeAddress, dataPack, NULL, NULL, 0, onThisThread);
}

/*
 */
CDataWrapper* MessageChannel::sendRequest(const char * const nodeID, const char * const actionName, CDataWrapper * const requestPack, uint32_t millisecToWait, bool async,  bool onThisThread) {
    CHAOS_ASSERT(nodeID && actionName)
    CDataWrapper *result = NULL;
    CDataWrapper *dataPack = new CDataWrapper();
    //lock lk(waith_asnwer_mutex);
    if(requestPack)dataPack->addCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE, *requestPack);
    atomic_int_type currentRequestID =  prepareRequestPackAndSend(async, nodeID, actionName, dataPack, onThisThread);
    
    //waith the answer
    //waith_asnwer_condition.wait(lk);
    if(async) return result;
    
    if(millisecToWait)
        result = sem.wait(currentRequestID, millisecToWait);
    else
        result = sem.wait(currentRequestID);
    
    return result;
}

/*! 
 called when a result of an 
 */
void MessageChannel::setHandler(MessageHandler handler) {
    boost::unique_lock< boost::shared_mutex > uniqueLock(mutext_answer_managment);
    response_handler = handler;
}

/*!
 Remove request id
 */
void MessageChannel::clearHandler() {
    boost::unique_lock< boost::shared_mutex > uniqueLock(mutext_answer_managment);
    response_handler = NULL;
}


/*!
 Poll for see if the response is arrived
 */
CDataWrapper* MessageChannel::pollAnswer(atomic_int_type request_id, uint32_t millisec_to_wait) {
    return sem.wait(request_id, millisec_to_wait);
}
