//
//  LLRpcApi.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 30/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "LLRpcApi.h"
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/global.h>

using namespace boost;

using namespace chaos;
using namespace chaos::ui;

#define INIT_STEP   0
#define DEINIT_STEP 1
/*
 LL Rpc Api static initialization it should be called once for application
 */
void LLRpcApi::init()  throw (CException) {
    LAPP_ << "Init LLRpcApi";
    SetupStateManager::levelUpFrom(INIT_STEP, "LLRpcApi already initialized");
    
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
    SetupStateManager::levelDownFrom(DEINIT_STEP, "LLRpcApi already deinitialized");
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
MDSMessageChannel *LLRpcApi::getNewMetadataServerChannel() {
    string serverHost = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    return static_cast<MDSMessageChannel*>(rpcMessageBroker->getMetadataserverMessageChannel(serverHost));
}