/*
 *	DirectIODeviceServerChannel.cpp
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
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>

namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIODeviceServerChannel, DirectIOVirtualServerChannel);


//define the static deallocator class
DirectIODeviceServerChannel::DirectIODeviceServerChannelDeallocator DirectIODeviceServerChannel::STATIC_DirectIODeviceServerChannelDeallocator;


DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index),
handler(NULL) {
    //set this class as delegate for the endpoint
    DirectIOVirtualServerChannel::setDelegate(this);
}

void DirectIODeviceServerChannel::setHandler(DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler *_handler) {
    handler = _handler;
}

int DirectIODeviceServerChannel::consumeDataPack(DirectIODataPack *dataPack,
                                                 DirectIODataPack *synchronous_answer,
                                                 DirectIODeallocationHandler **answer_header_deallocation_handler,
                                                 DirectIODeallocationHandler **answer_data_deallocation_handler) {
    CHAOS_ASSERT(handler)
    int err = -1;
    //set the clean handler
    *answer_data_deallocation_handler = *answer_header_deallocation_handler = &STATIC_DirectIODeviceServerChannelDeallocator;
    
    // get the opcode
    opcode::DeviceChannelOpcode  channel_opcode = static_cast<opcode::DeviceChannelOpcode>(dataPack->header.dispatcher_header.fields.channel_opcode);
    switch (channel_opcode) {
        case opcode::DeviceChannelOpcodePutOutput: {
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderPutOpcode* >(dataPack->channel_header_data);
            //reallign the pointer to the start of the key
            header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
            err = handler->consumePutEvent(header, dataPack->channel_data, dataPack->header.channel_data_size);
            break;
        }
        case opcode::DeviceChannelOpcodeGetLastOutput: {
            if(synchronous_answer == NULL) return -1000;
            //allocate variable for result
            void *result_data = NULL;
            opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderGetOpcode* >(dataPack->channel_header_data);
            opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header = (DirectIODeviceChannelHeaderGetOpcodeResult*)calloc(sizeof(DirectIODeviceChannelHeaderGetOpcodeResult), 1);
            
            //decode the endianes off the data
            header->field.endpoint = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.endpoint);
            header->field.p_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.p_port);
            header->field.s_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.s_port);
            header->field.address = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.address);
            
            err = handler->consumeGetEvent(header,
                                           dataPack->channel_data,
                                           dataPack->header.channel_data_size,
                                           result_header,
                                           &result_data);
            if(err == 0){
                //set the result header and data
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIODeviceChannelHeaderGetOpcodeResult))
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_header->value_len)
                result_header->value_len = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->value_len);
            } else {
                if(result_data) free(result_data);
                if(result_header) free(result_header);
            }
            break;
        }
        case opcode::DeviceChannelOpcodeQueryDataCloud: {
            chaos_data::CDataWrapper *query = NULL;
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud*>(dataPack->channel_header_data);
            
            try {
                query = new chaos_data::CDataWrapper((char *)dataPack->channel_data);
                if (query && header) {
                    //decode the endianes off the data
                    header->field.endpoint = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.endpoint);
                    header->field.p_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.p_port);
                    header->field.s_port = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.s_port);
                    header->field.address = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.address);
                    
                    std::string key = query->hasKey(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING)?query->getStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING):"";
                    uint64_t start_ts = query->hasKey(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64)?query->getUInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64):0;
                    uint64_t end_ts = query->hasKey(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64)?query->getUInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64):0;
                    
                    //call server api if we have at least the key
                    if((key.compare("") != 0)) err = handler->consumeDataCloudQuery(header, key, start_ts, end_ts);
                }else {
                    
                }
            } catch (...) {
                // inca se of error header an cdatawrapper are cleaned here
                
                if(header) free(header);
            }
            
            //data and query are deleted in anyway server api manage only the header
            if(dataPack->channel_data) free(dataPack->channel_data);
            if(query) delete(query);
            break;
        }
            
        case opcode::DeviceChannelOpcodeQueryDataCloudStartResult:{
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResultPtr header =
            reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResultPtr>(dataPack->channel_header_data);
            header->field.total_element_found = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.total_element_found);
            err = handler->consumeDataCloudQueryStartResult(header);
            break;
        }
            
        case opcode::DeviceChannelOpcodeQueryDataCloudResult: {
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr header =
            reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr>(dataPack->channel_header_data);
            
            //decode the endianes off the data
            
            header->field.element_index = FROM_LITTLE_ENDNS_NUM(uint64_t, header->field.element_index);
            
            //call server api
            err = handler->consumeDataCloudQueryResult(header,
                                                       dataPack->channel_data,
                                                       dataPack->header.channel_data_size);
            break;
        }
            
        case opcode::DeviceChannelOpcodeQueryDataCloudEndResult:{
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResultPtr header =
            reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResultPtr>(dataPack->channel_header_data);
            header->field.error = FROM_LITTLE_ENDNS_NUM(int32_t, header->field.error);
            header->field.error_message_length = FROM_LITTLE_ENDNS_NUM(int32_t, header->field.error_message_length);
            err = handler->consumeDataCloudQueryEndResult(header,
                                                          dataPack->channel_data,
                                                          dataPack->header.channel_data_size);
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
void DirectIODeviceServerChannel::DirectIODeviceServerChannelDeallocator::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
    switch(free_info_ptr->sent_part) {
        case DisposeSentMemoryInfo::SentPartHeader:{
            free(sent_data_ptr);
            break;
        }
            
        case DisposeSentMemoryInfo::SentPartData: {
            free(sent_data_ptr);
            break;
        }
    }
}
