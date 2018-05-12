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
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

DEFINE_CLASS_FACTORY(DirectIODeviceServerChannel, DirectIOVirtualServerChannel);


//define the static deallocator class
DirectIODeviceServerChannel::DirectIODeviceServerChannelDeallocator DirectIODeviceServerChannel::STATIC_DirectIODeviceServerChannelDeallocator;


DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index),
handler(NULL) {DirectIOVirtualServerChannel::setDelegate(this);}

void DirectIODeviceServerChannel::setHandler(DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler *_handler) {handler = _handler;}

int DirectIODeviceServerChannel::consumeDataPack(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                 chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    CHAOS_ASSERT(handler)
    int err = -1;

    // get the opcode
    opcode::DeviceChannelOpcode  channel_opcode = static_cast<opcode::DeviceChannelOpcode>(data_pack->header.dispatcher_header.fields.channel_opcode);
    switch (channel_opcode) {
        case opcode::DeviceChannelOpcodePutOutput: {
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderPutOpcode* >(data_pack->channel_header_data->data());
            //reallign the pointer to the start of the key
            header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
            err = handler->consumePutEvent(header,
                                           data_pack->channel_data,
                                           data_pack->header.channel_data_size);
            break;
        }

        case opcode::DeviceChannelOpcodePutHeathData: {
            opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderPutOpcode* >(data_pack->channel_header_data->data());
            //reallign the pointer to the start of the key
            header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
            err = handler->consumeHealthDataEvent(header,
                                                  data_pack->channel_data,
                                                  data_pack->header.channel_data_size);
            break;
        }

        case opcode::DeviceChannelOpcodeGetLastOutput: {
            if(synchronous_answer == NULL) return -1000;
            //allocate variable for result
            BufferSPtr result_data;
            opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderGetOpcode* >(data_pack->channel_header_data->data());
            BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderGetOpcodeResult));

            err = handler->consumeGetEvent(header,
                                           data_pack->channel_data,
                                           data_pack->header.channel_data_size,
                                           result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>(),
                                           result_data);
            if(err == 0){
                //set the result header and data
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIODeviceChannelHeaderGetOpcodeResult))
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len)
                result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len);
            }
            break;
        }

        case opcode::DeviceChannelOpcodeMultiGetLastOutput: {
            if(synchronous_answer == NULL) return -1000;
            //allocate variable for result
            uint32_t result_data_size = 0;
            BufferSPtr result_data;
            opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode *header = reinterpret_cast< opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode* >(data_pack->channel_header_data->data());
            BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult));
            
            //fetch the set of keys
            DataBuffer<> data_buffer(data_pack->channel_data->data(),
                                     data_pack->header.channel_data_size);
            data_pack->channel_data.reset();

            ChaosStringVector keys;
            for(int idx = 0;
                idx < header->field.number_of_key;
                idx ++) {
                keys.push_back(data_buffer.readStringUntilNull());
            }
            err = handler->consumeGetEvent(header,
                                           keys,
                                           result_header->data<opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult>(),
                                           result_data,
                                           result_data_size);
            if(err == 0){
                //set the result header and data
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIODeviceChannelHeaderMultiGetOpcodeResult));
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_data_size);
                result_header->data<opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult>()->number_of_result = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult>()->number_of_result);
            }
            break;
        }

        case opcode::DeviceChannelOpcodeQueryDataCloud: {
            if(synchronous_answer == NULL) return -1000;
            opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr header = data_pack->channel_header_data->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud>();

            try {
                if (data_pack &&
                    data_pack->channel_data) {
                    BufferSPtr result_data;
                    QueryResultPage result_page;
                    chaos_data::CDataWrapper query(data_pack->channel_data->data());
                    BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult));

                    header->field.record_for_page = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.record_for_page);

                    //decode the endianes off the data
                    std::string key = CDW_GET_SRT_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, "");
                    uint64_t start_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, getUInt64Value, 0);
                    uint64_t end_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, getUInt64Value, 0);
                    SearchSequence last_sequence_info = {CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_RUN_ID, getInt64Value, 0),
                                                        CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_DP_COUNTER, getInt64Value, 0)};
                    //call server api if we have at least the key
                    if((key.compare("") != 0)) {err = handler->consumeDataCloudQuery(header,
                                                                                     key,
                                                                                     start_ts,
                                                                                     end_ts,
                                                                                     last_sequence_info,//in-out
                                                                                     result_page);}
                    if(err == 0){
                        //manage emory for retur data
                        if((result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->numer_of_record_found = (uint32_t)result_page.size())){
                            result_data = ChaosMakeSharedPtr<Buffer>();
                            //we successfully have perform query
                            result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size = 0;
                            for(QueryResultPageIterator it = result_page.begin(),
                                end = result_page.end();
                                it != end;
                                it++) {
                                //write result into mresults memory
                                int element_bson_size = 0;
                                const char * element_bson_mem = (*it)->getBSONRawData(element_bson_size);

                                //enlarge buffer
//                                result_data.capacity((result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size + element_bson_size));

                                //copy bson elelment in memory location
                                result_data->append(element_bson_mem, element_bson_size);
//                                char *mem_start_copy = ((char*)result_data.data())+result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size;

                                //copy
//                                std::memcpy(mem_start_copy, element_bson_mem, element_bson_size);

                                //keep track of the full size of the result
                                result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size +=element_bson_size;
                            }
                        }

                        //set the result header and data
                        DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult));
                        DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size);
                        result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size);
                        result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->numer_of_record_found = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->numer_of_record_found);
                        result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->last_found_sequence.run_id = TO_LITTEL_ENDNS_NUM(uint64_t, last_sequence_info.run_id);
                        result_header->data<opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->last_found_sequence.datapack_counter = TO_LITTEL_ENDNS_NUM(uint64_t, last_sequence_info.datapack_counter);
                    }
                }
            } catch (...) {
                // inca se of error header an cdatawrapper are cleaned here

            }
            break;
        }

        case opcode::DeviceChannelOpcodeDeleteDataCloud: {
            try {
                if (data_pack &&
                    data_pack->channel_data) {
                    chaos_data::CDataWrapper query((char *)data_pack->channel_data->data());

                    //decode the endianes off the data
                    std::string key = CDW_GET_SRT_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, "");
                    uint64_t start_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, getUInt64Value, 0);
                    uint64_t end_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, getUInt64Value, 0);
                    //call server api if we have at least the key
                    if((key.compare("") != 0)) {
                        err = handler->consumeDataCloudDelete(key,
                                                              start_ts,
                                                              end_ts);
                    }
                }
            } catch (...) {}
            break;
        }
        default:
            break;
    }
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
