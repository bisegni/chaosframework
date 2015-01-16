/*
 *	DefaultPersistenceDriver.cpp
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
#include "DefaultPersistenceDriver.h"

#include <chaos/common/network/URL.h>

#define DPD_LOG_HEAD "[DefaultPersistenceDriver] - "

#define DPD_LAPP LAPP_ << DPD_LOG_HEAD
#define DPD_LDBG LDBG_ << DPD_LOG_HEAD << __PRETTY_FUNCTION__
#define DPD_LERR LERR_ << DPD_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") "


using namespace chaos::wan_proxy::persistence;

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
DefaultPersistenceDriver::DefaultPersistenceDriver(NetworkBroker *_network_broker):
AbstractPersistenceDriver("DefaultPersistenceDriver"),
network_broker(_network_broker),
direct_io_client(NULL),
mds_message_channel(NULL),
connection_feeder("DefaultPersistenceDriver", this) {
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
DefaultPersistenceDriver::~DefaultPersistenceDriver() {
	
}

void DefaultPersistenceDriver::init(void *init_data) throw (chaos::CException) {
	
	//! get the mds message channel
	mds_message_channel = network_broker->getMetadataserverMessageChannel();
	if(!mds_message_channel) throw chaos::CException(-1, "No mds channel found", __PRETTY_FUNCTION__);
		
	//! get the direct io client
	direct_io_client = network_broker->getDirectIOClientInstance();
	InizializableService::initImplementation(direct_io_client,
											 init_data,
											 direct_io_client->getName(),
											 __PRETTY_FUNCTION__);
}

void DefaultPersistenceDriver::deinit() throw (chaos::CException) {
	
	connection_feeder.clear();
	
	if(direct_io_client) {
		CHAOS_NOT_THROW(InizializableService::deinitImplementation(direct_io_client,
																   direct_io_client->getName(),
																   __PRETTY_FUNCTION__);)
		delete(direct_io_client);
	}
	
	if(mds_message_channel) network_broker->disposeMessageChannel(mds_message_channel);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void DefaultPersistenceDriver::clear() {
	connection_feeder.clear();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void DefaultPersistenceDriver::addServerList(const std::vector<std::string>& _cds_address_list) {
	//checkif someone has passed us the device indetification
	DPD_LAPP << "Scan the directio address";
	
	for (std::vector<std::string>::const_iterator it = _cds_address_list.begin();
		 it != _cds_address_list.end();
		 it++ ){
		if(!common::direct_io::DirectIOClient::checkURL(*it)) {
			DPD_LDBG << "Data proxy server description " << *it << " non well formed";
			continue;
		}
		//add new url to connection feeder
		connection_feeder.addURL(chaos::common::network::URL(*it));
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void DefaultPersistenceDriver::disposeService(void *service_ptr) {
	if(!service_ptr) return;
	DirectIOChannelsInfo	*info = static_cast<DirectIOChannelsInfo*>(service_ptr);
	
	if(info->device_client_channel) info->connection->releaseChannelInstance(info->device_client_channel);
	direct_io_client->releaseConnection(info->connection);
	delete(info);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void* DefaultPersistenceDriver::serviceForURL(const common::network::URL& url, uint32_t service_index) {
	DPD_LDBG << "Add connection for " << url.getURL();
	DirectIOChannelsInfo * clients_channel = NULL;
	chaos_direct_io::DirectIOClientConnection *tmp_connection = direct_io_client->getNewConnection(url.getURL());
	if(tmp_connection) {
		clients_channel = new DirectIOChannelsInfo();
		clients_channel->connection = tmp_connection;
		
		//allocate the client channel
		clients_channel->device_client_channel = (DirectIODeviceClientChannel*)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
		if(!clients_channel->device_client_channel) {
			DPD_LDBG << "Error creating client device channel for " << url.getURL();
			
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
		DPD_LERR << "Error creating client connection for " << url.getURL();
	}
	return clients_channel;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void DefaultPersistenceDriver::handleEvent(DirectIOClientConnection *client_connection,
							  DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//if the channel has bee disconnected turn the relative index offline, if onli reput it online
	boost::unique_lock<boost::shared_mutex>(mutext_feeder);
	uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
	DEBUG_CODE(DPD_LDBG << "Manage event for service with index " << service_index << " and url" << client_connection->getURL();)
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
int DefaultPersistenceDriver::pushNewDataset(const std::string& producer_key,
											 CDataWrapper *new_dataset,
											 int store_hint) {
	CHAOS_ASSERT(new_dataset)
	int err = 0;
	auto_ptr<SerializationBuffer> serialization(new_dataset->getBSONData());
	delete(new_dataset);
	
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
		DEBUG_CODE(DPD_LDBG << "No available socket->loose packet");
	}
	
	return err;
}

// get a dataset
int DefaultPersistenceDriver::getLastDataset(const std::string& producer_key,
											 chaos::common::data::CDataWrapper **last_dataset) {
	int err = 0;
	uint32_t size = 0;
	char* result = NULL;
	DirectIOChannelsInfo	*next_client = static_cast<DirectIOChannelsInfo*>(connection_feeder.getService());
	if(!next_client) return NULL;
	
	boost::shared_lock<boost::shared_mutex>(next_client->connection_mutex);
	
	next_client->device_client_channel->requestLastOutputData(producer_key, (void**)&result, size);
	*last_dataset = new CDataWrapper(result);
	return err;
}