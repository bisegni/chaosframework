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
#include <boost/format.hpp>
#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>

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
    network_broker->init();
    LLRA_LAPP_ << "NetworkBroker Initialized";
    LLRA_LAPP_ << "Starting NetworkBroker";
    network_broker->start();
    LLRA_LAPP_ << "NetworkBroker Started";
}

/*
 Deinitialization of LL rpc api
 */
void LLRpcApi::deinit()  throw (CException) {
    LLRA_LAPP_ << "Deinit LLRpcApi";
    SetupStateManager::levelDownFrom(DEINIT_STEP, "LLRpcApi already deinitialized");
    LLRA_LAPP_ << "Stop NetworkBroker";
    network_broker->stop();
    LLRA_LAPP_ << "Deinit NetworkBroker";
    network_broker->deinit();
    LLRA_LAPP_ << "NetworkBroker deinitialized";
}

/*
 */
LLRpcApi::LLRpcApi() {
    network_broker = new NetworkBroker();
}

/*
 */
LLRpcApi::~LLRpcApi() {
    if(network_broker)
        delete (network_broker);
}

/*
 * Return an instance for the configured data live driver
 */
IODataDriver *LLRpcApi::getDataProxyChannelNewInstance() throw(CException) {
	IODataDriver *result = NULL;
	std::string impl_name =  boost::str( boost::format("%1%IODriver") % GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
	
	result = ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(impl_name);
	if(result) {
		if(impl_name.compare("IODirectIODriver") == 0) {
			//set the information
			IODirectIODriverInitParam init_param;
			std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
			//get client and endpoint
			init_param.network_broker = network_broker;
			init_param.client_instance = NULL;
			init_param.endpoint_instance = NULL;
			((IODirectIODriver*)result)->setDirectIOParam(init_param);
		}
	}
    return result;
}

/*
 */
MDSMessageChannel *LLRpcApi::getNewMetadataServerChannel() {
    string serverHost = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    return network_broker->getMetadataserverMessageChannel(serverHost);
}

/*!
 Return a new device channel
 */
DeviceMessageChannel *LLRpcApi::getNewDeviceMessageChannel(CDeviceNetworkAddress *deviceNetworkAddress) {
    return network_broker->getDeviceMessageChannelFromAddress(deviceNetworkAddress);
}

/*!
 Return a new device channel
 */
chaos::common::message::PerformanceNodeChannel *LLRpcApi::getNewPerformanceChannel(CNetworkAddress *note_network_address) {
    return network_broker->getPerformanceChannelFromAddress(note_network_address);
}

void LLRpcApi::deleteMessageChannel(MessageChannel *channelToDispose) {
    network_broker->disposeMessageChannel(channelToDispose);
}
/*!
 Delete a previously instantiatedchannel
 */
void LLRpcApi::deleteMessageChannel(NodeMessageChannel *channelToDispose) {
	network_broker->disposeMessageChannel(channelToDispose);
}

event::channel::AlertEventChannel *LLRpcApi::getNewAlertEventChannel() throw (CException) {
    return network_broker->getNewAlertEventChannel();
}

event::channel::InstrumentEventChannel *LLRpcApi::getNewInstrumentEventChannel() throw (CException) {
    return network_broker->getNewInstrumentEventChannel();
}

void LLRpcApi::disposeEventChannel(event::channel::EventChannel *eventChannel) throw (CException) {
    network_broker->disposeEventChannel(eventChannel);
}
