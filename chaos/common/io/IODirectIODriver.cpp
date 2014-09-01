/*
 *	IODirectIODriver.cpp
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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>


#define LMEMDRIVER_ LAPP_ << "[Memcached IO Driver] - "


#define IODirectIODriver_LOG_HEAD "[IODirectIODriver] - "

#define IODirectIODriver_LAPP_ LAPP_ << IODirectIODriver_LOG_HEAD
#define IODirectIODriver_DLDBG_ LDBG_ << IODirectIODriver_LOG_HEAD
#define IODirectIODriver_LERR_ LERR_ << IODirectIODriver_LOG_HEAD

using namespace chaos;
using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace chaos_direct_io = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

//using namespace memcache;

/*
 * Driver constructor
 */
IODirectIODriver::IODirectIODriver(std::string alias):NamedService(alias), current_endpoint_p_port(0), current_endpoint_s_port(0), current_endpoint_index(0), connectionFeeder(alias, this) {
	//clear
	std::memset(&init_parameter, 0, sizeof(IODirectIODriverInitParam));
	
	device_server_channel = NULL;
	
	read_write_index = 0;
	data_cache.data_ptr = NULL;
	data_cache.data_len = 0;
}

/*
 * Driver distructor
 */
IODirectIODriver::~IODirectIODriver() {
}

void IODirectIODriver::setDirectIOParam(IODirectIODriverInitParam& _init_parameter) {
	//store the configuration
	init_parameter = _init_parameter;
}

/*
 * Init method, the has map has all received value for configuration
 * every implemented driver need to get all needed configuration param
 */
void IODirectIODriver::init(void *_init_parameter) throw(CException) {
	IODataDriver::init(_init_parameter);
	
	IODirectIODriver_LAPP_ << "Check init parameter";
	
	if(!init_parameter.network_broker) throw CException(-1, "No network broker configured", __PRETTY_FUNCTION__);
	
	init_parameter.client_instance = init_parameter.network_broker->getDirectIOClientInstance();
	if(!init_parameter.client_instance) throw CException(-1, "No client configured", __PRETTY_FUNCTION__);
	
	init_parameter.endpoint_instance = init_parameter.network_broker->getDirectIOServerEndpoint();
	if(!init_parameter.endpoint_instance) throw CException(-1, "No endpoint configured", __PRETTY_FUNCTION__);
	
	//initialize client
	utility::InizializableService::initImplementation(init_parameter.client_instance, _init_parameter, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
	
	//get the client and server channel
	IODirectIODriver_LAPP_ << "Allcoate the default device server channel";
	device_server_channel = (chaos_dio_channel::DirectIODeviceServerChannel *)init_parameter.endpoint_instance->getNewChannelInstance("DirectIODeviceServerChannel");
	device_server_channel->setHandler(this);
	
	//store endpoint idnex for fast access
	current_endpoint_p_port = init_parameter.endpoint_instance->getPublicServerInterface()->getPriorityPort();
	current_endpoint_s_port = init_parameter.endpoint_instance->getPublicServerInterface()->getServicePort();
	current_endpoint_index = init_parameter.endpoint_instance->getRouteIndex();
	IODirectIODriver_LAPP_ << "Our receiving priority port is " << current_endpoint_p_port << " and enpoint is " <<current_endpoint_index;
	
}

/*
 * Deinitialization of memcached driver
 */
void IODirectIODriver::deinit() throw(CException) {
	if(data_cache.data_ptr) {
		IODirectIODriver_LAPP_ << "delete last data received";
		free(data_cache.data_ptr);
	}
	
	//remove all url and service
	IODirectIODriver_LAPP_ << "Remove all urls";
	connectionFeeder.clear();
	
	//initialize client
	utility::InizializableService::deinitImplementation(init_parameter.client_instance, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
	delete(init_parameter.client_instance);
	
	//deinitialize server channel
	if(device_server_channel) {
		init_parameter.endpoint_instance->releaseChannelInstance(device_server_channel);
	}
	
	if(init_parameter.endpoint_instance) {
		init_parameter.network_broker->releaseDirectIOServerEndpoint(init_parameter.endpoint_instance);
	}
	IODataDriver::deinit();
}

/*
 * This method retrive the cached object by CSDawrapperUsed as query key and
 * return a pointer to the class ArrayPointer of CDataWrapper type
 */
void IODirectIODriver::storeRawData(chaos::common::data::SerializationBuffer *serialization)  throw(CException) {
	CHAOS_ASSERT(serialization)
	boost::shared_lock<boost::shared_mutex>(mutext_feeder);
	//if(next_client->connection->getState() == chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected)
	IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
	serialization->disposeOnDelete = !next_client;
	if(next_client) {
		//free the packet
		next_client->device_client_channel->storeAndCacheDataOutputChannel((void*)serialization->getBufferPtr(), (uint32_t)serialization->getBufferLen());
		return;
	} else {
		DEBUG_CODE(IODirectIODriver_DLDBG_ << "No available socket->loose packet");
	}
	delete(serialization);
}

/*
 * This method retrive the cached object by CSDawrapperUsed as query key and
 * return a pointer to the class ArrayPointer of CDataWrapper type
 */
char* IODirectIODriver::retriveRawData(size_t *dim)  throw(CException) {
	char* result = NULL;
	boost::shared_lock<boost::shared_mutex>(mutext_feeder);
	IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
	if(!next_client) return NULL;
	
	uint32_t size =0;
	next_client->device_client_channel->requestLastOutputData((void**)&result, size);
	*dim = (size_t)size;
	return result;
}

//we have request data and this arrive with the put opcode
int IODirectIODriver::consumePutEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
									   void *channel_data,
									   uint32_t channel_data_len,
									   common::direct_io::DirectIOSynchronousAnswerPtr synchronous_answer) {
	delete(header);
	if(channel_data)free(channel_data);
	return 0;
}

int IODirectIODriver::consumeGetEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
									   void *channel_data,
									   uint32_t channel_data_len,
									   common::direct_io::DirectIOSynchronousAnswerPtr synchronous_answer) {
	delete(header);
	if(channel_data)free(channel_data);
	
	return 0;
}

/*
 Update the driver configuration
 */
chaos::common::data::CDataWrapper* IODirectIODriver::updateConfiguration(chaos::common::data::CDataWrapper* newConfigration) {
	//lock the feeder access
	boost::unique_lock<boost::shared_mutex>(mutext_feeder);
	//checkif someone has passed us the device indetification
	if(newConfigration->hasKey(DatasetDefinitionkey::DEVICE_ID)){
		dataKey = newConfigration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
		IODirectIODriver_LAPP_ << "The key for memory cache is: " << dataKey;
	}
	if(newConfigration->hasKey(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS)){
		IODirectIODriver_LAPP_ << "Get the DataManager LiveData address value";
		auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> liveMemAddrConfig(newConfigration->getVectorValue(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS));
		size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
		for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
			string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
			if(!common::direct_io::DirectIOClient::checkURL(serverDesc)) {
				IODirectIODriver_DLDBG_ << "Data proxy server description " << serverDesc << " non well formed";
				continue;
			}
			//add new url to connection feeder
			connectionFeeder.addURL(chaos::common::network::URL(serverDesc));
		}
	}
	return NULL;
}

void IODirectIODriver::disposeService(void *service_ptr) {
	if(!service_ptr) return;
	IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(service_ptr);
	next_client->connection->releaseChannelInstance(next_client->device_client_channel);
	init_parameter.client_instance->releaseConnection(next_client->connection);
	delete(next_client);
}
void* IODirectIODriver::serviceForURL(const common::network::URL& url, uint32_t service_index) {
	IODirectIODriver_DLDBG_ << "Add connection for " << url.getURL();
	IODirectIODriverClientChannels * clients_channel = NULL;
	chaos_direct_io::DirectIOClientConnection *tmp_connection = init_parameter.client_instance->getNewConnection(url.getURL());
	if(tmp_connection) {
		clients_channel = new IODirectIODriverClientChannels();
		clients_channel->connection = tmp_connection;
		clients_channel->device_client_channel = (chaos_dio_channel::DirectIODeviceClientChannel *)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
		if(!clients_channel->device_client_channel) {
			IODirectIODriver_DLDBG_ << "Error creating client device channel for " << url.getURL();
			init_parameter.client_instance->releaseConnection(tmp_connection);
			delete(clients_channel);
			return NULL;
		}
		//set device id
		clients_channel->device_client_channel->setDeviceID(dataKey, common::direct_io::channel::DirectIODeviceClientChannelPutModeStoricizeAnLive);
		
		//set the answer information
		clients_channel->device_client_channel->setAnswerServerInfo(current_endpoint_p_port, current_endpoint_s_port, current_endpoint_index);
		
		//set this driver instance as event handler for connection
		clients_channel->connection->setEventHandler(this);
		clients_channel->connection->setCustomStringIdentification(boost::lexical_cast<std::string>(service_index));
	} else {
		IODirectIODriver_DLDBG_ << "Error creating client connection for " << url.getURL();
	}
	return clients_channel;
}

void IODirectIODriver::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
										   chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//if the channel has bee disconnected turn the relative index offline, if onli reput it online
	boost::unique_lock<boost::shared_mutex>(mutext_feeder);
	uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
	DEBUG_CODE(IODirectIODriver_DLDBG_ << "Manage event for service with index " << service_index << " and url" << client_connection->getURL();)
	switch(event) {
		case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
			connectionFeeder.setURLOnline(service_index);
			break;
			
		case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
			connectionFeeder.setURLOffline(service_index);
			break;
	}
}