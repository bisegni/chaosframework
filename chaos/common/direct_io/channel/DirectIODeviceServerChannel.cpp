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

DirectIODeviceServerChannel::DirectIODeviceServerChannel(std::string alias):
DirectIOVirtualServerChannel(alias, DIODataset_Channel_Index),
handler(NULL) {DirectIOVirtualServerChannel::setDelegate(this);}

void DirectIODeviceServerChannel::setHandler(DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler *_handler) {handler = _handler;}

int DirectIODeviceServerChannel::consumeDataPack(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                 chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    CHAOS_ASSERT(handler)
    int err = -1;

    // get the opcode
    synchronous_answer = ChaosMakeSharedPtr<DirectIODataPack>();
    opcode::DeviceChannelOpcode  channel_opcode = static_cast<opcode::DeviceChannelOpcode>(data_pack->header.dispatcher_header.fields.channel_opcode);
    switch (channel_opcode) {
        case opcode::DeviceChannelOpcodePutOutput: {
            DirectIODeviceChannelHeaderPutOpcode *header = data_pack->channel_header_data->data<DirectIODeviceChannelHeaderPutOpcode>();
            //reallign the pointer to the start of the key
            header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
            err = handler->consumePutEvent(*header,
                                           data_pack->channel_data,
                                           data_pack->header.channel_data_size);
            break;
        }

        case opcode::DeviceChannelOpcodePutHeathData: {
            DirectIODeviceChannelHeaderPutOpcode *header = data_pack->channel_header_data->data<DirectIODeviceChannelHeaderPutOpcode>();
            //reallign the pointer to the start of the key
            header->tag = FROM_LITTLE_ENDNS_NUM(uint32_t, header->tag);
            err = handler->consumeHealthDataEvent(*header,
                                                  data_pack->channel_data,
                                                  data_pack->header.channel_data_size);
            break;
        }

        case opcode::DeviceChannelOpcodeGetLastOutput: {
            if(!data_pack->header.dispatcher_header.fields.synchronous_answer) return -1000;
            //allocate variable for result
            BufferSPtr result_data;
            BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderGetOpcodeResult));

            err = handler->consumeGetEvent(data_pack->channel_data,
                                           data_pack->header.channel_data_size,
                                           *result_header->data<DirectIODeviceChannelHeaderGetOpcodeResult>(),
                                           result_data);
            if(err == 0){
                //set the result header and data
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, (uint32_t)result_header->size())
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len)
                result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult>()->value_len);
            }
            break;
        }

        case opcode::DeviceChannelOpcodeMultiGetLastOutput: {
            if(!data_pack->header.dispatcher_header.fields.synchronous_answer) return -1000;
            //allocate variable for result
            uint32_t result_data_size = 0;
            BufferSPtr result_data;
            DirectIODeviceChannelHeaderMultiGetOpcode *header = data_pack->channel_header_data->data<DirectIODeviceChannelHeaderMultiGetOpcode>();
            BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderMultiGetOpcodeResult));
            
            //fetch the set of keys
            DataBuffer<> data_buffer(data_pack->channel_data->data(),
                                     data_pack->header.channel_data_size,
                                     false);

            ChaosStringVector keys;
            for(int idx = 0;
                idx < header->field.number_of_key;
                idx ++) {
                keys.push_back(data_buffer.readStringUntilNull());
            }
            err = handler->consumeGetEvent(*header,
                                           keys,
                                           *result_header->data<DirectIODeviceChannelHeaderMultiGetOpcodeResult>(),
                                           result_data,
                                           result_data_size);
            if(err == 0){
                //set the result header and data
                DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, (uint32_t)result_header->size());
                DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_data_size);
                result_header->data<opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult>()->number_of_result = TO_LITTEL_ENDNS_NUM(uint32_t, result_header->data<opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult>()->number_of_result);
            }
            break;
        }

        case opcode::DeviceChannelOpcodeQueryDataCloud: {
            if(!data_pack->header.dispatcher_header.fields.synchronous_answer) return -1000;
            DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header = data_pack->channel_header_data->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloud>();
            try {
                if (data_pack &&
                    data_pack->channel_data) {
                    BufferSPtr result_data;
                    QueryResultPage result_page;
                    chaos_data::CDataWrapper query(data_pack->channel_data->data());
                    BufferSPtr result_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult));
                    DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult  *result_header_t = result_header->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>();

                    
                    header->field.record_for_page = FROM_LITTLE_ENDNS_NUM(uint32_t, header->field.record_for_page);

                    //decode the endianes off the data
                    std::string key = CDW_GET_SRT_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, "");
                    uint64_t start_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, getUInt64Value, 0);
                    uint64_t end_ts = CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, getUInt64Value, 0);
                    SearchSequence last_sequence_info = {CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_RUN_ID, getInt64Value, 0),
                                                        CDW_GET_VALUE_WITH_DEFAULT(&query, DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_DP_COUNTER, getInt64Value, 0)};
                    //call server api if we have at least the key
                    if((key.compare("") != 0)) {err = handler->consumeDataCloudQuery(*header,
                                                                                     key,
                                                                                     start_ts,
                                                                                     end_ts,
                                                                                     last_sequence_info,//in-out
                                                                                     result_page);}
                    if(err == 0){
                        //manage emory for retur data
                        if((result_header_t->numer_of_record_found = (uint32_t)result_page.size())){
                            result_data = ChaosMakeSharedPtr<Buffer>();
                            //we successfully have perform query
                            result_header->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size = 0;
                            for(QueryResultPageIterator it = result_page.begin(),
                                end = result_page.end();
                                it != end;
                                it++) {
                                //write result into mresults memory
                                int element_bson_size = 0;
                                const char * element_bson_mem = (*it)->getBSONRawData(element_bson_size);
                                result_data->append(element_bson_mem, element_bson_size);
                                result_header->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>()->result_data_size +=element_bson_size;
                            }
                        }

                        //set the result header and data
                        DIRECT_IO_SET_CHANNEL_HEADER(synchronous_answer, result_header, result_header->size());
                        DIRECT_IO_SET_CHANNEL_DATA(synchronous_answer, result_data, result_data->size());
                        result_header_t->result_data_size = TO_LITTEL_ENDNS_NUM(uint32_t, result_header_t->result_data_size);
                        result_header_t->numer_of_record_found = TO_LITTEL_ENDNS_NUM(uint32_t, result_header_t->numer_of_record_found);
                        result_header_t->last_found_sequence.run_id = TO_LITTEL_ENDNS_NUM(uint64_t, last_sequence_info.run_id);
                        result_header_t->last_found_sequence.datapack_counter = TO_LITTEL_ENDNS_NUM(uint64_t, last_sequence_info.datapack_counter);
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
                    chaos_data::CDataWrapper query(data_pack->channel_data->data());

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
