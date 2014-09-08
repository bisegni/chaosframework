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
#define PUT_HEADER_LEN  (GET_PUT_OPCODE_FIXED_PART_LEN+((uint32_t)device_id.size()))


DirectIODeviceClientChannel::DirectIODeviceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index, true),
//device_hash(0),
device_id(""),
put_mode(DirectIODeviceClientChannelPutModeLiveOnly),
put_opcode_header(NULL) {
	
}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {
	if(put_opcode_header) {
		free(put_opcode_header);
	}
}

void DirectIODeviceClientChannel::setDeviceID(std::string _CU_id, DirectIODeviceClientChannelPutMode _put_mode) {
	//keep track of the device id
	device_id = _CU_id;
	put_mode = _put_mode;
	if(put_opcode_header) {
		free(put_opcode_header);
	}
	put_opcode_header = (opcode_headers::DirectIODeviceChannelHeaderPutOpcode *)calloc(1, (PUT_HEADER_LEN+device_id.size()));
	//std::memset(put_opcode_header, 0, PUT_HEADER_LEN);
	//allign the key pointr to his position
	//put_opcode_header->key_data = (void*)((char*)put_opcode_header+sizeof(DirectIODeviceChannelHeaderPutOpcode));
	
	//-------->initialize the headers <----------
	prepare_put_opcode();
	prepare_get_opcode();
}

void DirectIODeviceClientChannel::prepare_put_opcode() {
	if(!put_opcode_header) return;
	//put_opcode_header->device_hash = TO_LITTE_ENDNS_NUM(uint32_t, device_hash);
	put_opcode_header->tag = /*TO_LITTE_ENDNS_NUM(uint32_t,*/(uint8_t) put_mode; /*);*/
	put_opcode_header->key_len = device_id.size();
	//put_opcode_header->key_data
	std::memcpy(GET_PUT_OPCODE_KEY_PTR(put_opcode_header), device_id.c_str(), put_opcode_header->key_len);
}

void DirectIODeviceClientChannel::prepare_get_opcode() {
	std::memset(&get_opcode_header, 0, sizeof(DirectIODeviceChannelHeaderGetOpcode));
    //get_opcode_header.field.device_hash = TO_LITTE_ENDNS_NUM(uint32_t, device_hash);
	//get_opcode_header.field.answer_server_hash =  TO_LITTE_ENDNS_NUM(uint32_t, answer_server_info.hash);
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
	//-------->initialize the headers <----------
	prepare_put_opcode();
	prepare_get_opcode();
}

int64_t DirectIODeviceClientChannel::storeAndCacheDataOutputChannel(void *buffer, uint32_t buffer_len) {
	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
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
int64_t DirectIODeviceClientChannel::requestLastOutputData(void **result, uint32_t &size) {
	uint64_t err = 0;
	DirectIOSynchronousAnswer *answer = NULL;
	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
	std::memset(data_pack, 0, sizeof(DirectIODataPack));
	
	//set opcode
	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetLastOutput);
	
	//set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, &get_opcode_header, sizeof(DirectIODeviceChannelHeaderGetOpcode))
	DIRECT_IO_SET_CHANNEL_DATA(data_pack, (void*)device_id.c_str(), (uint32_t)device_id.size())
	//send data with synchronous answer flag
	if((err = client_instance->sendPriorityData(this, completeDataPack(data_pack, true), &answer))) {
		//error getting last value
		if(answer && answer->answer_data) free(answer->answer_data);
	} else {
		//we got answer
		if(answer) {
			*result  = answer->answer_data;
			size = answer->answer_size;
		} else {
			*result = NULL;
			size = 0;
		}
	}
	if(answer) free(answer);
	return err;
}

int64_t DirectIODeviceClientChannel::queryDataCloud(std::string keys, uint64_t start_ts, uint64_t end_ts) {
	chaos_data::CDataWrapper query_description;
	//allcoate the data to send direct io pack
	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
	DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr query_data_lcoud_header = (DirectIODeviceChannelHeaderOpcodeQueryDataCloud*)calloc(sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud), 1);
				
	//fill the query CDataWrapper
	query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, keys);
	query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
	query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
	
	//fill the hader
    query_data_lcoud_header->field.address = TO_LITTE_ENDNS_NUM(uint64_t, answer_server_info.ip);
    query_data_lcoud_header->field.p_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.p_server_port);
	query_data_lcoud_header->field.s_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.s_server_port);
    query_data_lcoud_header->field.endpoint = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.endpoint);
	
	//set opcode
	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloud);
	
	//get the buffer to send
	auto_ptr<chaos_data::SerializationBuffer> buffer(query_description.getBSONData());
	
	//the frre of memeory is managed in this class in async way
	buffer->disposeOnDelete = false;
	
	//set data for the query
	DIRECT_IO_SET_CHANNEL_DATA(data_pack, (void*)buffer->getBufferPtr(), (uint32_t)buffer->getBufferLen());
	
	//send query request
	return client_instance->sendPriorityData(this, completeDataPack(data_pack));
}

//dispose the resuource forwarded in async way
void DirectIODeviceClientChannel::freeSentData(void *data, DisposeSentMemoryInfo& dispose_memory_info) {
	switch (dispose_memory_info.sent_part) {
			//header
		case 1:
			//free(data); the ehader must not be deleted
			break;
			// data
		case 2: {
			switch(static_cast<opcode::DeviceChannelOpcode>(dispose_memory_info.sent_opcode)) {
				case opcode::DeviceChannelOpcodePutOutput:
				case opcode::DeviceChannelOpcodeQueryDataCloud:
					free(data);
					break;
			}
		}
			break;
		default:break;
	}
}