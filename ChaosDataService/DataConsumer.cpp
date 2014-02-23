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

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/endianess.h>
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
	cache_driver_instance = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	if(!cache_driver_instance) throw chaos::CException(-6, "Error allocating cache driver", __FUNCTION__);
	
	DSLAPP_ << "Configure server on cache driver";
	for(CacheServerListIterator iter = settings->startup_chache_servers.begin();
		iter != settings->startup_chache_servers.end();
		iter++) {
		if(cache_driver_instance->addServer(*iter)) {
			DSLERR_ << "Error registering server " << *iter;
		} else {
			DSLERR_ << "Registered server " << *iter;
		}
	}
}

void DataConsumer::start() throw (chaos::CException) {
}

void DataConsumer::stop() throw (chaos::CException) {
}

void DataConsumer::deinit() throw (chaos::CException) {
	DSLAPP_ << "Release DirectIOCDataWrapperServerChannel into the endpoint";
	server_endpoint->releaseChannelInstance(device_channel);
	
	DSLAPP_ << "Release cache Driver";
	if(cache_driver_instance) delete(cache_driver_instance);
}


void DataConsumer::consumeDeviceEvent(opcode::DeviceChannelOpcode channel_opcode, void* channel_header_ptr, void *channel_data) {
	chaos_data::CDataWrapper *data = NULL;
	chaos_data::SerializationBuffer *serialization = NULL;
	switch (channel_opcode) {
		case opcode::DeviceChannelOpcodePutOutputWithCache:
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode header;
            header.device_hash = byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)channel_header_ptr));
			data = static_cast<chaos_data::CDataWrapper*>(channel_data);
			serialization = data->getBSONData();
			cache_driver_instance->putData(header.device_hash, (void*)serialization->getBufferPtr(), (uint32_t)serialization->getBufferLen());
		default:
			break;
	}
}