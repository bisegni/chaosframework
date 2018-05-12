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

#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/DataBuffer.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <boost/format.hpp>

#include <string.h>

using namespace chaos::common::data;
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

//#define PUT_HEADER_LEN  sizeof(DirectIODeviceChannelHeaderPutOpcode)-sizeof(void*)+device_id.size()
#define PUT_HEADER_LEN(x)  (GET_PUT_OPCODE_FIXED_PART_LEN+((uint32_t)x.size()))

#define DECLARE_AND_CALLOC_DATA_PACK(n)\
DirectIODataPack *n = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);

#define DECLARE_AND_CALLOC_HEADER(x,n)\
x *n = (x*)calloc(sizeof(x),1);

#define DIODCCLAPP_ INFO_LOG(DirectIODeviceClientChannel)
#define DIODCCLDBG_ DBG_LOG(DirectIODeviceClientChannel)
#define DIODCCLERR_ ERR_LOG(DirectIODeviceClientChannel)

DEFINE_CLASS_FACTORY(DirectIODeviceClientChannel, DirectIOVirtualClientChannel);


DirectIODeviceClientChannel::DirectIODeviceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index),
answer_server_info(){}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {}

void DirectIODeviceClientChannel::setAnswerServerInfo(uint16_t p_server_port, uint16_t s_server_port, uint16_t answer_endpoint) {
    answer_server_info.p_server_port = p_server_port;
    answer_server_info.s_server_port = s_server_port;
    answer_server_info.endpoint = answer_endpoint;
    //setEndpoint(endpoint);
    answer_server_info.ip = ((DirectIOClientConnection*)client_instance())->getI64Ip();
}

int DirectIODeviceClientChannel::storeAndCacheDataOutputChannel(const std::string& key,
                                                                void *buffer,
                                                                uint32_t buffer_len,
                                                                DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                                bool wait_result) {
    using HeaderPO = opcode_headers::DirectIODeviceChannelHeaderPutOpcode;
    int err = 0;
    DirectIODataPackSPtr answer;
    DirectIODataPackUPtr data_pack(new DirectIODataPack());
    BufferSPtr put_opcode_header = ChaosMakeSharedPtr<Buffer>(sizeof(HeaderPO));
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer, buffer_len, buffer_len, true);
    
    put_opcode_header->data<HeaderPO>()->tag = (uint8_t) _put_mode;
    put_opcode_header->data<HeaderPO>()->key_len = key.size();
    //put_opcode_header->key_data
    std::memcpy(GET_PUT_OPCODE_KEY_PTR(put_opcode_header->data<HeaderPO>()), key.c_str(), put_opcode_header->data<HeaderPO>()->key_len);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutOutput);
    
    //set the header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)PUT_HEADER_LEN(key))
    //set data if the have some
    if(buffer_len){DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());}
    if(wait_result) {
        if((err = sendPriorityData(ChaosMoveOperator(data_pack), answer))) {
            //error getting last value
            DIODCCLERR_ << "Error storing value for key:" << key << " with error:" <<err;
        }
    } else {
        err = sendPriorityData(ChaosMoveOperator(data_pack));
    }
    return err;
}

int DirectIODeviceClientChannel::storeAndCacheHealthData(const std::string& key,
                                                         void *buffer,
                                                         uint32_t buffer_len,
                                                         DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                         bool wait_result) {
    using ApiHeader = opcode_headers::DirectIODeviceChannelHeaderPutOpcode;
    int err = 0;
    DirectIODataPackSPtr answer;
    DirectIODataPackUPtr data_pack(new DirectIODataPack());
    BufferSPtr put_opcode_header = ChaosMakeSharedPtr<Buffer>((PUT_HEADER_LEN(key)+key.size()));
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer, buffer_len, buffer_len, true);
    
    put_opcode_header->data<ApiHeader>()->tag = (uint8_t) _put_mode;
    put_opcode_header->data<ApiHeader>()->key_len = key.size();
    //put_opcode_header->key_data
    std::memcpy(GET_PUT_OPCODE_KEY_PTR(put_opcode_header->data()), key.c_str(), put_opcode_header->data<ApiHeader>()->key_len);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutHeathData);
    
    //set the header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)PUT_HEADER_LEN(key))
    //set data if the have some
    if(buffer_len){DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());}
    if(wait_result) {
        if((err = sendPriorityData(ChaosMoveOperator(data_pack), answer))) {
            //error getting last value
            DIODCCLERR_ << "Error storing value for key:" << key << " with error:" <<err;
        }
    } else {
        err = sendPriorityData(ChaosMoveOperator(data_pack));
    }
    return err;
}

//! Send device serialization with priority
int DirectIODeviceClientChannel::requestLastOutputData(const std::string& key,
                                                       void **result,
                                                       uint32_t &size) {
    using ApiHeader = DirectIODeviceChannelHeaderGetOpcode;
    int err = 0;
    DirectIODataPackSPtr answer;
    DirectIODataPackUPtr data_pack(new DirectIODataPack());
    
    //the precomputed header for get last shared output channel
    BufferSPtr get_opcode_header  = ChaosMakeSharedPtr<Buffer>(sizeof(ApiHeader));
    
    //allocate memory for key
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>();
    channel_data->append(key.c_str(), key.size());
    
    get_opcode_header->data<ApiHeader>()->field.address = TO_LITTEL_ENDNS_NUM(uint64_t, answer_server_info.ip);
    get_opcode_header->data<ApiHeader>()->field.p_port = TO_LITTEL_ENDNS_NUM(uint16_t, answer_server_info.p_server_port);
    get_opcode_header->data<ApiHeader>()->field.s_port = TO_LITTEL_ENDNS_NUM(uint16_t, answer_server_info.s_server_port);
    get_opcode_header->data<ApiHeader>()->field.endpoint = TO_LITTEL_ENDNS_NUM(uint16_t, answer_server_info.endpoint);
    
    
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetLastOutput);
    
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, get_opcode_header, sizeof(DirectIODeviceChannelHeaderGetOpcode));
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    //send data with synchronous answer flag
    if((err = sendPriorityData(ChaosMoveOperator(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << "Error getting last value for key:" << key << " with error:" <<err;
        *result = NULL;
        return err;
    } else {
        //we got answer
        if(answer) {
            using AnswerHeader = opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult;
            answer->channel_header_data->data<AnswerHeader>()->value_len = FROM_LITTLE_ENDNS_NUM(uint32_t, answer->channel_header_data->data<AnswerHeader>()->value_len);
            if(answer->channel_header_data->data<AnswerHeader>()->value_len > 0) {
                size = answer->channel_header_data->data<AnswerHeader>()->value_len;
                *result = answer->channel_data->detach();
            }
        } else {
            *result = NULL;
            size = 0;
        }
    }
    return err;
}

int DirectIODeviceClientChannel::requestLastOutputData(const ChaosStringVector& keys,
                                                       VectorCDWShrdPtr& results) {
    using ApiHeader = DirectIODeviceChannelHeaderMultiGetOpcode;
    if(keys.size() == 0) return -1;
    int err = 0;
    DataBuffer<> data_buffer;
    DirectIODataPackSPtr answer;
    DirectIODataPackUPtr data_pack( new DirectIODataPack());
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeMultiGetLastOutput);
    
    BufferSPtr mget_opcode_header = ChaosMakeSharedPtr<Buffer>(sizeof(ApiHeader));
    mget_opcode_header->data<ApiHeader>()->field.number_of_key = TO_LITTEL_ENDNS_NUM(uint16_t, keys.size());
    
    for(ChaosStringVectorConstIterator it = keys.begin(),
        end = keys.end();
        it != end;
        it++) {
        data_buffer.writeByte(it->c_str(), (int32_t)it->size());
        data_buffer.writeByte('\0');
    }
    
    //set header
    uint32_t data_size = data_buffer.getCursorLocation();
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(data_buffer.release(), data_size, data_size, true);
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, mget_opcode_header, sizeof(ApiHeader))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    //send data with synchronous answer flag
    if((err = sendPriorityData(ChaosMoveOperator(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << "Error getting last value for multikey set with error:" << err;
        return err;
    } else {
        //we got answer
        if(answer) {
            //get the header
            using ResultHeader = opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult;
            
            ResultHeader *result_header = answer->channel_header_data->data<ResultHeader>();
            result_header->number_of_result = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->number_of_result);
            CHAOS_ASSERT(result_header->number_of_result > 0);
            CHAOS_ASSERT(answer->channel_data);
            
            DataBuffer<> data_buffer(answer->channel_data->data(), answer->header.channel_data_size);
            answer->channel_data = NULL;
            for(int idx = 0;
                idx < result_header->number_of_result;
                idx++) {
                results.push_back(data_buffer.readCDataWrapperAsShrdPtr());
            }
        }
    }
    return err;
}

int DirectIODeviceClientChannel::queryDataCloud(const std::string& key,
                                                uint64_t start_ts,
                                                uint64_t end_ts,
                                                uint32_t page_dimension,
                                                SearchSequence& last_sequence_id,
                                                QueryResultPage& found_element_page) {
    using ApiHeader = DirectIODeviceChannelHeaderOpcodeQueryDataCloud;
    int err = 0;
    DirectIODataPackSPtr answer;
    CDataWrapper query_description;
    //allcoate the data to send direct io pack
    DirectIODataPackUPtr data_pack(new DirectIODataPack());
    BufferSPtr query_data_cloud_header = ChaosMakeSharedPtr<Buffer>(sizeof(ApiHeader));
    
    //fill the query CDataWrapper
    query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, key);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_RUN_ID, last_sequence_id.run_id);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_DP_COUNTER, last_sequence_id.datapack_counter);
    
    //copy the query id on header
    query_data_cloud_header->data<ApiHeader>()->field.record_for_page = TO_LITTEL_ENDNS_NUM(uint32_t, page_dimension);
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloud);
    
    //get the buffer to send
    ChaosUniquePtr<SerializationBuffer> buffer(query_description.getBSONData());
    
    //the frre of memeory is managed in this class in async way
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(sizeof(buffer->getBufferPtr(), buffer->getBufferLen(), buffer->getBufferLen()));
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, query_data_cloud_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(ChaosMoveOperator(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing query for key %1%",%key);
    } else {
        //we got answer
        if(answer) {
            using ResultHeader = opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult;
            //get the header
            ResultHeader *result_header = answer->channel_header_data->data<ResultHeader>();
            uint32_t result_data_size = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->result_data_size);
            uint32_t numer_of_record_found = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->numer_of_record_found);
            last_sequence_id.run_id = FROM_LITTLE_ENDNS_NUM(uint64_t, result_header->last_found_sequence.run_id);
            last_sequence_id.datapack_counter = FROM_LITTLE_ENDNS_NUM(uint64_t, result_header->last_found_sequence.datapack_counter);
            if(result_data_size > 0) {
                //scan all result
                char *current_data_prt = (char*)answer->channel_data->data();
                while(found_element_page.size() < numer_of_record_found) {
                    ChaosSharedPtr<CDataWrapper> last_record(new CDataWrapper(current_data_prt));
                    //!at this time cdata wrapper copy the data
                    found_element_page.push_back(last_record);
                    //clear current record
                    current_data_prt += last_record->getBSONRawSize();
                }
            }
        }
    }
    return err;
}

int DirectIODeviceClientChannel::deleteDataCloud(const std::string& key,
                                                 uint64_t start_ts,
                                                 uint64_t end_ts) {
    int err = 0;
    CDataWrapper query_description;
    //allcoate the data to send direct io pack
    DirectIODataPackUPtr data_pack(new DirectIODataPack());
    
    //fill the query CDataWrapper
    query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, key);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeDeleteDataCloud);
    
    //get the buffer to send
    ChaosUniquePtr<SerializationBuffer> buffer(query_description.getBSONData());
    
    //the frre of memeory is managed in this class in async way
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer->getBufferPtr(), buffer->getBufferLen(), buffer->getBufferLen());
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(ChaosMoveOperator(data_pack)))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing deelte operation for key %1%",%key);
    }
    return err;
}
