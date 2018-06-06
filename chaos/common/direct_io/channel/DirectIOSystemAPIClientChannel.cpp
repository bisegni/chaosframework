/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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

//base constructor
DirectIOSystemAPIClientChannel::DirectIOSystemAPIClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIOSystemAPI_Channel_Index) {}

//base destructor
DirectIOSystemAPIClientChannel::~DirectIOSystemAPIClientChannel() {}

int DirectIOSystemAPIClientChannel::echo(chaos::common::data::BufferSPtr message,
                                         chaos::common::data::BufferSPtr& echo_message) {
    int err = 0;
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeEcho);
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, message, (uint32_t)message->size());
    if((err = (int)sendServiceData(ChaosMoveOperator(data_pack), answer))) {
        //error getting last value
        DIOSCC_ERR << "Error on echo api execution with error:" <<err;
    } else if(answer) {
        echo_message = answer->channel_data;
    }
    return err;
}

//! get the snapshot for one or more producer key
int DirectIOSystemAPIClientChannel::getDatasetSnapshotForProducerKey(const std::string& snapshot_name,
                                                                     const std::string& producer_key,
                                                                     uint32_t channel_type,
                                                                     DirectIOSystemAPIGetDatasetSnapshotResult& api_result_handle) {
    int err = 0;
    if(snapshot_name.size() > 255) {
        //bad Snapshot name size
        return -1000;
    }
    //allocate the datapack
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    
    //allocate the header
    BufferSPtr get_snapshot_opcode_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader));
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey);
    
    //copy the snapshot name to the header
    std::strncpy(get_snapshot_opcode_header->data<DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader>()->field.snap_name, snapshot_name.c_str(), 255);
    get_snapshot_opcode_header->data<DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader>()->field.channel_type = channel_type;
    
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, get_snapshot_opcode_header, sizeof(DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader))
    if(producer_key.size()) {
        
        //set the header field for the producer concatenation string
        get_snapshot_opcode_header->data<DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader>()->field.producer_key_set_len = TO_LITTEL_ENDNS_NUM(uint32_t, (uint32_t)producer_key.size());
        
        //copy the memory for forwarding buffer
        BufferSPtr producer_key_send_buffer = ChaosMakeSharedPtr<Buffer>();
        producer_key_send_buffer->append(producer_key.c_str(), producer_key.size());
        //set as data
        DIRECT_IO_SET_CHANNEL_DATA(data_pack, producer_key_send_buffer, (uint32_t)producer_key.size());
    }
    //send data with synchronous answer flag
    if((err = (int)sendServiceData(ChaosMoveOperator(data_pack), answer))) {
        //error getting last value
        DIOSCC_ERR << "Error on sendServiceData execution with error:" <<err;
    } else {
        //we got answer
        if(answer) {
            
            //get the header
            if(answer->channel_header_data){
                answer->channel_header_data->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len = FROM_LITTLE_ENDNS_NUM(uint32_t, answer->channel_header_data->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len);
                answer->channel_header_data->data<DirectIOSystemAPISnapshotResultHeader>()->error = FROM_LITTLE_ENDNS_NUM(int32_t, answer->channel_header_data->data<DirectIOSystemAPISnapshotResultHeader>()->error);
                
                api_result_handle.api_result = *answer->channel_header_data->data<DirectIOSystemAPISnapshotResultHeader>();
                api_result_handle.channel_data = ChaosMakeSharedPtr<CDataWrapper>(answer->channel_data->data());
            } else {
                err=-2;
                DIOSCC_ERR << "## INTERNAL ERROR: NO RESULT HEADER";
            }
        }
    }
    return err;
}

int DirectIOSystemAPIClientChannel::pushLogEntries(const std::string& node_name,
                                                   const ChaosStringVector& log_entries) {
    if(log_entries.size() == 0) {
        //bad node name
        return -1000;
    }
    
    int err = 0;
    DataBuffer<> buffer;
    int32_t tmp_element_size = 0;
    
    //allocate the header
    BufferSPtr header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader));
    
    //allocate the datapack
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::SystemAPIChannelOpcodePushLogEntryForANode);
    
    //write the number of the entry in the header
    header->data<DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader>()->field.data_entries_num = FROM_LITTLE_ENDNS_NUM(uint32_t, (uint32_t)log_entries.size());
    
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
    int32_t data_len = buffer.getCursorLocation();
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer.release(), data_len, data_len, true);
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, header, header->size())
    //set as data
    
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, channel_data->size());
    //send data with synchronous answer flag
    if((err = (int)sendPriorityData(ChaosMoveOperator(data_pack)))) {
        //error getting last value
        DIOSCC_ERR << "Error on sendServiceData execution with error:" <<err;
    }
    return err;
}
