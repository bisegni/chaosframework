/*
 *	DirectIOSystemAPIClientChannel.cpp
 *	!CHAOS
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
#include <chaos/common/direct_io/channel/DirectIOSystemAPIClientChannel.h>

namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIOSystemAPIClientChannel, DirectIOVirtualClientChannel);

//define the static deallocator class
DirectIOSystemAPIClientChannel::DirectIOSystemAPIClientChannelDeallocator
DirectIOSystemAPIClientChannel::STATIC_DirectIOSystemAPIClientChannelDeallocator;

//base constructor
DirectIOSystemAPIClientChannel::DirectIOSystemAPIClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIOSystemAPI_Channel_Index) {
	//associate the default static allocator
	header_deallocator = &STATIC_DirectIOSystemAPIClientChannelDeallocator;
}

//base destructor
DirectIOSystemAPIClientChannel::~DirectIOSystemAPIClientChannel() {
	
}

// start a new Snapshot creation
int64_t DirectIOSystemAPIClientChannel::makeNewDatasetSnapshot(const std::string& snapshot_name,
															   const std::vector<std::string>& producer_keys,
															   DirectIOSystemAPIGetDatasetSnapshotResult **api_result_handle) {
	int64_t err = 0;
//	DirectIODataPack *answer = NULL;
//	if(snapshot_name.size() > 255) {
//		//bad Snapshot name size
//		return -1000;
//	}
//	//allocate the datapack
//	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
//	
//	//allocate the header
//	DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr new_snapshot_opcode_header =
//	(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr)calloc(sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader), 1);
//	
//	//set opcode
//	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeNewSnapshotDataset);
//	
//		//copy the snapshot name to the header
//	std::memcpy(new_snapshot_opcode_header->field.snap_name, snapshot_name.c_str(), snapshot_name.size());
//
//	//set header
//	DIRECT_IO_SET_CHANNEL_HEADER(data_pack, new_snapshot_opcode_header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
//	if(producer_keys.size()) {
//		//we have also a set of producer key so senti it in the data part of message
//		std::string producer_key_concatenation;
//		for(std::vector<std::string>::const_iterator it = producer_keys.begin();
//			it != producer_keys.end();) {
//			//add key
//			producer_key_concatenation.append(*it);
//			
//			if((++it) != producer_keys.end()) {
//				producer_key_concatenation.append(",");
//			}
//		}
//		//set the header field for the producer concatenation string
//		new_snapshot_opcode_header->field.producer_key_set_len = TO_LITTE_ENDNS_NUM(uint32_t, (uint32_t)producer_key_concatenation.size());
//
//		//copy the memory for forwarding buffer
//		void * producer_key_concatenation_memory = calloc(producer_key_concatenation.size(), 1);
//		std::memcpy(producer_key_concatenation_memory, producer_key_concatenation.c_str(), producer_key_concatenation.size());
//		//set as data
//		DIRECT_IO_SET_CHANNEL_DATA(data_pack, producer_key_concatenation_memory, (uint32_t)producer_key_concatenation.size());
//	}
//	//send data with synchronous answer flag
//	if((err = (int)sendServiceData(data_pack, &answer))) {
//		//error getting last value
//		if(answer && answer->answer_data) free(answer->answer_data);
//	} else {
//		//we got answer
//		if(answer && answer->answer_size == sizeof(DirectIOSystemAPISnapshotResult)) {
//			*api_result_handle  = static_cast<DirectIOSystemAPISnapshotResult*>(answer->answer_data);
//			(*api_result_handle)->error = FROM_LITTLE_ENDNS_NUM(int32_t, (*api_result_handle)->error);
//		} else {
//			*api_result_handle = NULL;
//		}
//	}
//	if(answer) free(answer);
	return err;
}

//! delete the snapshot identified by name
int64_t DirectIOSystemAPIClientChannel::deleteDatasetSnapshot(const std::string& snapshot_name,
															  DirectIOSystemAPIGetDatasetSnapshotResult **api_result_handle) {
	int64_t err = 0;
//	DirectIODataPack *answer = NULL;
//	if(snapshot_name.size() > 255) {
//		//bad Snapshot name size
//		return -1000;
//	}
//	//allocate the datapack
//	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
//	
//	//allocate the header
//	DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr new_snapshot_opcode_header =
//	(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr)calloc(sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader), 1);
//	
//	//set opcode
//	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset);
//	
//	//copy the snapshot name to the header
//	std::memcpy(new_snapshot_opcode_header->field.snap_name, snapshot_name.c_str(), snapshot_name.size());
//	
//	//set header
//	DIRECT_IO_SET_CHANNEL_HEADER(data_pack, new_snapshot_opcode_header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
//	
//	//send data with synchronous answer flag
//	if((err = (int)sendServiceData(data_pack, &answer))) {
//		//error getting last value
//		if(answer && answer->answer_data) free(answer->answer_data);
//	} else {
//		//we got answer
//		if(answer && answer->answer_size == sizeof(DirectIOSystemAPISnapshotResult)) {
//			*api_result_handle  = static_cast<DirectIOSystemAPISnapshotResult*>(answer->answer_data);
//			(*api_result_handle)->error = FROM_LITTLE_ENDNS_NUM(int32_t, (*api_result_handle)->error);
//		} else {
//			*api_result_handle = NULL;
//		}
//	}
//	if(answer) free(answer);
	return err;
}

//! get the snapshot for one or more producer key
int64_t DirectIOSystemAPIClientChannel::getDatasetSnapshotForProducerKey(const std::string& snapshot_name,
																		 const std::string& producer_key,
																		 uint32_t channel_type,
																		 DirectIOSystemAPIGetDatasetSnapshotResult **api_result_handle) {
	int64_t err = 0;
//	DirectIODataPack *answer = NULL;
//	if(snapshot_name.size() > 255) {
//		//bad Snapshot name size
//		return -1000;
//	}
//	//allocate the datapack
//	DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
//	
//	//allocate the header
//	DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr get_snapshot_opcode_header =
//	(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr)calloc(sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader), 1);
//	
//	//set opcode
//	data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey);
//	
//	//copy the snapshot name to the header
//	std::memcpy(get_snapshot_opcode_header->field.snap_name, snapshot_name.c_str(), snapshot_name.size());
//	get_snapshot_opcode_header->field.channel_type = channel_type;
//	
//	//set header
//	DIRECT_IO_SET_CHANNEL_HEADER(data_pack, get_snapshot_opcode_header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
//	if(producer_key.size()) {
//		
//		//set the header field for the producer concatenation string
//		get_snapshot_opcode_header->field.producer_key_set_len = TO_LITTE_ENDNS_NUM(uint32_t, (uint32_t)producer_key.size());
//		
//		//copy the memory for forwarding buffer
//		void * producer_key_send_buffer = malloc(producer_key.size());
//		std::memcpy(producer_key_send_buffer, producer_key.c_str(), producer_key.size());
//		//set as data
//		DIRECT_IO_SET_CHANNEL_DATA(data_pack, producer_key_send_buffer, (uint32_t)producer_key.size());
//	}
//	//send data with synchronous answer flag
//	if((err = (int)sendServiceData(data_pack, &answer))) {
//		//error getting last value
//		if(answer && answer->answer_data) free(answer->answer_data);
//	} else {
//		//we got answer
//		if(answer && answer->answer_data) {
//			*api_result_handle  = static_cast<DirectIOSystemAPIGetDatasetSnapshotResult*>(answer->answer_data);
//			(*api_result_handle)->api_result.error = FROM_LITTLE_ENDNS_NUM(int32_t, (*api_result_handle)->api_result.error);
//		} else {
//			*api_result_handle = NULL;
//		}
//	}
//	if(answer) free(answer);
//	return err;
	return 0;
}

//! default data deallocator implementation
void DirectIOSystemAPIClientChannel::DirectIOSystemAPIClientChannelDeallocator::freeSentData(void* sent_data_ptr,
																							 DisposeSentMemoryInfo *free_info_ptr) {
	switch(free_info_ptr->sent_part) {
		case DisposeSentMemoryInfo::SentPartHeader:{
			switch(static_cast<opcode::SystemAPIChannelOpcode>(free_info_ptr->sent_opcode)) {
				case opcode::SystemAPIChannelOpcodeNewSnapshotDataset:
				case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset:
				case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey:
					free(sent_data_ptr);
					break;
				default:
					break;
			}
			break;
		}
			
		case DisposeSentMemoryInfo::SentPartData: {
			switch(static_cast<opcode::SystemAPIChannelOpcode>(free_info_ptr->sent_opcode)) {
				case opcode::SystemAPIChannelOpcodeNewSnapshotDataset:
				case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey:
					free(sent_data_ptr);
					break;
				default:
					break;
			}
			break;
		}
	}
}