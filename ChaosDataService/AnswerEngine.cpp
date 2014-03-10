//
//  AnswerEngine.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "AnswerEngine.h"
#include <chaos/common/utility/InetUtility.h>
#include <boost/format.hpp>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
#define AnswerEngine_LOG_HEAD "[AnswerEngine] - "

#define AELAPP_ LAPP_ << AnswerEngine_LOG_HEAD
#define AELDBG_ LDBG_ << AnswerEngine_LOG_HEAD
#define AELERR_ LERR_ << AnswerEngine_LOG_HEAD

typedef boost::unique_lock<boost::shared_mutex>	AnswerEngineWriteLock;
typedef boost::shared_lock<boost::shared_mutex> AnswerEngineReadLock;

AnswerEngine::AnswerEngine():network_broker(NULL) {
	
}

AnswerEngine::~AnswerEngine() {
	
}

int AnswerEngine::registerNewClient(opcode_headers::DirectIODeviceChannelHeaderGetOpcode& client_header) {
	CHAOS_ASSERT(InizializableService::getServiceState() == chaos::utility::service_state_machine::InizializableServiceType::IS_INITIATED)
	AnswerEngineWriteLock(mutex_map);
	
    if(map_connections.count(client_header.field.device_hash)) {
        //client already registered
        return 0;
    }
	
	AnswerEngineClientInfo *client_info = new AnswerEngineClientInfo();
	std::string client_server_description = boost::str( boost::format("%1%:%2%:%3%") % UI64_TO_STRIP(client_header.field.address) % client_header.field.p_port % client_header.field.s_port);
	AELAPP_ << "Get new connection for server description " << client_server_description;
	
	client_info->connection = direct_io_client->getNewConnection(client_server_description);
	client_info->device_channel = reinterpret_cast<DirectIODeviceClientChannel*>(client_info->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
        //all is gone well
        //now we can add client and channel to the maps

	map_connections.insert(make_pair(client_header.field.device_hash, client_info));
	return 0;
}

void AnswerEngine::sendCacheAnswher(uint32_t client_hash, void *buffer, uint32_t buffer_len) {
	AnswerEngineReadLock(mutex_map);
	//send answer to client
	map_connections[client_hash]->device_channel->putDataOutputChannel(true, buffer, buffer_len);
}

//! Initialize instance
void AnswerEngine::init(void *init_data) throw(chaos::CException) {
	AELAPP_ << "Check for network broker instance";
	if(!network_broker) throw CException(-1, "No network brocker associated", "AnswerEngine::init");
	
	direct_io_client = network_broker->getDirectIOClientInstance();
	if(!direct_io_client) throw CException(-1, "Error allcoating direct io client", __PRETTY_FUNCTION__);
	
	chaos::utility::InizializableService::initImplementation(direct_io_client,  init_data, "DirectIOClient", __PRETTY_FUNCTION__);
}

//! Deinit the implementation
void AnswerEngine::deinit() throw(chaos::CException) {
	AELAPP_ << "delete all client instance";
	AnswerEngineWriteLock(mutex_map);
	for(MapConnectionIterator iter = map_connections.begin();
		iter != map_connections.end();
		iter++) {
		AnswerEngineClientInfo *client_info = iter->second;
		client_info->connection->releaseChannelInstance(client_info->device_channel);
		direct_io_client->releaseConnection(client_info->connection);
		delete client_info;
	}
	AELAPP_ << "Delete all client map entry";
	map_connections.clear();
	
	chaos::utility::InizializableService::deinitImplementation(direct_io_client, "DirectIOClient", __PRETTY_FUNCTION__);
	delete(direct_io_client);
}