 /*
 *	DataConsumer.h
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

#include "DataConsumer.h"
#include "worker/DeviceSharedDataWorker.h"
#include "worker/AnswerDataWorker.h"
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/InetUtility.h>
//#include <chaos/common/direct_io/DirectIOClient.h>
//#include <chaos/common/direct_io/channel/DirectIOCDataWrapperClientChannel.h>
#include <boost/thread.hpp>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define DataConsumer_LOG_HEAD "[DataConsumer] - "

#define DSLAPP_ LAPP_ << DataConsumer_LOG_HEAD
#define DSLDBG_ LDBG_ << DataConsumer_LOG_HEAD
#define DSLERR_ LERR_ << DataConsumer_LOG_HEAD

DataConsumer::DataConsumer() {
    
}

DataConsumer::~DataConsumer() {
    
}

void DataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!settings)  throw chaos::CException(-1, "No setting provided", __FUNCTION__);
	if(!settings->cache_driver_impl.size())  throw chaos::CException(-2, "No cache implemetation provided", __FUNCTION__);
	if(!settings->startup_chache_servers.size())  throw chaos::CException(-3, "No cache servers provided", __FUNCTION__);
	
	server_endpoint = network_broker->getDirectIOServerEndpoint();
	if(!server_endpoint) throw chaos::CException(-4, "Invalid server endpoint", __FUNCTION__);
	DSLAPP_ << "DataConsumer initialized with endpoint "<< server_endpoint->getRouteIndex();
	
	DSLAPP_ << "Allocating DirectIODeviceServerChannel";
	device_channel = (DirectIODeviceServerChannel*)server_endpoint->getNewChannelInstance("DirectIODeviceServerChannel");
	if(!device_channel) throw chaos::CException(-5, "Error allocating device server channel", __FUNCTION__);
	device_channel->setHandler(this);
	
	cache_impl_name = settings->cache_driver_impl;
	cache_impl_name.append("CacheDriver");
	DSLAPP_ << "The cache implementation to allocate is " << cache_impl_name;
	//cache_driver_instance
	device_data_worker = (chaos::data_service::worker::DataWorker**) malloc(sizeof(chaos::data_service::worker::DataWorker**) * DEVICE_WORKER_NUMBER);
	if(!device_data_worker) throw chaos::CException(-5, "Error allocating device workers", __FUNCTION__);
	
	chaos::data_service::worker::DeviceSharedDataWorker *tmp = NULL;
	for(int idx = 0; idx < DEVICE_WORKER_NUMBER; idx++) {
		device_data_worker[idx] = (tmp = new chaos::data_service::worker::DeviceSharedDataWorker(cache_impl_name));
		DSLAPP_ << "Configure server on device worker " << idx;
		for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
			iter != settings->startup_chache_servers.end();
			iter++) {
			tmp->init(NULL);
			tmp->start();
			tmp->addServer(*iter);
		}
	}
	
	//add answer worker
	chaos::data_service::worker::AnswerDataWorker *tmp_data_worker = NULL;
	for(int idx = 0; idx < ANSWER_WORKER_NUMBER; idx++) {
		//allocate a new worker with his personal client instance
		//get the cache driver instance
		cache_system::CacheDriver *cache_driver_instance = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
		for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
			iter != settings->startup_chache_servers.end();
			iter++) {
			cache_driver_instance->addServer(*iter);
		}
		tmp_data_worker = new chaos::data_service::worker::AnswerDataWorker(network_broker->getDirectIOClientInstance(), cache_driver_instance);
		tmp_data_worker->init(NULL);
		tmp_data_worker->start();
		answer_worker_list.addSlot(tmp_data_worker);
	}
}

void DataConsumer::start() throw (chaos::CException) {
}

void DataConsumer::stop() throw (chaos::CException) {
}

void DataConsumer::deinit() throw (chaos::CException) {
	DSLAPP_ << "Release DirectIOCDataWrapperServerChannel into the endpoint";
	server_endpoint->releaseChannelInstance(device_channel);
	
	for(int idx = 0; idx < answer_worker_list.getNumberOfSlot(); idx++) {
		chaos::data_service::worker::DataWorker *worker = answer_worker_list.accessSlotByIndex(idx);
		worker->stop();
		worker->deinit();
		delete(worker);
	}
	answer_worker_list.clearSlots();
	
	for(int idx = 0; idx < DEVICE_WORKER_NUMBER; idx++) {
		DSLAPP_ << "Release device worker "<< idx;
		device_data_worker[idx]->stop();
		device_data_worker[idx]->deinit();
		delete(device_data_worker[idx]);
		device_data_worker[idx] = NULL;
	}
	free(device_data_worker);
}

void DataConsumer::consumePutEvent(DirectIODeviceChannelHeaderPutOpcode *header, void *channel_data, uint32_t channel_data_len) {
	uint32_t index_to_use = device_data_worker_index++ % DEVICE_WORKER_NUMBER;

	chaos::data_service::worker::DeviceSharedWorkerJob *job = new chaos::data_service::worker::DeviceSharedWorkerJob();
	job->request_header = header;
	job->data_pack = channel_data;
	job->data_pack_len = channel_data_len;
	
    while(!device_data_worker[index_to_use]->submitJobInfo(job));
	
}

void DataConsumer::consumeGetEvent(DirectIODeviceChannelHeaderGetOpcode *header, void *channel_data, uint32_t channel_data_len) {
	chaos::data_service::worker::DataWorker *worker = answer_worker_list.accessSlot();
	
    chaos::data_service::worker::AnswerDataWorkerJob *job = new chaos::data_service::worker::AnswerDataWorkerJob();
	job->key_data = channel_data;
	job->key_len = channel_data_len;
	job->request_header = header;
	//while(!worker->submitJobInfo(job));
	((worker::AnswerDataWorker*)worker)->executeJob(job);
}