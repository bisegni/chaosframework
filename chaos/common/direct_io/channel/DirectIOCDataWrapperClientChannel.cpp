/*
 *	DirectIOCDataWrapperClientChannel.cpp
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

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/channel/DirectIOCDataWrapperClientChannel.h>

using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

DirectIOCDataWrapperClientChannel::DirectIOCDataWrapperClientChannel(std::string alias):DirectIOVirtualClientChannel(alias, DIOCDC_Channel_Index, true) {
}
DirectIOCDataWrapperClientChannel::~DirectIOCDataWrapperClientChannel() {
}
uint32_t DirectIOCDataWrapperClientChannel::pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t opcode, chaos_data::SerializationBuffer *data_pack) {
	std::memset(&dio_data_pack, 0, sizeof(DirectIODataPack));
	dio_data_pack.header.dispatcher_header.fields.route_addr = endpoint_idx;
	dio_data_pack.header.dispatcher_header.fields.channel_idx = DIOCDC_Channel_Index;
	dio_data_pack.header.dispatcher_header.fields.channel_opcode = opcode;
	DIRECT_IO_SET_CHANNEL_DATA(dio_data_pack, (void*)data_pack->getBufferPtr(), (uint32_t)data_pack->getBufferLen())
	
	return sendData(&dio_data_pack);
}