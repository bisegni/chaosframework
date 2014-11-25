/*
 *	DirectIOSystemAPIServerChannel.cpp
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
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;


DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIOSystemAPI_Channel_Index),
handler(NULL) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIOSystemAPIServerChannel::setHandler(DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler *_handler) {
	handler = _handler;
}

int DirectIOSystemAPIServerChannel::consumeDataPack(DirectIODataPack *dataPack,
													DirectIOSynchronousAnswerPtr synchronous_answer) {
	CHAOS_ASSERT(handler)
	int err = 0;
	// the opcode
	opcode::SystemAPIChannelOpcode  channel_opcode = static_cast<opcode::SystemAPIChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
	
	switch (channel_opcode) {
		case opcode::SystemAPIChannelOpcodeNewSnapshotDataset: {
			//set the answer pointer
			synchronous_answer->answer_data = std::calloc(sizeof(DirectIOSystemAPISnapshotResult), 1);
			synchronous_answer->answer_size = sizeof(DirectIOSystemAPISnapshotResult);
			
			//get the header
			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);

			//call the handler
			handler->consumeNewSnapshotEvent(header,
											 dataPack->channel_data,
											 dataPack->header.channel_data_size,
											 (DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data);
			//fix endianes into api result
			((DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data)->error =
			TO_LITTE_ENDNS_NUM(int32_t, ((DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data)->error);
			break;
		}
			
		case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset: {
			//set the answer pointer
			synchronous_answer->answer_data = std::calloc(sizeof(DirectIOSystemAPISnapshotResult), 1);
			synchronous_answer->answer_size = sizeof(DirectIOSystemAPISnapshotResult);
			
			//get the header
			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
			
			//call the handler
			handler->consumeDeleteSnapshotEvent(header,
												(DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data);
			//fix endianes into api result
			((DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data)->error =
			TO_LITTE_ENDNS_NUM(int32_t, ((DirectIOSystemAPISnapshotResult*)synchronous_answer->answer_data)->error);
			break;
		}
			
		case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey: {
			std::string producer_key;
			void *channel_found_data = NULL;
			uint32_t channel_found_data_length = 0;
			
			//get the header
			DirectIOSystemAPIGetDatasetSnapshotResult *result_data = (DirectIOSystemAPIGetDatasetSnapshotResult*)std::calloc(sizeof(DirectIOSystemAPIGetDatasetSnapshotResult), 1);
			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
			
			//chec if a producere key has been forwarded
			if(dataPack->header.channel_data_size) {
				//set error
				producer_key.assign((const char*)dataPack->channel_data, dataPack->header.channel_data_size);
				//delete the memory where is located producer key
				free(dataPack->channel_data);
			}
			
			//call the handler
			handler->consumeGetDatasetSnapshotEvent(header,
													producer_key,
													&channel_found_data,
													channel_found_data_length,
													&result_data->api_result);
			//set the answer pointer
			if(channel_found_data) {
				//whe have data to return
				result_data = (DirectIOSystemAPIGetDatasetSnapshotResult*)std::realloc(result_data, (synchronous_answer->answer_size = (channel_found_data_length+sizeof(DirectIOSystemAPIGetDatasetSnapshotResult))));
				synchronous_answer->answer_data = result_data;

				std::memcpy(((char*)result_data+sizeof(DirectIOSystemAPISnapshotResult)+4), channel_found_data, channel_found_data_length);
				((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->channel_len = channel_found_data_length;
				free(channel_found_data);
			}
			//fix endianes into api result
			((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->api_result.error =
			TO_LITTE_ENDNS_NUM(int32_t, ((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->api_result.error);
			break;
		}
		default:
			break;
	}
	
	//only data pack is deleted, header and data of the channel are managed by handler
	free(dataPack);
	
	//return no result
	return err;
}
