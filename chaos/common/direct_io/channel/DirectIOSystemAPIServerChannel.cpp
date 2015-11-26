/*
 *	DirectIOSystemAPIServerChannel.cpp
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

#include <chaos/common/utility/endianess.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIOSystemAPIServerChannel, DirectIOVirtualServerChannel);

//define the static deallocator class
DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelDeallocator DirectIOSystemAPIServerChannel::STATIC_DirectIOSystemAPIServerChannelDeallocator;

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
													DirectIODataPack *synchronous_answer,
                                                    DirectIODeallocationHandler **answer_header_deallocation_handler,
                                                    DirectIODeallocationHandler **answer_data_deallocation_handler) {
	CHAOS_ASSERT(handler)
	int err = -1;
    
    //set the clean handler
    *answer_data_deallocation_handler = *answer_header_deallocation_handler = &STATIC_DirectIOSystemAPIServerChannelDeallocator;
    
	// the opcode
	opcode::SystemAPIChannelOpcode  channel_opcode = static_cast<opcode::SystemAPIChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
	
	switch (channel_opcode) {
		case opcode::SystemAPIChannelOpcodeNewSnapshotDataset: {
            if(synchronous_answer == NULL) return -1000;
            
			//get the header
            DirectIOSystemAPISnapshotResultHeader *result_header = (DirectIOSystemAPISnapshotResultHeader*)calloc(sizeof(DirectIOSystemAPISnapshotResultHeader), 1);
			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);

			//call the handler
			err = handler->consumeNewSnapshotEvent(header,
											 dataPack->channel_data,
											 dataPack->header.channel_data_size,
											 *result_header);
            
            if(err == 0){
                //set the result header
                //fix endianes into api result
                result_header->error = TO_LITTEL_ENDNS_NUM(int32_t, result_header->error);
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIOSystemAPISnapshotResultHeader))
            }
			break;
		}
			
		case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset: {
			//set the answer pointer
			DirectIOSystemAPISnapshotResultHeader *result_header = (DirectIOSystemAPISnapshotResultHeader*)calloc(sizeof(DirectIOSystemAPISnapshotResultHeader), 1);
			
			//get the header
			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
			
			//call the handler
			err = handler->consumeDeleteSnapshotEvent(header,
                                                      *result_header);
            if(err == 0){
                //set the result header
                //fix endianes into api result
                result_header->error = TO_LITTEL_ENDNS_NUM(int32_t, result_header->error);
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIOSystemAPISnapshotResultHeader))
            }
			break;
		}
			
		case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey: {
//			std::string producer_key;
//			void *channel_found_data = NULL;
//			uint32_t channel_found_data_length = 0;
//			
//			//get the header
//			DirectIOSystemAPIGetDatasetSnapshotResult *result_data = (DirectIOSystemAPIGetDatasetSnapshotResult*)std::calloc(sizeof(DirectIOSystemAPIGetDatasetSnapshotResult), 1);
//			synchronous_answer->answer_data = result_data;
//			synchronous_answer->answer_size = sizeof(DirectIOSystemAPIGetDatasetSnapshotResult);
//			
//			opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr >(dataPack->channel_header_data);
//			header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
//			
//			//chec if a producere key has been forwarded
//			if(dataPack->header.channel_data_size) {
//				//set error
//				producer_key.assign((const char*)dataPack->channel_data, dataPack->header.channel_data_size);
//				//delete the memory where is located producer key
//				free(dataPack->channel_data);
//			}
//			
//			//call the handler
//			handler->consumeGetDatasetSnapshotEvent(header,
//													producer_key,
//													&channel_found_data,
//													channel_found_data_length,
//													&result_data->api_result);
//			//set the answer pointer
//			if(channel_found_data) {
//				//whe have data to return
//				result_data = (DirectIOSystemAPIGetDatasetSnapshotResult*)std::realloc(result_data, (synchronous_answer->answer_size = (channel_found_data_length+sizeof(DirectIOSystemAPIGetDatasetSnapshotResult))));
//				synchronous_answer->answer_data = result_data;
//
//				std::memcpy(((char*)result_data+sizeof(DirectIOSystemAPISnapshotResult)+4), channel_found_data, channel_found_data_length);
//				((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->channel_len = channel_found_data_length;
//				free(channel_found_data);
//			}
//			//fix endianes into api result
//			((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->api_result.error =
//			TO_LITTEL_ENDNS_NUM(int32_t, ((DirectIOSystemAPIGetDatasetSnapshotResult*)synchronous_answer->answer_data)->api_result.error);
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

//! default data deallocator implementation
void DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelDeallocator::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
    switch(free_info_ptr->sent_part) {
            case DisposeSentMemoryInfo::SentPartHeader:{
                switch (free_info_ptr->sent_opcode) {
                        case opcode::SystemAPIChannelOpcodeNewSnapshotDataset:
                        free(sent_data_ptr);
                        break;
                        
                    default:
                        break;
                }
                break;
            }
            
            case DisposeSentMemoryInfo::SentPartData: {
                switch (free_info_ptr->sent_opcode) {
                        case opcode::SystemAPIChannelOpcodeNewSnapshotDataset:
                        break;
                        
                    default:
                        break;
                }
                break;
            }
    }
}