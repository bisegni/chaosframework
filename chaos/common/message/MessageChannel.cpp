//
//  MessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MessageChannel.h"
#include <chaos/common/message/MessageBroker.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;

MessageChannel::MessageChannel(MessageBroker *_broker, const char*const _remoteHost):broker(_broker){
    //create the network address
    remoteNodeAddress= _remoteHost;
}

MessageChannel::MessageChannel(MessageBroker *_broker):broker(_broker){

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
 called when a result of an 
 */
CDataWrapper *MessageChannel::response(CDataWrapper *responseData, bool& detachData) {
    if(!responseData->hasKey(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID)) return NULL;
    atomic_int_type requestID = 0;
    try {
        detachData = true;
        
            //lock lk(waith_asnwer_mutex);
        atomic_int_type requestID = responseData->getInt32Value(RpcActionDefinitionKey::CS_CMDM_MESSAGE_ID);
        LDBG_ << "new requestd id arrived:" << requestID;
            //call the handler
        if(responsIdHandlerMap.count(requestID)>0){
            responsIdHandlerMap[requestID](responseData);
        }else{
            sem.setWaithedObjectForKey(requestID, responseData);
        }
    } catch (...) {
        LDBG_ << "An error occuring dispatching the response:" << requestID;
    }
    return NULL;
}

/*
 */
atomic_int_type MessageChannel::prepareRequestPackAndSend(const char * const nodeID, const char * const actionName, CDataWrapper *requestPack, boost::function<void(CDataWrapper*)> *handler) {
    CHAOS_ASSERT(nodeID && actionName && requestPack)
        //get new reqeust id
    atomic_int_type currentRequestID = atomic_increment(&channelRequestIDCounter);
    LDBG_ << "new requestd id to send:" << currentRequestID;

    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);

    //register the handler
    if(handler && !handler->empty())
        responsIdHandlerMap.insert(make_pair(currentRequestID, *handler));
    else
        sem.initKey(currentRequestID);
    
        //add current server as
    requestPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ANSWER_ID, currentRequestID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_DOMAIN, channelID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_ACTION, "response");
    
    broker->submiteRequest(remoteNodeAddress, requestPack);
    
    return currentRequestID;
}

/*! 
 called when a result of an 
 */
void MessageChannel::sendMessage(const char * const nodeID,const char * const actionName, CDataWrapper *messagePack) {
    CHAOS_ASSERT(nodeID && actionName && messagePack)
    
    //add the action and dommain name
    messagePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    messagePack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);
    
    //send the request
    broker->submitMessage(remoteNodeAddress, messagePack);
}

/*! 
 called when a result of an 
 */
void MessageChannel::sendRequest(const char * const nodeID, const char * const actionName, CDataWrapper *requestPack, boost::function<void(CDataWrapper*)> handler) {
    prepareRequestPackAndSend(nodeID, actionName, requestPack, &handler);
}

/*
 */
CDataWrapper* MessageChannel::sendRequest(const char * const nodeID, const char * const actionName, CDataWrapper *requestPack, unsigned int millisecToWait) {
    CHAOS_ASSERT(nodeID && actionName && requestPack)
    CDataWrapper *result = NULL;
    //lock lk(waith_asnwer_mutex);
    atomic_int_type currentRequestID =  prepareRequestPackAndSend(nodeID, actionName, requestPack, NULL);

    //waith the answer
    //waith_asnwer_condition.wait(lk);
    if(millisecToWait)
        result = sem.wait(currentRequestID, millisecToWait);
    else
        result = sem.wait(currentRequestID);

    return result;
}