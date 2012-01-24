//
//  LLRpcApi.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 30/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "LLRpcApi.h"
#include "../../common/data/CDataWrapper.h"

using namespace boost;

using namespace chaos;
using namespace chaos::ui;

/*
 LL Rpc Api static initialization it should be called once for application
 */
void LLRpcApi::init()  throw (CException) {
    LAPP_ << "Init LLRpcApi";
        
    LAPP_ << "Init MessageBroker";
    rpcMessageBroker->init();
    LAPP_ << "MessageBroker Initialized";
    LAPP_ << "Starting MessageBroker";
    rpcMessageBroker->start();
    LAPP_ << "MessageBroker Started";
}

/*
 Deinitialization of LL rpc api
 */
void LLRpcApi::deinit()  throw (CException) {
    LAPP_ << "Deinit LLRpcApi";
    
    LAPP_ << "Deinit MessageBroker";
    rpcMessageBroker->deinit();
    LAPP_ << "MessageBroker deinitialized";
}

/*
 */
LLRpcApi::LLRpcApi() {
    rpcMessageBroker = new MessageBroker();
}

/*
 */
LLRpcApi::~LLRpcApi() {
    if(rpcMessageBroker)
        delete (rpcMessageBroker);
}

/*
 */
MessageChannel *LLRpcApi::getNewMetadataServerChannel() {
    string serverHost = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    return rpcMessageBroker->getNewMessageChannelForremoteHost(serverHost, MessageBroker::NORMAL);
}