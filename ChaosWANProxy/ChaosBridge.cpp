/*
 *	ChaosBridge.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "ChaosBridge.h"

#include <chaos/common/network/URL.h>

#define CB_LOG_HEAD "[ChaosBridge] - "

#define CB_LAPP LAPP_ << CB_LOG_HEAD
#define CB_LDBG LDBG_ << CB_LOG_HEAD << __PRETTY_FUNCTION__
#define CB_LERR LERR_ << CB_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") "


using namespace chaos::wan_proxy;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ChaosBridge::ChaosBridge(chaos::common::direct_io::DirectIOClient	*_direct_io_client):
direct_io_client(_direct_io_client),
connection_feeder("ChaosBridge", this) {
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ChaosBridge::~ChaosBridge() {
	
}

void ChaosBridge::init(void *init_data) throw (chaos::CException) {
	
	InizializableService::initImplementation(direct_io_client,
											 init_data,
											 direct_io_client->getName(),
											 __PRETTY_FUNCTION__);
}

void ChaosBridge::deinit() throw (chaos::CException) {
	
	connection_feeder.clear();
	
	if(direct_io_client) {
		CHAOS_NOT_THROW(InizializableService::deinitImplementation(direct_io_client,
																   direct_io_client->getName(),
																   __PRETTY_FUNCTION__);)
		delete(direct_io_client);
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ChaosBridge::clear() {
	connection_feeder.clear();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ChaosBridge::addServerList(const std::vector<std::string>& _cds_address_list) {
	//checkif someone has passed us the device indetification
	CB_LAPP << "Scan the directio address";
	
	for (std::vector<std::string>::const_iterator it = _cds_address_list.begin();
		 it != _cds_address_list.end();
		 it++ ){
		if(!common::direct_io::DirectIOClient::checkURL(*it)) {
			CB_LDBG << "Data proxy server description " << *it << " non well formed";
			continue;
		}
		//add new url to connection feeder
		connection_feeder.addURL(chaos::common::network::URL(*it));
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ChaosBridge::disposeService(void *service_ptr) {
	if(!service_ptr) return;
	DirectIOChannelsInfo	*info = static_cast<DirectIOChannelsInfo*>(service_ptr);
	
	if(info->device_client_channel) info->connection->releaseChannelInstance(info->device_client_channel);
	direct_io_client->releaseConnection(info->connection);
	delete(info);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void* ChaosBridge::serviceForURL(const common::network::URL& url, uint32_t service_index) {
	CB_LDBG << "Add connection for " << url.getURL();
	DirectIOChannelsInfo * clients_channel = NULL;
	chaos_direct_io::DirectIOClientConnection *tmp_connection = direct_io_client->getNewConnection(url.getURL());
	if(tmp_connection) {
		clients_channel = new DirectIOChannelsInfo();
		clients_channel->connection = tmp_connection;
		
		//allocate the client channel
		clients_channel->device_client_channel = (DirectIODeviceClientChannel*)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
		if(!clients_channel->device_client_channel) {
			CB_LDBG << "Error creating client device channel for " << url.getURL();
			
			//release conenction
			direct_io_client->releaseConnection(tmp_connection);
			
			//relase struct
			delete(clients_channel);
			return NULL;
		}
		
		//set this driver instance as event handler for connection
		clients_channel->connection->setEventHandler(this);
		//!put the index in the conenction so we can found it wen we receive event from it
		clients_channel->connection->setCustomStringIdentification(boost::lexical_cast<std::string>(service_index));
	} else {
		CB_LERR << "Error creating client connection for " << url.getURL();
	}
	return clients_channel;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ChaosBridge::handleEvent(DirectIOClientConnection *client_connection,
							  DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//if the channel has bee disconnected turn the relative index offline, if onli reput it online
	boost::unique_lock<boost::shared_mutex>(mutext_feeder);
	uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
	DEBUG_CODE(CB_LDBG << "Manage event for service with index " << service_index << " and url" << client_connection->getURL();)
	switch(event) {
		case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
			connection_feeder.setURLOnline(service_index);
			break;
			
		case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
			connection_feeder.setURLOffline(service_index);
			break;
	}
}

// push a dataset
void ChaosBridge::pushDataset(const std::string& producer_key,
							  CDataWrapper *dataset,
							  int store_hint) {
	CHAOS_ASSERT(dataset)
	
	auto_ptr<SerializationBuffer> serialization(dataset->getBSONData());
	delete(dataset);
	
	DirectIOChannelsInfo	*next_client = static_cast<DirectIOChannelsInfo*>(connection_feeder.getService());
	serialization->disposeOnDelete = !next_client;
	if(next_client) {
		boost::shared_lock<boost::shared_mutex>(next_client->connection_mutex);
		
		//free the packet
		serialization->disposeOnDelete = false;
		next_client->device_client_channel->storeAndCacheDataOutputChannel(producer_key,
																		   (void*)serialization->getBufferPtr(),
																		   (uint32_t)serialization->getBufferLen(),
																		   (DirectIODeviceClientChannelPutMode)store_hint);
	} else {
		DEBUG_CODE(CB_LDBG << "No available socket->loose packet");
	}
}

// get a dataset
CDataWrapper *ChaosBridge::getDataset(const std::string& producer_key) {
	uint32_t size = 0;
	char* result = NULL;
	DirectIOChannelsInfo	*next_client = static_cast<DirectIOChannelsInfo*>(connection_feeder.getService());
	if(!next_client) return NULL;
	
	boost::shared_lock<boost::shared_mutex>(next_client->connection_mutex);
	
	next_client->device_client_channel->requestLastOutputData(producer_key, (void**)&result, size);
	return new CDataWrapper(result);
}