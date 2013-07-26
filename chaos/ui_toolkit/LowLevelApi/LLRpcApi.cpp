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

#define LLRA_LAPP_ LAPP_ << "[LLRpcApi] - "


#define INIT_STEP   0
#define DEINIT_STEP 1
/*
 LL Rpc Api static initialization it should be called once for application
 */
void LLRpcApi::init()  throw (CException) {
    LLRA_LAPP_ << "Init";
    SetupStateManager::levelUpFrom(INIT_STEP, "LLRpcApi already initialized");
    
    LLRA_LAPP_ << "Init NetworkBroker";
    rpcMessageBroker->init();
    LLRA_LAPP_ << "NetworkBroker Initialized";
    LLRA_LAPP_ << "Starting NetworkBroker";
    rpcMessageBroker->start();
    LLRA_LAPP_ << "NetworkBroker Started";
}

/*
 Deinitialization of LL rpc api
 */
void LLRpcApi::deinit()  throw (CException) {
    LLRA_LAPP_ << "Deinit LLRpcApi";
    SetupStateManager::levelDownFrom(DEINIT_STEP, "LLRpcApi already deinitialized");
    LLRA_LAPP_ << "Stop NetworkBroker";
    rpcMessageBroker->stop();
    LLRA_LAPP_ << "Deinit NetworkBroker";
    rpcMessageBroker->deinit();
    LLRA_LAPP_ << "NetworkBroker deinitialized";
}

/*
 */
LLRpcApi::LLRpcApi() {
    rpcMessageBroker = new NetworkBroker();
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

void LLRpcApi::deleteMessageChannel(NodeMessageChannel *channelToDispose) {
    rpcMessageBroker->disposeMessageChannel(channelToDispose);
}


event::channel::AlertEventChannel *LLRpcApi::getNewAlertEventChannel() throw (CException) {
    return rpcMessageBroker->getNewAlertEventChannel();
}

event::channel::InstrumentEventChannel *LLRpcApi::getNewInstrumentEventChannel() throw (CException) {
    return rpcMessageBroker->getNewInstrumentEventChannel();
}

void LLRpcApi::disposeEventChannel(event::channel::EventChannel *eventChannel) throw (CException) {
    rpcMessageBroker->disposeEventChannel(eventChannel);
}
