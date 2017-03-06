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

#include <chaos/common/global.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/DataBuffer.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
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
            return -10000;
            break;
        }
            
        case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset: {
            return -10000;
            break;
        }
            
        case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey: {
            if(synchronous_answer == NULL) return -1000;
            std::string producer_key;
            void *channel_found_data = NULL;
            
            //allocate the answer header
            DirectIOSystemAPISnapshotResultHeader *result_header = (DirectIOSystemAPISnapshotResultHeader*)calloc(sizeof(DirectIOSystemAPISnapshotResultHeader), 1);
            
            //read the request header
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
            err = handler->consumeGetDatasetSnapshotEvent(header,
                                                          producer_key,
                                                          &channel_found_data,
                                                          result_header->channel_data_len,
                                                          *result_header);
            //whe have data to return
            if(err == 0){
                //set the result header
                result_header->error = TO_LITTEL_ENDNS_NUM(int32_t, result_header->error);
                if(result_header->channel_data_len) {
                    //asosciate the pointer to the datapack to be returned
                    DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, channel_found_data, result_header->channel_data_len);
                }
                result_header->channel_data_len = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->channel_data_len);
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIOSystemAPISnapshotResultHeader))
            }
            break;
        }
            
        case opcode::SystemAPIChannelOpcodePushLogEntryForANode: {
            if(synchronous_answer != NULL) return -1000;
            ChaosStringVector log_entry;
            DataBuffer<> buffer(dataPack->channel_data,
                                dataPack->header.channel_data_size);
            //read the request header
            opcode_headers::DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr header = reinterpret_cast< opcode_headers::DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr >(dataPack->channel_header_data);
            header->field.data_entries_num = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.data_entries_num);
            
            //now we can read data
            uint32_t node_name_size = (uint32_t)FROM_LITTLE_ENDNS_NUM(uint32_t, buffer.readInt32());
            std::string node_name = buffer.readString(node_name_size);
            
            //read all entry
            uint32_t entry_len = 0;
            for(int idx = 0;
                idx < header->field.data_entries_num;
                idx++) {
                entry_len = (uint32_t)FROM_LITTLE_ENDNS_NUM(uint32_t, buffer.readInt32());
                log_entry.push_back(buffer.readString(entry_len));
            }
            
            //call the handler
            err = handler->consumeLogEntries(node_name,
                                             log_entry);
            
            if(header) free(header);
            if(dataPack->channel_data) free(dataPack->channel_data);
            return err;
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
                case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset:
                case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey:
                case opcode::SystemAPIChannelOpcodePushLogEntryForANode:
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
                case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset:
                    break;
                    
                case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey:
                case opcode::SystemAPIChannelOpcodePushLogEntryForANode:
                    if(sent_data_ptr) free(sent_data_ptr);
                    break;
                default:
                    break;
            }
            break;
        }
    }
}
