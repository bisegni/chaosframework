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

#define AnswerDataWorker_LOG_HEAD "[AnswerDataWorker] - "

#define ADWLAPP_ LAPP_ << AnswerDataWorker_LOG_HEAD
#define ADWLDBG_ LDBG_ << AnswerDataWorker_LOG_HEAD << __FUNCTION__ << " - "
#define ADWLERR_ LERR_ << AnswerDataWorker_LOG_HEAD

typedef boost::unique_lock<boost::shared_mutex>	AnswerEngineWriteLock;
typedef boost::shared_lock<boost::shared_mutex> AnswerEngineReadLock;


AnswerDataWorker::AnswerDataWorker(chaos_direct_io::DirectIOClient *_client_instance, std::string _cache_impl_name):
work_on_purge(false),
cache_impl_name(_cache_impl_name),
direct_io_client(_client_instance) {
}

AnswerDataWorker::~AnswerDataWorker() {
	if(getServiceState() == service_state_machine::InizializableServiceType::IS_INITIATED) {
		deinit();
	}
}

void AnswerDataWorker::init(void *init_data) throw (chaos::CException) {
	//call superclass init method
	DataWorker::init(init_data);
	
	//allocate cached driver for every thread
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		thread_cookie[idx] = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	}

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
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		delete(tmp_ptr);
	}

	ADWLAPP_ << "stop the purge thread";
	work_on_purge = false;
	purge_thread_wait_variable.unlock();
	purge_thread->join();
	ADWLAPP_ << "purge thread stoppped";

	//delete all registered object
	ADWLAPP_ << "Clear all active connections";
	ADWKeyObjectContainer::clearElement();
	
	//try to remove all other remainde disconnected connections
	ADWLAPP_ << "Clear all orfaned answer connection";
	purge();
	
	//deinit client
	ADWLAPP_ << "Deinitilize the directio client";
	chaos::utility::InizializableService::deinitImplementation(direct_io_client, direct_io_client->getName(), __PRETTY_FUNCTION__);	
	
	//call superclass init method
	DataWorker::deinit();
}

bool AnswerDataWorker::increaseAccessNumber(ClientConnectionInfo *conn_info) {
    boost::uint32_t old_var = 0;
    volatile boost::uint32_t *mem_var = &conn_info->access_number;
    if((old_var = conn_info->access_number) != 0) {
        do {
            if((old_var = conn_info->access_number) == 0) return false;
        } while(boost::interprocess::ipcdetail::atomic_add32(mem_var, 1) != old_var);
        return true;
    } else return false;
}

void AnswerDataWorker::decreaseAccessNumber(ClientConnectionInfo *conn_info) {
    volatile boost::uint32_t *mem_var = &conn_info->access_number;
    boost::interprocess::ipcdetail::atomic_add32(mem_var, -1);
}

ClientConnectionInfo *AnswerDataWorker::getClientChannel(AnswerDataWorkerJob *answer_job_info) {
	ClientConnectionInfo *conn_info = NULL;
	// lock for all
	boost::unique_lock<boost::shared_mutex> lock(mutex_add_new_client);
	
	if(ADWKeyObjectContainer::hasKey(answer_job_info->request_header->field.answer_server_hash)) {
        conn_info = ADWKeyObjectContainer::accessItem(answer_job_info->request_header->field.answer_server_hash);
        if(increaseAccessNumber(conn_info)) {
            return conn_info;
        }
	}
	
	std::string key_requested((const char *)answer_job_info->key_data, answer_job_info->key_len);
	std::string answer_server_description = boost::str(boost::format("%1%:%2%:%3%|%4%") %
													   UI64_TO_STRIP(answer_job_info->request_header->field.address) %
													   answer_job_info->request_header->field.p_port %
													   answer_job_info->request_header->field.s_port %
													   answer_job_info->request_header->field.endpoint);
	
	uint32_t answer_server_hash = chaos::common::data::cache::FastHash::hash(answer_server_description.c_str(), answer_server_description.size(), 0);

	DEBUG_CODE(ADWLDBG_ << "verify the rigth hash for " << answer_server_description;);
	DEBUG_CODE(ADWLDBG_ << "Forwarded by client -> " << answer_job_info->request_header->field.answer_server_hash;)
	DEBUG_CODE(ADWLDBG_ << "calculated-> " << answer_server_hash;)
	if(answer_server_hash != answer_job_info->request_header->field.answer_server_hash) {
		DEBUG_CODE(ADWLERR_ << "Error on lcient get opcode header for answer server information";)
	}
	//allocate the client info struct
	conn_info = new ClientConnectionInfo();
	
	if(!conn_info) return NULL;
	conn_info->access_number = 2;
    conn_info->connection = NULL;
    conn_info->channel = NULL;
	//get connection
	conn_info->connection = direct_io_client->getNewConnection(answer_server_description);
    if(conn_info->connection->getConnectionHash() != answer_job_info->request_header->field.answer_server_hash) {
		DEBUG_CODE(ADWLERR_ << "Error ongenerated and connection hash";)
	}
	if(!conn_info->connection) {
		disposeClientInfo(conn_info);
		return NULL;
	}
	//conn_info->connection->setConnectionHash(answer_job_info->request_header->field.answer_server_hash);
	conn_info->connection->setEventHandler(this);
	//get channel
	conn_info->channel = dynamic_cast<DirectIODeviceClientChannel*>(conn_info->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
	if(!conn_info->channel) {
		disposeClientInfo(conn_info);
		return NULL;
	}
    conn_info->channel->setDeviceID(key_requested);
	//all is gone well
	//now we can add client and channel to the maps
	ADWKeyObjectContainer::registerElement(conn_info->connection->getConnectionHash(), conn_info);
	DEBUG_CODE(ADWLDBG_ << "Allocate new connection for server description " << answer_server_description << " and hash " << answer_job_info->request_header->field.answer_server_hash;)
	

	//map_client_connection.insert(make_pair);
	return conn_info;
}

void AnswerDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	AnswerDataWorkerJob *dw_job_ptr = reinterpret_cast<AnswerDataWorkerJob*>(job_info);
	cache_system::CacheDriver *cache_driver_instance = reinterpret_cast<cache_system::CacheDriver*>(cookie);

	ClientConnectionInfo *connection_info = getClientChannel(dw_job_ptr);
	if(connection_info) {
		//send data to requester node
		void *data = NULL;
		uint32_t data_len = 0;
		cache_driver_instance->getData(dw_job_ptr->key_data, dw_job_ptr->key_len, &data, data_len);
		
		connection_info->channel->storeAndCacheDataOutputChannel(data, data_len);
        
        //decrease the use of the channel
        decreaseAccessNumber(connection_info);
	}
	free(dw_job_ptr->key_data);
	free(dw_job_ptr->request_header);
	free(dw_job_ptr);
}

void AnswerDataWorker::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	//manage the event disconnected evenrt ( the conneciton is done directli by the request
	if(event == DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected) return;
	
    //lock the creation of new connection
    boost::unique_lock<boost::shared_mutex> uniqueLock(mutex_add_new_client);
    
	DEBUG_CODE(ADWLDBG_ << "Handle disconnection event for hash -> " << client_connection->getConnectionHash();)
    
    if(ADWKeyObjectContainer::hasKey(client_connection->getConnectionHash())) {
        ClientConnectionInfo *connection_info = ADWKeyObjectContainer::accessItem(client_connection->getConnectionHash());

        //NEED TO BE FOUND A LOGIC TO INVALIDATE AND DELETE AFTER THIS METHOD IS TERMINATED
        map_to_purge.insert(make_pair(client_connection->getConnectionHash(), connection_info));
        DEBUG_CODE(ADWLDBG_ << "Added to purge queue for connection of the server " << client_connection->getServerDescription() << " and hash " << client_connection->getConnectionHash();)

        ADWKeyObjectContainer::deregisterElementKey(client_connection->getConnectionHash());
    }
    
}

void AnswerDataWorker::freeObject(uint32_t key, ClientConnectionInfo *elementPtr) {
	//lock the creation of new connection
    boost::unique_lock<boost::shared_mutex> uniqueLock(mutex_add_new_client);

	if(ADWKeyObjectContainer::hasKey(elementPtr->connection->getConnectionHash())) {
        ClientConnectionInfo *connection_info = ADWKeyObjectContainer::accessItem(elementPtr->connection->getConnectionHash());
		
        //NEED TO BE FOUND A LOGIC TO INVALIDATE AND DELETE AFTER THIS METHOD IS TERMINATED
        map_to_purge.insert(make_pair(elementPtr->connection->getConnectionHash(), connection_info));
        DEBUG_CODE(ADWLDBG_ << "Added to purge queue for connection of the server " << elementPtr->connection->getServerDescription() << " and hash " << elementPtr->connection->getConnectionHash();)
		
        ADWKeyObjectContainer::deregisterElementKey(elementPtr->connection->getConnectionHash());
    }
	
	//dispose element non managed, thi smethod is called only when
	//ADWKeyObjectContainer::clearElement(); is called (only in destructor
	//disposeClientInfo(elementPtr);
}

void AnswerDataWorker::disposeClientInfo(ClientConnectionInfo *client_info) {
	CHAOS_ASSERT(client_info)
	uint32_t conn_hash = client_info->connection->getConnectionHash();
	std::string server_desc = client_info->connection->getServerDescription();
	
	DEBUG_CODE(ADWLDBG_ << "starting purge oepration for " << server_desc;)
	if(client_info->channel) {
		client_info->connection->releaseChannelInstance(client_info->channel);
	}
	if(client_info->connection) {
		direct_io_client->releaseConnection(client_info->connection);
	}
    if(ADWKeyObjectContainer::hasKey(conn_hash)) {
        ADWKeyObjectContainer::deregisterElementKey(conn_hash);
    }
	delete (client_info);
	DEBUG_CODE(ADWLDBG_ << "ending purge operation for " << server_desc;)
}

void AnswerDataWorker::purge_thread_worker() {
	//work on purge after some time passed to sleep
	while(work_on_purge) {
		purge();
		purge_thread_wait_variable.wait(5000);
	}
}

void AnswerDataWorker::purge(uint32_t max_purge_element) {
	//lock the map
    boost::uint32_t old_value = NULL;
    volatile boost::uint32_t *mem = NULL;
	boost::unique_lock<boost::mutex> lock(mutex_map_to_purge);
	
	for(std::map<uint32_t, ClientConnectionInfo* >::iterator iter = map_to_purge.begin();
		iter != map_to_purge.end() && --max_purge_element;) {
        
        mem = &iter->second->access_number;
        if(iter->second->access_number > 1) continue;
        //we must to be on 0 at this poicass on value to port i to -1
        old_value = iter->second->access_number;
        if(boost::interprocess::ipcdetail::atomic_cas32(mem, *mem-1, old_value) != old_value) continue;
        
        //we should be in negative number so we can dispose the channel
		//delete the unused channel
		if(iter->second) disposeClientInfo(iter->second);
		//erase after increment the iterator
		map_to_purge.erase(iter++);
	}
}

void AnswerDataWorker::addServer(std::string server_description) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->addServer(server_description);
	}
	
}

void AnswerDataWorker::updateServerConfiguration() {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->updateConfig();
	}
}