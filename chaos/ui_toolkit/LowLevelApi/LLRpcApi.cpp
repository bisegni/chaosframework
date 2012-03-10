/*	
 *	LLRpcApi.cpp
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
    return rpcMessageBroker->getMetadataserverMessageChannel(serverHost);
}

/*!
 Return a new device channel
 */
DeviceMessageChannel *LLRpcApi::getNewDeviceMessageChannel(CDeviceNetworkAddress *deviceNetworkAddress) {
    return rpcMessageBroker->getDeviceMessageChannelFromAddress(deviceNetworkAddress);
}

void LLRpcApi::deleteMessageChannel(NetworkAddressMessageChannel *channelToDispose) {
    rpcMessageBroker->disposeMessageChannel(channelToDispose);
}
