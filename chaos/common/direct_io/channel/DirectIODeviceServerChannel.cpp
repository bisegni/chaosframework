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
		case opcode::DeviceChannelOpcodePutOutputWithCache: {
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode header;
            header.device_hash = byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)dataPack->channel_header_data));
			handler->consumePutEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
        case opcode::DeviceChannelOpcodeGetOutputFromCache: {
            opcode_headers::DirectIODeviceChannelHeaderGetOpcode header;
            //decode the endianes off the data
            header.field.device_hash = TO_LITTE_ENDNS(uint32_t, dataPack->channel_header_data, 0);
            header.field.address = TO_LITTE_ENDNS(uint32_t, dataPack->channel_header_data, 4);
            header.field.port = TO_LITTE_ENDNS(uint32_t, dataPack->channel_header_data, 8);
            header.field.endpoint = TO_LITTE_ENDNS(uint32_t, dataPack->channel_header_data, 10);
			handler->consumeGetEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
		default:
			break;
	}
	
	//forward event
	
	
	//delete datapack
	delete dataPack;
}
