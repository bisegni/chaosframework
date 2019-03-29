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
DirectIOVirtualClientChannel(alias, DIODataset_Channel_Index){}

DirectIODeviceClientChannel::~DirectIODeviceClientChannel() {}

int DirectIODeviceClientChannel::storeAndCacheDataOutputChannel(const std::string& key,
                                                                void *buffer,
                                                                uint32_t buffer_len,
                                                                chaos::DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                                const ChaosStringSet& tag_set,
                                                                bool wait_result) {
    int err = 0;
    if(key.size() > 250) return -1;
    
    DataBuffer data_buffer;
    DirectIODataPackSPtr answer;
    
    //write mode and tags number
    data_buffer.writeInt8((int8_t) _put_mode);
    data_buffer.writeInt16(tag_set.size());
    //write key
    data_buffer.writeByte(key.c_str(), (int32_t)key.size());
    data_buffer.writeByte('\0');
    //write tags
    for(ChaosStringSetConstIterator it = tag_set.begin(),
        end = tag_set.end();
        it!=end;
        it++) {
        data_buffer.writeByte(it->c_str(), (int32_t)it->size());
        data_buffer.writeByte('\0');
    }
    
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    BufferSPtr put_opcode_header = data_buffer.detachBuffer();
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer, buffer_len, buffer_len, true);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutOutput);
    
    //set the header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)put_opcode_header->size())
    //set data if the have some
    if(buffer_len){DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());}
    if(wait_result) {
        if((err = sendPriorityData(MOVE(data_pack), answer))) {
            //error getting last value
            DIODCCLERR_ << "Error storing value for key:" << key << " with error:" <<err;
        }
    } else {
        err = sendPriorityData(MOVE(data_pack));
    }
    return err;
}

int DirectIODeviceClientChannel::storeAndCacheHealthData(const std::string& key,
                                                         void *buffer,
                                                         uint32_t buffer_len,
                                                         chaos::DataServiceNodeDefinitionType::DSStorageType _put_mode,
                                                         const ChaosStringSet& tag_set,
                                                         bool wait_result) {
    int err = 0;
    if(key.size() > 250) return -1;
    DataBuffer data_buffer;
    DirectIODataPackSPtr answer;
    
    //write mode and tags number
    data_buffer.writeInt8((int8_t) _put_mode);
    data_buffer.writeInt16(tag_set.size());
    //write key
    data_buffer.writeByte(key.c_str(), (int32_t)key.size());
    data_buffer.writeByte('\0');
    //write tags
    for(ChaosStringSetConstIterator it = tag_set.begin(),
        end = tag_set.end();
        it!=end;
        it++) {
        data_buffer.writeByte(it->c_str(), (int32_t)it->size());
        data_buffer.writeByte('\0');
    }
    
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    BufferSPtr put_opcode_header = data_buffer.detachBuffer();
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(buffer, buffer_len, buffer_len, true);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodePutHeathData);
    
    //set the header
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, put_opcode_header, (uint32_t)put_opcode_header->size())
    //set data if the have some
    if(buffer_len){DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());}
    if(wait_result) {
        if((err = sendPriorityData(MOVE(data_pack), answer))) {
            //error getting last value
            DIODCCLERR_ << "Error storing value for key:" << key << " with error:" <<err;
        }
    } else {
        err = sendPriorityData(MOVE(data_pack));
    }
    return err;
}

//! Send device serialization with priority
int DirectIODeviceClientChannel::requestLastOutputData(const std::string& key,
                                                       char **result,
                                                       uint32_t &size) {
    int err = 0;
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    
    //allocate memory for key
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>();
    channel_data->append(key.c_str(), key.size());
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetLastOutput);
    
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    //send data with synchronous answer flag
    if((err = sendPriorityData(MOVE(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << "Error getting last value for key:" << key << " with error:" <<err;
        *result = NULL;
        return err;
    } else {
        //we got answer
        if(answer) {
            if(answer->channel_data) {
                size = (uint32_t)answer->channel_data->size();
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
    if(keys.size() == 0) return -1;
    int err = 0;
    DataBuffer data_buffer;
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeMultiGetLastOutput);
    
    BufferSPtr mget_opcode_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderMultiGetOpcode));
    mget_opcode_header->data<DirectIODeviceChannelHeaderMultiGetOpcode>()->field.number_of_key = TO_LITTEL_ENDNS_NUM(uint16_t, keys.size());
    results.clear();
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
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, mget_opcode_header, sizeof(DirectIODeviceChannelHeaderMultiGetOpcode))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    //send data with synchronous answer flag
    if((err = sendPriorityData(MOVE(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << "Error getting last value for multikey set with error:" << err;
        return err;
    } else {
        //we got answer
        if(answer) {
            //get the header
            DirectIODeviceChannelHeaderMultiGetOpcodeResult *result_header = answer->channel_header_data->data<DirectIODeviceChannelHeaderMultiGetOpcodeResult>();
            result_header->number_of_result = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->number_of_result);
            CHAOS_ASSERT(result_header->number_of_result > 0);
            CHAOS_ASSERT(answer->channel_data.get());
            
            DataBuffer data_buffer_answer(answer->channel_data->data(),
                                          answer->header.channel_data_size,
                                          false);
            for(int idx = 0;
                idx < result_header->number_of_result;
                idx++) {
                results.push_back(data_buffer_answer.readCDataWrapperAsShrdPtr());
            }
        }
    }
    return err;
}

int DirectIODeviceClientChannel::queryDataCloud(const std::string& key,
                                                const ChaosStringSet& meta_tags,
                                                const uint64_t start_ts,
                                                const uint64_t end_ts,
                                                const uint32_t page_dimension,
                                                SearchSequence& last_sequence_id,
                                                QueryResultPage& found_element_page,
                                                bool only_index) {
    int err = 0;
    DirectIODataPackSPtr answer;
    CDataWrapper query_description;
    //allcoate the data to send direct io pack
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    BufferSPtr query_data_cloud_header = ChaosMakeSharedPtr<Buffer>(sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud));
    
    //fill the query CDataWrapper
    query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, key);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_RUN_ID, last_sequence_id.run_id);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_LAST_DP_COUNTER, last_sequence_id.datapack_counter);
    for(ChaosStringSetConstIterator it = meta_tags.begin(),
        end = meta_tags.end();
        it != end;
        it++) {
        query_description.appendStringToArray(*it);
    }
    query_description.finalizeArrayForKey(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_META_TAGS);
    //copy the query id on header
    query_data_cloud_header->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloud>()->field.record_for_page = TO_LITTEL_ENDNS_NUM(uint32_t, page_dimension);
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(only_index?
                                                                                     opcode::DeviceChannelOpcodeQueryDataCloudIndex:
                                                                                     opcode::DeviceChannelOpcodeQueryDataCloud);
    
    BufferSPtr channel_data = ChaosMakeSharedPtr<Buffer>(query_description.getBSONRawData(),
                                                         query_description.getBSONRawSize());
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_HEADER(data_pack, query_data_cloud_header, sizeof(DirectIODeviceChannelHeaderOpcodeQueryDataCloud))
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(MOVE(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing query for key %1%",%key);
    } else {
        //we got answer
        if(answer) {
            //get the header
            DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult *result_header = answer->channel_header_data->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>();
            uint32_t result_data_size = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->result_data_size);
            uint32_t numer_of_record_found = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->numer_of_record_found);
            last_sequence_id.run_id = FROM_LITTLE_ENDNS_NUM(uint64_t, result_header->last_found_sequence.run_id);
            last_sequence_id.datapack_counter = FROM_LITTLE_ENDNS_NUM(uint64_t, result_header->last_found_sequence.datapack_counter);
            if(numer_of_record_found &&
               result_data_size) {
                //scan all result
                char *current_data_prt = (char*)answer->channel_data->data();
                while(found_element_page.size() < numer_of_record_found) {
                    ChaosSharedPtr<CDataWrapper> last_record = ChaosMakeSharedPtr<CDataWrapper>(current_data_prt);
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

int DirectIODeviceClientChannel::getDataByIndex(const VectorCDWShrdPtr& indexs,
                                                VectorCDWShrdPtr& results) {
    int err = 0;
    CDataWrapper index_data;
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();

    //fill the query CDataWrapper
    for_each(indexs.begin(), indexs.end(), [&index_data](const CDWShrdPtr& index){
        index_data.appendCDataWrapperToArray(*index);
    });
    index_data.finalizeArrayForKey("indexes");

    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetDataByIndex);
   
    BufferSPtr channel_data = BufferSPtr(index_data.getBSONDataBuffer().release());
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(MOVE(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing query for get data by index with error %1%",%err);
    } else {
        //we got answer
        if(answer) {
            //get the header
            DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult *result_header = answer->channel_header_data->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>();
            uint32_t result_data_size = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->result_data_size);
            uint32_t numer_of_record_found = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->numer_of_record_found);
            if(numer_of_record_found &&
               result_data_size) {
                //scan all result
                char *current_data_prt = (char*)answer->channel_data->data();
                for(int idx = 0; idx < numer_of_record_found; idx++) {
                    CDWShrdPtr last_record = ChaosMakeSharedPtr<CDataWrapper>(current_data_prt);
                    //!at this time cdata wrapper copy the data
                    results.push_back(last_record);
                }
            }
        }
    }
    return err;
}

int DirectIODeviceClientChannel::getDataByIndex(const CDWShrdPtr& index,
                                                CDWShrdPtr& result) {
    int err = 0;
    CDataWrapper index_data;
    DirectIODataPackSPtr answer;
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    
    //fill the query CDataWrapper
    index_data.appendCDataWrapperToArray(*index);
    index_data.finalizeArrayForKey("indexes");
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeGetDataByIndex);
    
    BufferSPtr channel_data = BufferSPtr(index_data.getBSONDataBuffer().release());
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(MOVE(data_pack), answer))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing query for get data by index with error %1%",%err);
    } else {
        //we got answer
        if(answer) {
            //get the header
            DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult *result_header = answer->channel_header_data->data<DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult>();
            uint32_t result_data_size = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->result_data_size);
            uint32_t numer_of_record_found = FROM_LITTLE_ENDNS_NUM(uint32_t, result_header->numer_of_record_found);
            if(numer_of_record_found &&
               result_data_size) {
                result = ChaosMakeSharedPtr<CDataWrapper>(answer->channel_data->data());
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
    DirectIODataPackSPtr answer;
    BufferSPtr channel_data;
    //allcoate the data to send direct io pack
    DirectIODataPackSPtr data_pack = ChaosMakeSharedPtr<DirectIODataPack>();
    
    //fill the query CDataWrapper
    query_description.addStringValue(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_SEARCH_KEY_STRING, key);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_STAR_TS_I64, (int64_t)start_ts);
    query_description.addInt64Value(DeviceChannelOpcodeQueryDataCloudParam::QUERY_PARAM_END_TS_I64, (int64_t)end_ts);
    
    //set opcode
    data_pack->header.dispatcher_header.fields.channel_opcode = static_cast<uint8_t>(opcode::DeviceChannelOpcodeDeleteDataCloud);
    
    channel_data = ChaosMakeSharedPtr<Buffer>(query_description.getBSONRawData(),
                                              query_description.getBSONRawSize());
    //set header and data for the query
    DIRECT_IO_SET_CHANNEL_DATA(data_pack, channel_data, (uint32_t)channel_data->size());
    if((err = sendServiceData(MOVE(data_pack)))) {
        //error getting last value
        DIODCCLERR_ << CHAOS_FORMAT("Error executing delete operation for key %1%",%key);
    }
    return err;
}
