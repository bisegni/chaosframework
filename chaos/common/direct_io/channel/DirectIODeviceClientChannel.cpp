/*
 *	DirectIODeviceDatasetClientChannel.cpp
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

#include <chaos/common/utility/endianess.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>


namespace chaos_data = chaos::common::data;
namespace chaos_cache = chaos::common::data::cache;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DirectIODeviceClientChannel::DirectIODeviceClientChannel(std::string alias):DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index, true) {

}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {
	
}

void DirectIODeviceClientChannel::setDeviceID(std::string _device_id) {
	//create the has for the device id
	device_hash = chaos_cache::FastHash::hash(_device_id.c_str(), _device_id.size(), 0);
	
	//keep track of the device id
	device_id = _device_id;
}

int64_t DirectIODeviceClientChannel::putDataOutputChannel(bool cache_it, void *buffer, uint32_t buffer_len) {
	DirectIODataPack data_pack;
	DirectIODeviceChannelHeaderPutOpcode header_data;
	
	//set opcode
	data_pack.header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(cache_it?opcode::DeviceChannelOpcodePutOutputWithCache:opcode::DeviceChannelOpcodePutOutputWithCache);
	
	//set header
	header_data.device_hash = byte_swap<little_endian, host_endian, uint32_t>(device_hash);
	
	DIRECT_IO_SET_CHANNEL_HEADER(data_pack, &header_data, sizeof(DirectIODeviceChannelHeaderData))
	DIRECT_IO_SET_CHANNEL_DATA(data_pack, buffer, buffer_len)
	return client_instance->sendPriorityData(&data_pack);
}

//! Send device serialization with priority
int64_t DirectIODeviceClientChannel::getLastDataOutputChannel(void **buffer, uint32_t& buffer_len) {
	return 0;
}