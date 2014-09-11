 /*
 *	QueryDataConsumer.h
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

#include "QueryDataConsumer.h"

#include "worker/DeviceSharedDataWorker.h"
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/InetUtility.h>

#include <boost/thread.hpp>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define QueryDataConsumer_LOG_HEAD "[QueryDataConsumer] - "

#define QDCAPP_ LAPP_ << QueryDataConsumer_LOG_HEAD
#define QDCDBG_ LDBG_ << QueryDataConsumer_LOG_HEAD << __FUNCTION__ << " - "
#define QDCERR_ LERR_ << QueryDataConsumer_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

QueryDataConsumer::QueryDataConsumer(vfs::VFSManager *_vfs_manager_instance):
vfs_manager_instance(_vfs_manager_instance),
query_engine(NULL){
    
}

QueryDataConsumer::~QueryDataConsumer() {
    
}

void QueryDataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!settings)  throw chaos::CException(-1, "No setting provided", __FUNCTION__);
	if(!settings->cache_driver_impl.size())  throw chaos::CException(-2, "No cache implemetation provided", __FUNCTION__);
	if(!settings->startup_chache_servers.size())  throw chaos::CException(-3, "No cache servers provided", __FUNCTION__);
	
	server_endpoint = network_broker->getDirectIOServerEndpoint();
	if(!server_endpoint) throw chaos::CException(-4, "Invalid server endpoint", __FUNCTION__);
	QDCAPP_ << "QueryDataConsumer initialized with endpoint "<< server_endpoint->getRouteIndex();
	
	QDCAPP_ << "Allocating DirectIODeviceServerChannel";
	device_channel = (DirectIODeviceServerChannel*)server_endpoint->getNewChannelInstance("DirectIODeviceServerChannel");
	if(!device_channel) throw chaos::CException(-5, "Error allocating device server channel", __FUNCTION__);
	device_channel->setHandler(this);
	
	//
	cache_impl_name = settings->cache_driver_impl;
	cache_impl_name.append("CacheDriver");
	QDCAPP_ << "The cache implementation to allocate is " << cache_impl_name;
	
	//cache_driver_instance
	device_data_worker = (chaos::data_service::worker::DataWorker**) malloc(sizeof(chaos::data_service::worker::DataWorker**) * settings->caching_worker_num);
	if(!device_data_worker) throw chaos::CException(-5, "Error allocating device workers", __FUNCTION__);
	
	//get the cached driver
	cache_driver = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name);
	
	//allocate query manager
	query_engine = new query_engine::QueryEngine(network_broker->getDirectIOClientInstance(),
												 settings->query_manager_thread_poll_size,
												 vfs_manager_instance);
	if(!query_engine) throw chaos::CException(-5, "Error allocating Query Engine", __FUNCTION__);
	chaos::utility::StartableService::initImplementation(query_engine, init_data, "QueryEngine", __PRETTY_FUNCTION__);
	
	//Sahred data worker
	chaos::data_service::worker::DeviceSharedDataWorker *tmp = NULL;
	for(int idx = 0; idx < settings->caching_worker_num; idx++) {
		device_data_worker[idx] = (tmp = new chaos::data_service::worker::DeviceSharedDataWorker(cache_impl_name, vfs_manager_instance));
		tmp->init(&settings->caching_worker_setting);
		QDCAPP_ << "Configure server on device worker " << idx;
		for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
			iter != settings->startup_chache_servers.end();
			iter++) {
			tmp->addServer(*iter);
		}
		tmp->updateServerConfiguration();
		tmp->start();

	}
	for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
		iter != settings->startup_chache_servers.end();
		iter++) {
		cache_driver->addServer(*iter);
	}
	cache_driver->updateConfig();
	
	//start virtual file mantainers timer
	QDCAPP_ << "Start virtual file mantainers timer with a timeout of " << settings->vfile_mantainer_delay*1000 << "seconds";
	chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, 0, settings->vfile_mantainer_delay*1000);
}

void QueryDataConsumer::start() throw (chaos::CException) {
	chaos::utility::StartableService::startImplementation(query_engine, "QueryEngine", __PRETTY_FUNCTION__);
}

void QueryDataConsumer::stop() throw (chaos::CException) {
	chaos::utility::StartableService::stopImplementation(query_engine, "QueryEngine", __PRETTY_FUNCTION__);
}

void QueryDataConsumer::deinit() throw (chaos::CException) {
	QDCAPP_ << "Remove virtual file mantainers timer";
	chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);

	QDCAPP_ << "Release direct io device channel into the endpoint";
	server_endpoint->releaseChannelInstance(device_channel);
	
	if(query_engine) {
		chaos::utility::StartableService::deinitImplementation(query_engine, "QueryEngine", __PRETTY_FUNCTION__);
		delete(query_engine);
	}
	
	QDCAPP_ << "Deallocating device push data worker list";
	for(int idx = 0; idx < settings->caching_worker_num; idx++) {
		QDCAPP_ << "Release device worker "<< idx;
		device_data_worker[idx]->stop();
		device_data_worker[idx]->deinit();
		delete(device_data_worker[idx]);
		device_data_worker[idx] = NULL;
	}
	free(device_data_worker);
}

int QueryDataConsumer::consumePutEvent(DirectIODeviceChannelHeaderPutOpcode *header,
									   void *channel_data,
									   uint32_t channel_data_len,
									   DirectIOSynchronousAnswerPtr synchronous_answer) {
	uint32_t index_to_use = device_data_worker_index++ % settings->caching_worker_num;
	chaos::data_service::worker::DeviceSharedWorkerJob *job = new chaos::data_service::worker::DeviceSharedWorkerJob();
	job->request_header = header;
	job->data_pack = channel_data;
	job->data_pack_len = channel_data_len;
    if(!device_data_worker[index_to_use]->submitJobInfo(job)) {
		DEBUG_CODE(QDCDBG_ << "error pushing data into worker queue");
		delete job;
	}
	return 0;
}

int QueryDataConsumer::consumeDataCloudQuery(DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header,
											 const std::string& search_key,
											 uint64_t search_start_ts,
											 uint64_t search_end_ts,
											 DirectIOSynchronousAnswerPtr synchronous_answer) {
	//compose the DirectIO endpoint where forward the answer
	std::string answer_server_description = boost::str(boost::format("%1%:%2%:%3%|%4%") %
													   UI64_TO_STRIP(header->field.address) %
													   header->field.p_port %
													   header->field.s_port %
													   header->field.endpoint);
	std::string query_id(header->field.query_id, 8);
	//execute the query
	query_engine->executeQuery(new query_engine::DataCloudQuery(query_id,
																db_system::DataPackIndexQuery(search_key, search_start_ts, search_end_ts),
																answer_server_description));
	//delete header and
	if(header) free(header);
	return 0;
}

int QueryDataConsumer::consumeGetEvent(DirectIODeviceChannelHeaderGetOpcode *header,
									   void *channel_data,
									   uint32_t channel_data_len,
									   DirectIOSynchronousAnswerPtr synchronous_answer) {
	return cache_driver->getData(channel_data,
								 channel_data_len,
								 &synchronous_answer->answer_data,
								 synchronous_answer->answer_size);
}

//async central timer hook
void QueryDataConsumer::timeout() {
	for (int idx = 0; idx < settings->caching_worker_num; idx++) {
		device_data_worker[idx]->mantain();
	}
}
