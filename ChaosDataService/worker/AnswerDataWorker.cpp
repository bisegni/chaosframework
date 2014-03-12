//
//  AnswerDataWorker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 12/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "AnswerDataWorker.h"
#include <chaos/common/utility/InetUtility.h>
#include <boost/format.hpp>

using namespace chaos::data_service::worker;
using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define AnswerDataWorker_LOG_HEAD "[AnswerEngine] - "

#define ADWLAPP_ LAPP_ << AnswerDataWorker_LOG_HEAD
#define ADWLDBG_ LDBG_ << AnswerDataWorker_LOG_HEAD
#define ADWLERR_ LERR_ << AnswerDataWorker_LOG_HEAD

typedef boost::unique_lock<boost::shared_mutex>	AnswerEngineWriteLock;
typedef boost::shared_lock<boost::shared_mutex> AnswerEngineReadLock;


AnswerDataWorker::AnswerDataWorker(chaos_direct_io::DirectIOClient *_client_instance,  cache_system::CacheDriver *_cache_driver_instance):cache_driver_instance(_cache_driver_instance), direct_io_client(_client_instance) {
}
AnswerDataWorker::~AnswerDataWorker() {
	//delete the cache instance
	if(cache_driver_instance) delete(cache_driver_instance);
	
	//delete all registered object
	TemplatedKeyObjectContainer::clearElement();
}

void AnswerDataWorker::init(void *init_data) throw (chaos::CException) {
	
	//call superclass init method
	DataWorker::init(init_data);

	//check client
	if(!direct_io_client) throw chaos::CException(-1, "DirectIO client not specified", __PRETTY_FUNCTION__);
	
	//innit client
	chaos::utility::InizializableService::initImplementation(direct_io_client, init_data, direct_io_client->getName(), __PRETTY_FUNCTION__);
}

void AnswerDataWorker::deinit() throw (chaos::CException) {
	//deinit client
	chaos::utility::InizializableService::deinitImplementation(direct_io_client, direct_io_client->getName(), __PRETTY_FUNCTION__);
	
	
	//call superclass init method
	DataWorker::deinit();
}

DirectIODeviceClientChannel *AnswerDataWorker::getClientChannel(opcode_headers::DirectIODeviceChannelHeaderGetOpcode *client_header) {
	ClientConnectionInfo *conn_info = NULL;
	// get upgradable access
	boost::upgrade_lock<boost::shared_mutex> lock(mutex_add_new_client);
	
	
	if(TemplatedKeyObjectContainer::hasKey(client_header->field.device_hash)) {
		return TemplatedKeyObjectContainer::accessItem(client_header->field.device_hash)->channel;
	}

	// get exclusive access
	boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

	std::string client_server_description = boost::str( boost::format("%1%:%2%:%3%") % UI64_TO_STRIP(client_header->field.address) % client_header->field.p_port % client_header->field.s_port);
	
	//allocate the client info struct
	conn_info = new ClientConnectionInfo();
	
	if(!conn_info) return NULL;
	
	std::memset(conn_info, 0, sizeof(ClientConnectionInfo));
	//get connection
	conn_info->connection = direct_io_client->getNewConnection(client_server_description);
	if(!conn_info->connection) {
		disposeClientInfo(conn_info);
		return NULL;
	}
	conn_info->connection->setConnectionHash(client_header->field.device_hash);
	conn_info->connection->setEventHandler(this);
	//get channel
	conn_info->channel = reinterpret_cast<DirectIODeviceClientChannel*>(conn_info->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
	if(!conn_info->channel) {
		disposeClientInfo(conn_info);
		return NULL;
	}

	//all is gone well
	//now we can add client and channel to the maps
	TemplatedKeyObjectContainer::registerElement(client_header->field.device_hash, conn_info);
	DEBUG_CODE(ADWLDBG_ << "Get new connection for server description " << client_server_description << " and hash " << client_header->field.device_hash;)
	

	//map_client_connection.insert(make_pair);
	return conn_info->channel;
}

void AnswerDataWorker::executeJob(WorkerJobPtr job_info) {
	AnswerDataWorkerJob *dw_job_ptr = reinterpret_cast<AnswerDataWorkerJob*>(job_info);
	DirectIODeviceClientChannel *channel = getClientChannel(dw_job_ptr->request_header);
	if(channel) {
		//send data to requester node
		void *data = NULL;
		uint32_t data_len = 0;
		if(!cache_driver_instance->getData(dw_job_ptr->request_header->field.device_hash, &data, data_len)) {
			channel->putDataOutputChannel(true, data, data_len);
		}
		//
	}
	free(dw_job_ptr->request_header);
	free(dw_job_ptr);
}

void AnswerDataWorker::handleEvent(uint32_t connection_identifier, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//manage the event disconnected evenrt ( the conneciton is done directli by the request
	if(event == DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected) return;
	
	if(!TemplatedKeyObjectContainer::hasKey(connection_identifier)) return;
	
	ClientConnectionInfo *conn_info = TemplatedKeyObjectContainer::accessItem(connection_identifier);
	
	//deregister disconnected client
	DEBUG_CODE(ADWLDBG_ << "manage disconnection of the server " << conn_info->connection->getServerDescription() << " and hash " << connection_identifier;)
	//NEED TO BE FOUND A LOGIC TO INVALIDATE AND DELETE AFTER THIS METHOD IS TERMINATED
	//disposeClientInfo(conn_info);
}

void AnswerDataWorker::freeObject(uint32_t key, ClientConnectionInfo *elementPtr) {
	disposeClientInfo(elementPtr);
}

void AnswerDataWorker::disposeClientInfo(ClientConnectionInfo *client_info) {
	CHAOS_ASSERT(client_info)
	uint32_t conn_hash = client_info->connection->getConenctionHash();
	
	if(client_info->channel) {
		client_info->connection->releaseChannelInstance(client_info->channel);
	}
	if(client_info->connection) {
		direct_io_client->releaseConnection(client_info->connection);
	}
	TemplatedKeyObjectContainer::deregisterElementKey(conn_hash);
	delete (client_info);
}