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

#include <chaos/common/global.h>

#include <chaos/common/utility/DataBuffer.h>

#include <chaos/common/direct_io/channel/DirectIOSystemAPIClientChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIOSystemAPIClientChannel, DirectIOVirtualClientChannel);

#define DIOSCC_INFO INFO_LOG(DirectIOSystemAPIClientChannel)
#define DIOSCC_DBG DBG_LOG(DirectIOSystemAPIClientChannel)
#define DIOSCC_ERR ERR_LOG(DirectIOSystemAPIClientChannel)

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

//! get the snapshot for one or more producer key
int64_t DirectIOSystemAPIClientChannel::getDatasetSnapshotForProducerKey(const std::string& snapshot_name,
                                                                         const std::string& producer_key,
                                                                         uint32_t channel_type,
                                                                         DirectIOSystemAPIGetDatasetSnapshotResult **api_result_handle) {
    int64_t err = 0;
    if(snapshot_name.size() > 255) {
        //bad Snapshot name size
        return -1000;
    }
    //allocate the datapack
    DirectIODataPack *answer = NULL;
    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
    
    //allocate the header
    DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr get_snapshot_opcode_header =
    (DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr)calloc(sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader), 1);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey);
    
    //copy the snapshot name to the header
    std::strncpy(get_snapshot_opcode_header->field.snap_name, snapshot_name.c_str(), 255);
    get_snapshot_opcode_header->field.channel_type = channel_type;
    if(api_result_handle){
        *api_result_handle=NULL;
    }
    
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, get_snapshot_opcode_header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
    if(producer_key.size()) {
        
        //set the header field for the producer concatenation string
        get_snapshot_opcode_header->field.producer_key_set_len = TO_LITTEL_ENDNS_NUM(uint32_t, (uint32_t)producer_key.size());
        
        //copy the memory for forwarding buffer
        void * producer_key_send_buffer = malloc(producer_key.size());
        std::memcpy(producer_key_send_buffer, producer_key.c_str(), producer_key.size());
        //set as data
        DIRECT_IO_SET_CHANNEL_DATA(data_pack, producer_key_send_buffer, (uint32_t)producer_key.size());
    }
    //send data with synchronous answer flag
    if((err = (int)sendServiceData(data_pack, &answer))) {
        //error getting last value
        DIOSCC_ERR << "Error on sendServiceData execution with error:" <<err;
    } else {
        //we got answer
        if(answer) {
            *api_result_handle = (DirectIOSystemAPIGetDatasetSnapshotResult*)calloc(sizeof(DirectIOSystemAPIGetDatasetSnapshotResult), 1);
            //get the header
            opcode_headers::DirectIOSystemAPISnapshotResultHeaderPtr result_header = static_cast<opcode_headers::DirectIOSystemAPISnapshotResultHeaderPtr>(answer->channel_header_data);
            if(result_header){
                result_header->channel_data_len = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->channel_data_len);
                result_header->error = FROM_LITTLE_ENDNS_NUM(int32_t, result_header->error);
                
                (*api_result_handle)->api_result = *result_header;
                (*api_result_handle)->channel_data = answer->channel_data;
            } else {
                err=-2;
                DIOSCC_ERR << "## INTERNAL ERROR: NO RESULT HEADER";
                
            }
            
        }
    }
    if(answer) {
        if(answer->channel_header_data) free(answer->channel_header_data);
        free(answer);
    }
    return err;
}

int64_t DirectIOSystemAPIClientChannel::pushLogEntries(const std::string& node_name,
                                                       const ChaosStringVector& log_entries) {
    if(log_entries.size() == 0) {
        //bad node name
        return -1000;
    }
    
    int64_t err = 0;
    DataBuffer<> buffer;
    int32_t tmp_element_size = 0;
    
    //allocate the header
    DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr header =
    (DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr)calloc(sizeof(DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr), 1);
    
    //allocate the datapack
    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodePushLogEntryForANode);
    
    //write the number of the entry in the header
    header->field.data_entries_num = FROM_LITTLE_ENDNS_NUM(uint32_t, (uint32_t)log_entries.size());
    
    //encode node name
    buffer.writeInt32(FROM_LITTLE_ENDNS_NUM(uint32_t, tmp_element_size = (uint32_t)node_name.size()));
    buffer.writeByte((const char *)node_name.c_str(), tmp_element_size);

    //add the entry
    for(ChaosStringVectorConstIterator it = log_entries.begin(),
        end = log_entries.end();
        it != end;
        it++) {
        const std::string& entry = *it;
        
        buffer.writeInt32(FROM_LITTLE_ENDNS_NUM(uint32_t, tmp_element_size = (uint32_t)entry.size()));
        buffer.writeByte((const char *)entry.c_str(), tmp_element_size);
    }
    
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
    //set as data
    int32_t data_len = buffer.getSize();
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, buffer.release(), data_len);
    //send data with synchronous answer flag
    if((err = (int)sendPriorityData(data_pack))) {
        //error getting last value
        DIOSCC_ERR << "Error on sendServiceData execution with error:" <<err;
    }
    return err;
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
                case opcode::SystemAPIChannelOpcodePushLogEntryForANode:
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
                case opcode::SystemAPIChannelOpcodePushLogEntryForANode:
                    free(sent_data_ptr);
                    break;
                default:
                    break;
            }
            break;
        }
    }
}
