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
            //TODO: this need to be removed because answer is synchronous
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
            if(synchronous_answer == NULL) return -1000;
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud*>(dataPack->channel_header_data);
            
            try {
                if (dataPack &&
                    dataPack->channel_data) {
                    void *result_data = NULL;
                    chaos_data::CDataWrapper query((char *)dataPack->channel_data);
                    opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr result_header = (DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr)calloc(sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult), 1);
                    
                     header->field.record_for_page = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.record_for_page);
                    
                    //decode the endianes off the data
                    std::string key = CDW_GET_SRT_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, "");
                    uint64_t start_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, getUInt64Value, 0);
                    uint64_t end_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, getUInt64Value, 0);
                    bool start_ts_is_included = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_START_TS_INCLUDED, getBoolValue, false);
                    //call server api if we have at least the key
                    if((key.compare("") != 0)) {err = handler->consumeDataCloudQuery(header,
                                                                                     key,
                                                                                     start_ts,
                                                                                     end_ts,
                                                                                     start_ts_is_included,
                                                                                     result_header,
                                                                                     &result_data);}
                    if(err == 0){
                        //set the result header and data
                        DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult));
                        DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_header->result_data_size);
                        result_header->result_data_size = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->result_data_size);
                        result_header->numer_of_record_found = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->numer_of_record_found);
                        result_header->last_daq_ts = TO_LITTEL_ENDNS_NUM(uint64_t, result_header->last_daq_ts);
                    } else {
                        if(result_data) free(result_data);
                        if(result_header) free(result_header);
                    }
                }
            } catch (...) {
                // inca se of error header an cdatawrapper are cleaned here
                
            }
            if(header) free(header);
            if(dataPack->channel_data) free(dataPack->channel_data);
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
