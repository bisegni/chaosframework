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
int64_t DirectIOCDataWrapperClientChannel::pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t opcode, chaos_data::SerializationBuffer *serialization) {
	DirectIODataPack *data_pack = new DirectIODataPack();
	std::memset(data_pack, 0, sizeof(DirectIODataPack));
	data_pack->header.dispatcher_header.fields.route_addr = endpoint_idx;
	data_pack->header.dispatcher_header.fields.channel_opcode = opcode;
	DIRECT_IO_SET_CHANNEL_DATA(data_pack, (void*)serialization->getBufferPtr(), (uint32_t)serialization->getBufferLen())
	
	return sendData(data_pack);
}