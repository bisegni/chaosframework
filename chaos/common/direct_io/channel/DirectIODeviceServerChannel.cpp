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


DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index), handler(NULL) {
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
			//reallign the pointer to the start of the key
			//header->key_data = (void*)((char*)header+sizeof(DirectIODeviceChannelHeaderPutOpcode));

			//header->device_hash = FROM_LITTLE_ENDNS_NUM(uint32_t, header->device_hash);
			header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
			handler->consumePutEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
        case opcode::DeviceChannelOpcodeGetLastOutput: {
            opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderGetOpcode* >(dataPack->channel_header_data);
            //decode the endianes off the data
            header->field.endpoint = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.endpoint);
            header->field.p_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.p_port);
			header->field.s_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.s_port);
            //header->field.device_hash = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.device_hash);
			//header->field.answer_server_hash = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.answer_server_hash);
            header->field.address = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.address);
			handler->consumeGetEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
		default:
			break;
	}

	//only data pack is deleted, header data and channel data are managed by hendler
	delete dataPack;
}
