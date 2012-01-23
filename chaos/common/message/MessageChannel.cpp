//
//  MessageChannel.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MessageChannel.h"

using namespace chaos;

MessageChannel::MessageChannel(MessageBroker *parentBroker, string& _remoteHost) {
    remoteHost = _remoteHost;
    localChannelID = UUIDUtil::generateUUIDLite();
}

MessageChannel::~MessageChannel() {
    
}


/*! 
 \brief called when a result of an 
 */
void MessageChannel::response(CDataWrapper *responseData) {
    
}


/*! 
 \brief called when a result of an 
 */
void MessageChannel::sendMessage(CDataWrapper *messageData) {
    

}

/*! 
 \brief called when a result of an 
 */
void MessageChannel::sendRequest(CDataWrapper *requestData, boost::function<void(CDataWrapper*)> handler) {
    CHAOS_ASSERT(requestData)
    
    //get atomicaly request id
    atomic_int_type requestID = atomic_increment(&channelRequestID);
    
    //add answer id to datawrapper
    requestData->addInt32Value(RpcActionDefinitionKey::CS_CMDM_RESPONSE_ID, requestID);
}