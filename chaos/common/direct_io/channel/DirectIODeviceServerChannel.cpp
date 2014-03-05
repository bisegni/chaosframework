/*
 *	DirectIODeviceServerChannel.cpp
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
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;


DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIODeviceServerChannel::setHandler(DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler *_handler) {
	handler = _handler;
}

void DirectIODeviceServerChannel::consumeDataPack(DirectIODataPack *dataPack) {
	CHAOS_ASSERT(handler)
	
    // the opcode
	opcode::DeviceChannelOpcode  channel_opcode = static_cast<opcode::DeviceChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);

	switch (channel_opcode) {
		case opcode::DeviceChannelOpcodePutOutput: {
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderPutOpcode* >(dataPack->channel_header_data);
			header->device_hash = byte_swap<little_endian, host_endian, uint32_t>(header->device_hash);
			header->cache_tag = byte_swap<little_endian, host_endian, uint32_t>(header->cache_tag);
			handler->consumePutEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
        case opcode::DeviceChannelOpcodeGetLastOutput: {
            opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderGetOpcode* >(dataPack->channel_header_data);
            //decode the endianes off the data
            header->field.endpoint = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.endpoint);
            header->field.port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.port);
            header->field.device_hash = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.device_hash);
            header->field.address = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.address);
			handler->consumeGetEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
		default:
			break;
	}
	
	//forward event
	/*switch (dataPack->header.dispatcher_header.fields.channel_part) {
		case DIRECT_IO_CHANNEL_PART_EMPTY:
			break;
		case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
			free(dataPack->channel_header_data);
			break;
		case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
			free(dataPack->channel_data);
			break;
		case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
			free(dataPack->channel_data);
			free(dataPack->channel_header_data);
			break;
	}*/
}
