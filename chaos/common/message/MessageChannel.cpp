//
//  MessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MessageChannel.h"
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;

MessageChannel::MessageChannel(MessageBroker *_broker, string& _remoteHost):broker(_broker), remoteHost(_remoteHost),waith_asnwer_mutex(),waith_asnwer_condition() {
}

MessageChannel::~MessageChannel() {
    
}

/*!
  Initialization phase
 */
void MessageChannel::init() throw(CException) {
    //create a new channel id
    channelID = UUIDUtil::generateUUIDLite();

    
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
CDataWrapper *MessageChannel::response(CDataWrapper *responseData) {
    if(!responseData->hasKey(RpcActionDefinitionKey::CS_CMDM_RESPONSE_ID)) return NULL;
    lock lk(waith_asnwer_mutex);
    atomic_int_type requestID = responseData->getInt32Value(RpcActionDefinitionKey::CS_CMDM_RESPONSE_ID);
    
    //call the handler
    if(responsIdHandlerMap.count(requestID)>0){
        responsIdHandlerMap[requestID](responseData);
    }else{

        //put the result into the sync map result
        responseIdSyncMap.insert(make_pair(requestID, responseData));
        waith_asnwer_condition.notify_one();
    }
    return NULL;
}

/*
 */
void MessageChannel::prepareReqeustPack(CDataWrapper *requestPack, boost::function<void(CDataWrapper*)>& handler) {
    
    atomic_int_type requestID = prepareReqeustPack(requestPack);
    
    //register the handler
    responsIdHandlerMap.insert(make_pair(requestID, handler));
}

/*!
 Set the reqeust id into the CDataWrapper
 */
inline atomic_int_type MessageChannel::prepareReqeustPack(CDataWrapper *requestPack) {
    CHAOS_ASSERT(requestPack)
    //get new reqeust id
    atomic_int_type curentRequestID = atomic_increment(&channelRequestIDCounter);
    
    //add current server as
    requestPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_RESPONSE_ID, curentRequestID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_RESPONSE_DOMAIN, channelID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_RESPONSE_ACTION, "response");
    
    return curentRequestID;
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
    broker->submitMessage(remoteHost, messagePack);
}

/*! 
 called when a result of an 
 */
void MessageChannel::sendRequest(const char * const nodeID, const char * const actionName, CDataWrapper *requestPack, boost::function<void(CDataWrapper*)> handler) {
    CHAOS_ASSERT(nodeID && actionName && requestPack)
    
    //add the action and dommain name
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);
    
    //get atomicaly request id
    if(!handler.empty())
        prepareReqeustPack(requestPack, handler);
    
    //send the request
    broker->submiteRequest(remoteHost, requestPack);
    
}

CDataWrapper* MessageChannel::sendRequest(const char * const nodeID, const char * const actionName, CDataWrapper *requestPack) {
    CHAOS_ASSERT(nodeID && actionName && requestPack)
    CDataWrapper *result = NULL;
    lock lk(waith_asnwer_mutex);
    atomic_int_type currentRequestID = prepareReqeustPack(requestPack);
    
    //add the action and dommain name
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, nodeID);
    requestPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionName);
    
    //send the request
    broker->submiteRequest(remoteHost, requestPack);

    //waith the answer
    waith_asnwer_condition.wait(lk);
    
    //get the result
    result = responseIdSyncMap[currentRequestID];
    
    
    //delete the key
    responseIdSyncMap.erase(currentRequestID);
    
    lk.unlock();
    return result;
}