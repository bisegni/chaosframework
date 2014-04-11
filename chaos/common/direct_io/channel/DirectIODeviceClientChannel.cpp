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

#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/direct_io/DirectIOClient.h>

#include <boost/format.hpp>

#include <string.h>

namespace chaos_data = chaos::common::data;
namespace chaos_cache = chaos::common::data::cache;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

//#define PUT_HEADER_LEN  sizeof(DirectIODeviceChannelHeaderPutOpcode)-sizeof(void*)+device_id.size()
#define PUT_HEADER_LEN  GET_PUT_OPCODE_FIXED_PART_LEN+device_id.size()


DirectIODeviceClientChannel::DirectIODeviceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index, true),
device_hash(0),
device_id(""),
put_mode(DirectIODeviceClientChannelPutModeLiveOnly),
put_opcode_header(NULL) {

}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {
	if(put_opcode_header) {
		free(put_opcode_header);
	}
}

void DirectIODeviceClientChannel::setDeviceID(std::string _device_id, DirectIODeviceClientChannelPutMode _put_mode) {
	//create the has for the device id
	device_hash = chaos_cache::FastHash::hash(_device_id.c_str(), _device_id.size(), 0);
	
	//keep track of the device id
	device_id = _device_id;
	put_mode = _put_mode;
	if(put_opcode_header) {
		free(put_opcode_header);
	}
	put_opcode_header = (opcode_headers::DirectIODeviceChannelHeaderPutOpcode *)malloc(PUT_HEADER_LEN);
	std::memset(put_opcode_header, 0, PUT_HEADER_LEN);
	//allign the key pointr to his position
	//put_opcode_header->key_data = (void*)((char*)put_opcode_header+sizeof(DirectIODeviceChannelHeaderPutOpcode));
	
	//-------->initialize the headers <----------
	prepare_put_opcode();
	prepare_get_opcode();
}

void DirectIODeviceClientChannel::prepare_put_opcode() {
	if(!put_opcode_header) return;
	put_opcode_header->device_hash = TO_LITTE_ENDNS_NUM(uint32_t, device_hash);
	put_opcode_header->tag = /*TO_LITTE_ENDNS_NUM(uint32_t,*/(uint8_t) put_mode; /*);*/
	put_opcode_header->key_len = device_id.size();
	//put_opcode_header->key_data
	std::memcpy(GET_PUT_OPCODE_KEY_PTR(put_opcode_header), device_id.c_str(), put_opcode_header->key_len);
}

void DirectIODeviceClientChannel::prepare_get_opcode() {
	std::memset(&get_opcode_header, 0, sizeof(DirectIODeviceChannelHeaderGetOpcode));
    get_opcode_header.field.device_hash = TO_LITTE_ENDNS_NUM(uint32_t, device_hash);
	get_opcode_header.field.answer_server_hash =  TO_LITTE_ENDNS_NUM(uint32_t, answer_server_info.hash);
    get_opcode_header.field.address = TO_LITTE_ENDNS_NUM(uint64_t, answer_server_info.ip);
    get_opcode_header.field.p_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.p_server_port);
	get_opcode_header.field.s_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.s_server_port);
    get_opcode_header.field.endpoint = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.endpoint);
}

void DirectIODeviceClientChannel::setAnswerServerInfo(uint16_t p_server_port, uint16_t s_server_port, uint16_t answer_endpoint) {
	answer_server_info.p_server_port = p_server_port;
	answer_server_info.s_server_port = s_server_port;
    answer_server_info.endpoint = answer_endpoint;
    //setEndpoint(endpoint);
	answer_server_info.ip = ((DirectIOClientConnection*)client_instance)->getI64Ip();
	
	std::string client_server_description = boost::str( boost::format("%1%:%2%:%3%|%4%") % UI64_TO_STRIP(answer_server_info.ip) % answer_server_info.p_server_port % answer_server_info.s_server_port %  answer_server_info.endpoint);
	answer_server_info.hash = chaos::common::data::cache::FastHash::hash(client_server_description.c_str(), client_server_description.size(), 0);
	
	//-------->initialize the headers <----------
	prepare_put_opcode();
	prepare_get_opcode();
}

int64_t DirectIODeviceClientChannel::storeAndCacheDataOutputChannel(void *buffer, uint32_t buffer_len) {
	DirectIODataPack *data_pack = new DirectIODataPack();
	std::memset(data_pack, 0, sizeof(DirectIODataPack));

	//set opcode
	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutOutput);
	
	//set the header
	DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)PUT_HEADER_LEN)
	//set data if the have some
	if(buffer_len)DIRECT_IO_SET_CHANNEL_DATA(data_pack, buffer, buffer_len)
	return client_instance->sendPriorityData(this, completeDataPack(data_pack));
}

//! Send device serialization with priority
int64_t DirectIODeviceClientChannel::requestLastOutputData() {
	DirectIODataPack *data_pack = new DirectIODataPack();
	std::memset(data_pack, 0, sizeof(DirectIODataPack));

        //set opcode
	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetLastOutput);
	
        //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, &get_opcode_header, sizeof(DirectIODeviceChannelHeaderGetOpcode))
	DIRECT_IO_SET_CHANNEL_DATA(data_pack, (void*)device_id.c_str(), device_id.size())
	return client_instance->sendPriorityData(this, completeDataPack(data_pack));
}

void DirectIODeviceClientChannel::freeSentData(void *data, DisposeSentMemoryInfo& dispose_memory_info) {
	switch (dispose_memory_info.sent_part) {
		case 1:
			//header
			//free(data); the ehader must not be deleted
			break;
			
		case 2: // data
			if(static_cast<opcode::DeviceChannelOpcode>(dispose_memory_info.sent_opcode) == opcode::DeviceChannelOpcodePutOutput) {
				//the data sent with the opcode DeviceChannelOpcodePutOutput is the output dataset and need to be deleted
				free(data);
			}
			break;
		default:break;
	}

}