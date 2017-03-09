/*	
 *	LLRpcApi.cpp
 *	!CHAOS
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
using namespace chaos::common::io;
using namespace chaos::common::event;
using namespace chaos::common::message;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
#define LLRA_LAPP_ LAPP_ << "[LLRpcApi] - "

uint32_t DIOConn::garbage_counter = 0;

/*
 */
LLRpcApi::LLRpcApi() {
	direct_io_client = NULL;
}

/*
 */
LLRpcApi::~LLRpcApi() {
}
void LLRpcApi::init(chaos::common::network::NetworkBroker *nb){
     LLRA_LAPP_ << "Using a specified NetworkBroker:"<<nb;

     direct_io_client = NetworkBroker::getInstance()->getSharedDirectIOClientInstance();
}

/*
 LL Rpc Api static initialization it should be called once for application
 */
void LLRpcApi::init()  throw (CException) {
	
	//get new direct io client
	direct_io_client = NetworkBroker::getInstance()->getSharedDirectIOClientInstance();
}

/*
 Deinitialization of LL rpc api
 */
void LLRpcApi::deinit()  throw (CException) {
    LLRA_LAPP_ << "Deinit LLRpcApi";
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
    return NetworkBroker::getInstance()->getMetadataserverMessageChannel();
}

/*!
 Return a new device channel
 */
DeviceMessageChannel *LLRpcApi::getNewDeviceMessageChannel(CDeviceNetworkAddress *deviceNetworkAddress,
                                                           bool self_managed) {
    return NetworkBroker::getInstance()->getDeviceMessageChannelFromAddress(deviceNetworkAddress,
                                                                            self_managed);
}

/*!
 Return a new device channel
 */
chaos::common::message::PerformanceNodeChannel *LLRpcApi::getNewPerformanceChannel(CNetworkAddress *note_network_address) {
    return NetworkBroker::getInstance()->getPerformanceChannelFromAddress(note_network_address);
}

void LLRpcApi::deleteMessageChannel(MessageChannel *channelToDispose) {
    NetworkBroker::getInstance()->disposeMessageChannel(channelToDispose);
}
/*!
 Delete a previously instantiatedchannel
 */
void LLRpcApi::deleteMessageChannel(NodeMessageChannel *channelToDispose) {
	NetworkBroker::getInstance()->disposeMessageChannel(channelToDispose);
}

void LLRpcApi::deleteMessageChannel(chaos::common::message::MDSMessageChannel *channelToDispose) {
    NetworkBroker::getInstance()->disposeMessageChannel(channelToDispose);
}

chaos::common::event::channel::AlertEventChannel *LLRpcApi::getNewAlertEventChannel() throw (CException) {
    return NetworkBroker::getInstance()->getNewAlertEventChannel();
}

chaos::common::event::channel::InstrumentEventChannel *LLRpcApi::getNewInstrumentEventChannel() throw (CException) {
    return NetworkBroker::getInstance()->getNewInstrumentEventChannel();
}

void LLRpcApi::disposeEventChannel(chaos::common::event::channel::EventChannel *eventChannel) throw (CException) {
    NetworkBroker::getInstance()->disposeEventChannel(eventChannel);
}

SystemApiChannel *LLRpcApi::getSystemApiClientChannel(const std::string& direct_io_address) {
	DIOConn *conn = NULL;
	boost::unique_lock<boost::mutex> l(mutex_map_dio_addr_conn);
	if(map_dio_addr_conn.count(direct_io_address)) {
		conn = map_dio_addr_conn[direct_io_address];
	} else {
		conn = new DIOConn(direct_io_client->getNewConnection(direct_io_address));
	}
	conn->garbage_counter++;
	return new SystemApiChannel(conn, (chaos::common::direct_io::channel::DirectIOSystemAPIClientChannel*)conn->connection->getNewChannelInstance("DirectIOSystemAPIClientChannel"));
}

void LLRpcApi::releaseSystemApyChannel(SystemApiChannel *system_api_channel) {
	boost::unique_lock<boost::mutex> l(mutex_map_dio_addr_conn);
	if(!system_api_channel) return;
	
	DIOConn *conn = system_api_channel->connection;
	if(conn) {
		conn->garbage_counter--;
		if(system_api_channel->system_api_channel) {
			conn->connection->releaseChannelInstance(system_api_channel->system_api_channel);
		}
		
		if(!conn->garbage_counter) {
			//need to be deleted the connection from the map
			map_dio_addr_conn.erase(conn->connection->getURL());
			
			//and form the root client
			direct_io_client->releaseConnection(conn->connection);
		}
	}
	
}
