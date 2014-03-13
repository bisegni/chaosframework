/*
 *	AnswerDataWorker.cpp
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

#include "AnswerDataWorker.h"
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/data/cache/FastHash.h>

#include <boost/chrono.hpp>
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


AnswerDataWorker::AnswerDataWorker(chaos_direct_io::DirectIOClient *_client_instance,  cache_system::CacheDriver *_cache_driver_instance):
work_on_purge(false),
cache_driver_instance(_cache_driver_instance),
direct_io_client(_client_instance) {
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
	
	//start purge thread
	work_on_purge =true;
	purge_thread.reset(new boost::thread(&AnswerDataWorker::purge_thread_worker, this));
}

void AnswerDataWorker::deinit() throw (chaos::CException) {
	//shutdown purger thread
	work_on_purge = false;
	purge_thread_wait_variable. unlock();
	purge_thread->join();
	//deinit client
	chaos::utility::InizializableService::deinitImplementation(direct_io_client, direct_io_client->getName(), __PRETTY_FUNCTION__);
	
	
	//call superclass init method
	DataWorker::deinit();
}

DirectIODeviceClientChannel *AnswerDataWorker::getClientChannel(opcode_headers::DirectIODeviceChannelHeaderGetOpcode *client_header) {
	ClientConnectionInfo *conn_info = NULL;
	// get upgradable access
	boost::upgrade_lock<boost::shared_mutex> lock(mutex_add_new_client);
	
	
	if(TemplatedKeyObjectContainer::hasKey(client_header->field.answer_server_hash)) {
		return TemplatedKeyObjectContainer::accessItem(client_header->field.answer_server_hash)->channel;
	}

	// get exclusive access
	boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

	std::string answer_server_description = boost::str( boost::format("%1%:%2%:%3%") % UI64_TO_STRIP(client_header->field.address) % client_header->field.p_port % client_header->field.s_port);
	uint32_t answer_server_hash = chaos::common::data::cache::FastHash::hash(answer_server_description.c_str(), answer_server_description.size(), 0);

	DEBUG_CODE(ADWLDBG_ << "verify the rigth hash for " << answer_server_description;);
	DEBUG_CODE(ADWLDBG_ << "Forwarded by client -> " << client_header->field.answer_server_hash;)
	DEBUG_CODE(ADWLDBG_ << "calculated-> " << answer_server_hash;)
	if(answer_server_hash != client_header->field.answer_server_hash) {
		DEBUG_CODE(ADWLERR_ << "Error on lcient get opcode header for answer server information";)
	}
	//allocate the client info struct
	conn_info = new ClientConnectionInfo();
	
	if(!conn_info) return NULL;
	
	std::memset(conn_info, 0, sizeof(ClientConnectionInfo));
	//get connection
	conn_info->connection = direct_io_client->getNewConnection(answer_server_description);
	if(!conn_info->connection) {
		disposeClientInfo(conn_info);
		return NULL;
	}
	conn_info->connection->setConnectionHash(client_header->field.answer_server_hash);
	conn_info->connection->setEventHandler(this);
	//get channel
	conn_info->channel = dynamic_cast<DirectIODeviceClientChannel*>(conn_info->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
	if(!conn_info->channel) {
		disposeClientInfo(conn_info);
		return NULL;
	}

	//all is gone well
	//now we can add client and channel to the maps
	TemplatedKeyObjectContainer::registerElement(client_header->field.answer_server_hash, conn_info);
	DEBUG_CODE(ADWLDBG_ << "Allocate new connection for server description " << answer_server_description << " and hash " << client_header->field.answer_server_hash;)
	

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
		DEBUG_CODE(ADWLDBG_ << "Get data for hash " << dw_job_ptr->request_header->field.device_hash;);
		cache_driver_instance->getData(dw_job_ptr->request_header->field.device_hash, &data, data_len);
		
		DEBUG_CODE(ADWLDBG_ << "Post data for hash " << dw_job_ptr->request_header->field.device_hash << " of size " << data_len;);
		channel->putDataOutputChannel(true, data, data_len);
		//
	}
	free(dw_job_ptr->request_header);
	free(dw_job_ptr);
}

void AnswerDataWorker::handleEvent(uint32_t connection_identifier, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//manage the event disconnected evenrt ( the conneciton is done directli by the request
	if(event == DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected) return;
	DEBUG_CODE(ADWLDBG_ << "Handle disconnection event for hash -> " << connection_identifier;)

	//lock the purge queue
	boost::unique_lock<boost::mutex> lock(mutex_map_to_purge);
	if(!TemplatedKeyObjectContainer::hasKey(connection_identifier) || (map_to_purge.count(connection_identifier) > 0)) return;
	
	//element is never add to purge map
	ClientConnectionInfo *conn_info = TemplatedKeyObjectContainer::accessItem(connection_identifier);
	
	//deregister disconnected client
	//NEED TO BE FOUND A LOGIC TO INVALIDATE AND DELETE AFTER THIS METHOD IS TERMINATED
	map_to_purge.insert(make_pair(connection_identifier, conn_info));
	DEBUG_CODE(ADWLDBG_ << "Added to purge queue for connection of the server " << conn_info->connection->getServerDescription() << " and hash " << connection_identifier;)
}

void AnswerDataWorker::freeObject(uint32_t key, ClientConnectionInfo *elementPtr) {
	//dispose element non managed, thi smethod is called only when
	//TemplatedKeyObjectContainer::clearElement(); is called (only in destructor
	disposeClientInfo(elementPtr);
}

void AnswerDataWorker::disposeClientInfo(ClientConnectionInfo *client_info) {
	CHAOS_ASSERT(client_info)
	uint32_t conn_hash = client_info->connection->getConenctionHash();
	std::string server_desc = client_info->connection->getServerDescription();
	
	DEBUG_CODE(ADWLDBG_ << "Start purge oepration for " << server_desc;)
	if(client_info->channel) {
		client_info->connection->releaseChannelInstance(client_info->channel);
	}
	if(client_info->connection) {
		direct_io_client->releaseConnection(client_info->connection);
	}
	TemplatedKeyObjectContainer::deregisterElementKey(conn_hash);
	delete (client_info);
	DEBUG_CODE(ADWLDBG_ << "Endign purge oepration for " << server_desc;)
}

void AnswerDataWorker::purge_thread_worker() {
	//work on purge after some time passed to sleep
	while(work_on_purge) {
		purge();
		purge_thread_wait_variable.wait(5000);
	}
	//befor thread ends, try to remove all other remainde connections
	purge();
}

void AnswerDataWorker::purge(uint32_t max_purge_element) {
	ClientConnectionInfo *conn_info_to_delete = NULL;
	//lock the map
	boost::unique_lock<boost::mutex> lock(mutex_map_to_purge);
	
	for(std::map<uint32_t, ClientConnectionInfo* >::iterator iter = map_to_purge.begin();
		iter != map_to_purge.end() && --max_purge_element;) {
		//delete the unused channel
		if(iter->second) disposeClientInfo(iter->second);
		//erase after increment the iterator
		map_to_purge.erase(iter++);
	}
}