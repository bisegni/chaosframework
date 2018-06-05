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
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/DataBuffer.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIServerChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIOSystemAPIServerChannel, DirectIOVirtualServerChannel);

DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIOSystemAPI_Channel_Index),
handler(NULL) {
    //set this class as delegate for the endpoint
    DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIOSystemAPIServerChannel::setHandler(DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler *_handler) {
    handler = _handler;
}

int DirectIOSystemAPIServerChannel::consumeDataPack(DirectIODataPackSPtr data_pack,
                                                    DirectIODataPackSPtr& synchronous_answer) {
    CHAOS_ASSERT(handler)
    int err = -1;
    
    // the opcode
    opcode::SystemAPIChannelOpcode  channel_opcode = static_cast<opcode::SystemAPIChannelOpcode>(data_pack->header.dispatcher_header.fields.channel_opcode);
    
    switch (channel_opcode) {
        case opcode::SystemAPIChannelOpcodeEcho: {
            synchronous_answer = ChaosMakeSharedPtr<DirectIODataPack>();
            BufferSPtr answer_channel_data;
            err =  handler->consumeEchoEvent(ChaosMoveOperator(data_pack->channel_data),
                                             answer_channel_data);
            
            if(!err &&
               answer_channel_data &&
               answer_channel_data->size()){
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, answer_channel_data, (uint32_t)answer_channel_data->size());
            } else {
                err = -2;
            }
            break;
        }
        case opcode::SystemAPIChannelOpcodeNewSnapshotDataset: {
            return -10000;
            break;
        }
            
        case opcode::SystemAPIChannelOpcodeDeleteSnapshotDataset: {
            return -10000;
            break;
        }
            
        case opcode::SystemAPIChannelOpcodeGetSnapshotDatasetForAKey: {
            std::string producer_key;
            BufferSPtr channel_found_data;
            synchronous_answer = ChaosMakeSharedPtr<DirectIODataPack>();
            //allocate the answer header
            BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIOSystemAPISnapshotResultHeader));
            
            //read the request header
            DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header = data_pack->channel_header_data->data<DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader>();
            header->field.producer_key_set_len = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.producer_key_set_len);
            
            //chec if a producere key has been forwarded
            if(data_pack->header.channel_data_size) {
                //set error
                producer_key.assign((const char*)data_pack->channel_data->data(), data_pack->header.channel_data_size);
            }
            
            //call the handler
            err = handler->consumeGetDatasetSnapshotEvent(*header,
                                                          producer_key,
                                                          channel_found_data,
                                                          *result_header->data<DirectIOSystemAPISnapshotResultHeader>());
            //whe have data to return
            if(err == 0){
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, (uint32_t)result_header->size());
                //set the result header
                result_header->data<DirectIOSystemAPISnapshotResultHeader>()->error = TO_LITTEL_ENDNS_NUM(int32_t, result_header->data<DirectIOSystemAPISnapshotResultHeader>()->error);
                result_header->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len = (uint32_t)channel_found_data->size();
                if(result_header->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len) {
                    //asosciate the pointer to the datapack to be returned
                    DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, channel_found_data, result_header->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len);
                }
                result_header->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<DirectIOSystemAPISnapshotResultHeader>()->channel_data_len);
                
            }
            break;
        }
            
        case opcode::SystemAPIChannelOpcodePushLogEntryForANode: {
            synchronous_answer = ChaosMakeSharedPtr<DirectIODataPack>();
            ChaosStringVector log_entry;
            DataBuffer<> buffer(data_pack->channel_data->detach(),
                                data_pack->header.channel_data_size);
            //read the request header
            DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader *header = data_pack->channel_header_data->data<DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader>();
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
            buffer.release();
            return err;
        }
        default:
            break;
    }
    //return no result
    return err;
}

