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

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>


namespace chaos_data = chaos::common::data;
namespace chaos_cache = chaos::common::data::cache;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;


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

int64_t DirectIODeviceClientChannel::sendOutputDeviceChannels(chaos_data::SerializationBuffer *serialized_output_channels) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//set opcode
	dio_data_pack.header.dispatcher_header.fields.channel_opcode = DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_OUTPUT_CHANNEL;
	
	//set header
	header_data.device_hash = device_hash;
	DIRECT_IO_SET_CHANNEL_HEADER(dio_data_pack, &header_data, sizeof(DirectIODeviceChannelHeaderData))
	DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)serialized_output_channels->getBufferPtr(), (uint32_t)serialized_output_channels->getBufferLen())
	
	return sendData(&dio_data_pack);
}

int64_t DirectIODeviceClientChannel::sendOutputDeviceChannelsWithLive(chaos_data::SerializationBuffer *serialized_output_channels) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//set opcode
	dio_data_pack.header.dispatcher_header.fields.channel_opcode = DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_OUTPUT_CHANNEL_WITH_LIVE;
	//set header
	header_data.device_hash = device_hash;
	
	//set header and data
	DIRECT_IO_SET_CHANNEL_HEADER(dio_data_pack, &header_data, sizeof(DirectIODeviceChannelHeaderData))
	DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)serialized_output_channels->getBufferPtr(), (uint32_t)serialized_output_channels->getBufferLen())
	
	return client_instance->sendPriorityData(&dio_data_pack);
}

int64_t DirectIODeviceClientChannel::sendInputDeviceChannels(chaos_data::SerializationBuffer *serialized_input_channels) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//dio_data_pack.header.dispatcher_header.fields.channel_opcode = opcode;
	//DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)data_pack->getBufferPtr(), (uint32_t)data_pack->getBufferLen())
	//set header
	header_data.device_hash = device_hash;
	return client_instance->sendServiceData(&dio_data_pack);
}

int64_t DirectIODeviceClientChannel::sendInputDeviceChannelsWithLive(chaos_data::SerializationBuffer *serialized_input_channels) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//dio_data_pack.header.dispatcher_header.fields.channel_opcode = opcode;
	//DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)data_pack->getBufferPtr(), (uint32_t)data_pack->getBufferLen())
	//set header
	header_data.device_hash = device_hash;
	return client_instance->sendServiceData(&dio_data_pack);
}

int64_t DirectIODeviceClientChannel::sendNewReceivedCommand(chaos_data::SerializationBuffer *serialized_received_command) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//dio_data_pack.header.dispatcher_header.fields.channel_opcode = opcode;
	//DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)data_pack->getBufferPtr(), (uint32_t)data_pack->getBufferLen())
	//set header
	header_data.device_hash = device_hash;
	return client_instance->sendServiceData(&dio_data_pack);
}

int64_t DirectIODeviceClientChannel::sendNewReceivedCommandWithLive(chaos_data::SerializationBuffer *serialized_received_command) {
	DirectIODataPack dio_data_pack;
	DirectIODeviceChannelHeaderData header_data;
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	std::memset(&header_data, 0, sizeof(DirectIODeviceChannelHeaderData));
	
	//dio_data_pack.header.dispatcher_header.fields.channel_opcode = opcode;
	//DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)data_pack->getBufferPtr(), (uint32_t)data_pack->getBufferLen())
	//set header
	header_data.device_hash = device_hash;
	return client_instance->sendServiceData(&dio_data_pack);
}