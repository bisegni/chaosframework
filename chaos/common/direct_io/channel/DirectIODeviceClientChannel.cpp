/*
 *	DirectIODeviceDatasetClientChannel.cpp
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

#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/utility/endianess.h>
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

//define the static deallocator class
DirectIODeviceClientChannel::DirectIODeviceClientChannelDeallocator DirectIODeviceClientChannel::STATIC_DirectIODeviceClientChannelDeallocator;


DirectIODeviceClientChannel::DirectIODeviceClientChannel(std::string alias):
DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index),
answer_server_info(){
    //associate the default static allocator
    header_deallocator = &STATIC_DirectIODeviceClientChannelDeallocator;
}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {
    
}

void DirectIODeviceClientChannel::setAnswerServerInfo(uint16_t p_server_port, uint16_t s_server_port, uint16_t answer_endpoint) {
    answer_server_info.p_server_port = p_server_port;
    answer_server_info.s_server_port = s_server_port;
    answer_server_info.endpoint = answer_endpoint;
    //setEndpoint(endpoint);
    answer_server_info.ip = ((DirectIOClientConnection*)client_instance)->getI64Ip();
}

int64_t DirectIODeviceClientChannel::storeAndCacheDataOutputChannel(const std::string& key,
                                                                    void *buffer,
                                                                    uint32_t buffer_len,
                                                                    DirectIODeviceClientChannelPutMode _put_mode) {
    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
    DirectIODeviceChannelHeaderPutOpcode *put_opcode_header = (opcode_headers::DirectIODeviceChannelHeaderPutOpcode *)calloc((PUT_HEADER_LEN(key)+key.size()), 1);
    
    put_opcode_header->tag = (uint8_t) _put_mode;
    put_opcode_header->key_len = key.size();
    //put_opcode_header->key_data
    std::memcpy(GET_PUT_OPCODE_KEY_PTR(put_opcode_header), key.c_str(), put_opcode_header->key_len);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutOutput);
    
    //set the header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)PUT_HEADER_LEN(key))
    //set data if the have some
    if(buffer_len)DIRECT_IO_SET_CHANNEL_DATA(data_pack, buffer, buffer_len)
        return sendPriorityData(data_pack);
}

//! Send device serialization with priority
int64_t DirectIODeviceClientChannel::requestLastOutputData(const std::string& key,
                                                           void **result,
                                                           uint32_t &size) {
    int64_t err = 0;
    DirectIODataPack *answer = NULL;
    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
    
    //the precomputed header for get last shared output channel
    DirectIODeviceChannelHeaderGetOpcode *get_opcode_header  = (DirectIODeviceChannelHeaderGetOpcode*)calloc(sizeof(DirectIODeviceChannelHeaderGetOpcode), 1);
    
    //allocate memory for key
    void *data = (void*)malloc(key.size());
    std::memcpy(data, key.c_str(), key.size());
    
    get_opcode_header->field.address = TO_LITTE_ENDNS_NUM(uint64_t, answer_server_info.ip);
    get_opcode_header->field.p_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.p_server_port);
    get_opcode_header->field.s_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.s_server_port);
    get_opcode_header->field.endpoint = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.endpoint);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetLastOutput);
    
    //set header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, get_opcode_header, sizeof(DirectIODeviceChannelHeaderGetOpcode))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, data, (uint32_t)key.size())
    //send data with synchronous answer flag
    if((err = sendPriorityData(data_pack, &answer))) {
        //error getting last value
        DIODCCLERR_ << "Error getting last value for key:" << key << " with error:" <<err;
    } else {
        //we got answer
        if(answer) {
            //get the header
            opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header = static_cast<opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult*>(answer->channel_header_data);
            result_header->value_len = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->value_len);
            size = result_header->value_len;
            *result = answer->channel_data;
        } else {
            *result = NULL;
            size = 0;
        }
    }
    if(answer) {
        if(answer->channel_header_data) free(answer->channel_header_data);
        free(answer);
    }
    return err;
}

int64_t DirectIODeviceClientChannel::queryDataCloud(const std::string& key, uint64_t start_ts, uint64_t end_ts, string& query_id) {
    CDataWrapper query_description;
    //allcoate the data to send direct io pack
    DirectIODataPack *data_pack = (DirectIODataPack*)calloc(sizeof(DirectIODataPack), 1);
    DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr query_data_cloud_header =
    (DirectIODeviceChannelHeaderOpcodeQueryDataCloud*)calloc(sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud), 1);
    
    //fill the query CDataWrapper
    query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, key);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
    
    //fill the hader
    query_data_cloud_header->field.address = TO_LITTE_ENDNS_NUM(uint64_t, answer_server_info.ip);
    query_data_cloud_header->field.p_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.p_server_port);
    query_data_cloud_header->field.s_port = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.s_server_port);
    query_data_cloud_header->field.endpoint = TO_LITTE_ENDNS_NUM(uint16_t, answer_server_info.endpoint);
    
    query_id = UUIDUtil::generateUUIDLite();
    query_id.resize(8);
    //copy the query id on header
    std::strncpy(query_data_cloud_header->field.query_id, query_id.c_str(), 8);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloud);
    
    //get the buffer to send
    auto_ptr<SerializationBuffer> buffer(query_description.getBSONData());
    
    //the frre of memeory is managed in this class in async way
    buffer->disposeOnDelete = false;
    
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, query_data_cloud_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, (void*)buffer->getBufferPtr(), (uint32_t)buffer->getBufferLen());
    
    //send query request
    return sendServiceData(data_pack);
}


// start the answering sequence to a query
int64_t DirectIODeviceClientChannel::startQueryDataCloudResult(const std::string& query_id,
                                                               uint64_t total_element_found) {
    DECLARE_AND_CALLOC_DATA_PACK(data_pack)
    DECLARE_AND_CALLOC_HEADER(DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult, cq_start_result_header)
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloudStartResult);
    
    //copy the query id on header
    std::strncpy(cq_start_result_header->field.query_id, query_id.c_str(), 8);
    
    //configure the header
    cq_start_result_header->field.total_element_found = TO_LITTE_ENDNS_NUM(uint64_t, total_element_found);
    
    //set the header for forward
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, cq_start_result_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult))
    
    //send message
    return sendServiceData(data_pack);
}


//! Send the single result of the temporal query to requester
int64_t DirectIODeviceClientChannel::sendResultToQueryDataCloud(const std::string& query_id,
                                                                uint64_t element_index,
                                                                void *data,
                                                                uint32_t data_len,
                                                                DirectIODeallocationHandler *data_deallocator) {
    DECLARE_AND_CALLOC_DATA_PACK(data_pack)
    DECLARE_AND_CALLOC_HEADER(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult, cq_result_header)
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloudResult);
    
    //copy the query id on header
    std::strncpy(cq_result_header->field.query_id, query_id.c_str(), 8);
    
    cq_result_header->field.element_index = TO_LITTE_ENDNS_NUM(uint64_t, element_index);
    
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, cq_result_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, data, data_len);
    
    //send query request
    return sendServiceData(data_pack, (data_deallocator?data_deallocator:this));
}


//! Set the end of the answer to a query
int64_t DirectIODeviceClientChannel::endQueryDataCloudResult(const std::string& query_id,
                                                             int32_t error,
                                                             const std::string& error_message) {
    DECLARE_AND_CALLOC_DATA_PACK(data_pack)
    DECLARE_AND_CALLOC_HEADER(DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult, cq_end_result_header)
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeQueryDataCloudEndResult);
    
    //copy the query id on header
    std::strncpy(cq_end_result_header->field.query_id, query_id.c_str(), 8);
    
    //configure the header
    cq_end_result_header->field.error = TO_LITTE_ENDNS_NUM(int32_t, error);
    cq_end_result_header->field.error_message_length = TO_LITTE_ENDNS_NUM(uint32_t, (uint32_t)error_message.size());
    
    //set the header for forward
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, cq_end_result_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult))
    if(cq_end_result_header->field.error_message_length) {
        //send error message has data
        
    }
    //send message
    return sendServiceData(data_pack);
}

//! default data deallocator implementation
void DirectIODeviceClientChannel::DirectIODeviceClientChannelDeallocator::freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) {
    switch(free_info_ptr->sent_part) {
        case DisposeSentMemoryInfo::SentPartHeader:{
            switch(static_cast<opcode::DeviceChannelOpcode>(free_info_ptr->sent_opcode)) {
                case opcode::DeviceChannelOpcodeQueryDataCloud:
                case opcode::DeviceChannelOpcodeQueryDataCloudStartResult:
                case opcode::DeviceChannelOpcodeQueryDataCloudResult:
                case opcode::DeviceChannelOpcodeQueryDataCloudEndResult:
                case opcode::DeviceChannelOpcodePutOutput:
                case opcode::DeviceChannelOpcodeGetLastOutput:
                    free(sent_data_ptr);
                    break;
                    //these two opcode are header allocated in the stack
                    break;
            }
            break;
        }
            
        case DisposeSentMemoryInfo::SentPartData: {
            switch(static_cast<opcode::DeviceChannelOpcode>(free_info_ptr->sent_opcode)) {
                    //opcode with data
                case opcode::DeviceChannelOpcodePutOutput:
                case opcode::DeviceChannelOpcodeGetLastOutput:
                case opcode::DeviceChannelOpcodeQueryDataCloud:
                case opcode::DeviceChannelOpcodeQueryDataCloudResult:
                    free(sent_data_ptr);
                    break;
                    
                    //opcode with optionally data
                case opcode::DeviceChannelOpcodeQueryDataCloudEndResult:
                    if(sent_data_ptr) free(sent_data_ptr);
                    break;
                    
                    //opcode with no data
                case opcode::DeviceChannelOpcodeQueryDataCloudStartResult:
                    break;
            }
            break;
        }
    }
}