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


DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIODeviceServerChannel::setHandler(DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler *_handler) {
	handler = _handler;
}

void DirectIODeviceServerChannel::consumeDataPack(DirectIODataPack *dataPack) {
	CHAOS_ASSERT(handler)
	
	DirectIODeviceChannelHeaderData channel_header;
	
	DeviceChannelOpcode::DeviceChannelOpcode  opcode = static_cast<DeviceChannelOpcode::DeviceChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);

	//convert hash
	channel_header.device_hash = byte_swap<host_endian, little_endian, uint32_t>(static_cast<DirectIODeviceChannelHeaderData*>(dataPack->channel_header_data)->device_hash);

	//forward event
	handler->consumeDeviceEvent(opcode, channel_header, dataPack->channel_data);
	
	//delete datapack
	delete dataPack;
}
