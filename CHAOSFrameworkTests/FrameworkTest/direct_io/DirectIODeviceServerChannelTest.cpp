/*
 *	DirectIOChannelTest.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 19/05/2018 INFN, National Institute of Nuclear Physics
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

#include "DirectIOChannelTest.h"
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>

#include <limits>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

unsigned int req_counter = 0;
unsigned int get_counter = 0;
class DeviceServerHandler:
public DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler {
public:
    ~DeviceServerHandler(){}
    int consumePutEvent(opcode_headers::DirectIODeviceChannelHeaderPutOpcode& header,
                        BufferSPtr channel_data,
                        uint32_t channel_data_len) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            //right result the data need to be savet
            CDWUniquePtr push_data(new CDataWrapper(channel_data->data()));
            if(!push_data->hasKey("key") ||
               !push_data->isInt32Value("key")) {
                err = -1;
            } else if(header.tag != push_data->getInt32Value("key")) {
                err = -1;
            }
        } else {
            //error
            err = -1;
        }
        return err;
    };
    
    int consumeHealthDataEvent(opcode_headers::DirectIODeviceChannelHeaderPutOpcode& header,
                               BufferSPtr channel_data,
                               uint32_t channel_data_len) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            //right result the data need to be savet
            CDWUniquePtr push_data(new CDataWrapper(channel_data->data()));
            if(!push_data->hasKey("key") ||
               !push_data->isInt32Value("key")) {
                err = -1;
            } else if(header.tag != push_data->getInt32Value("key")) {
                err = -1;
            }
        } else {
            //error
            err = -1;
        }
        return err;
    };
    
    int consumeGetEvent(BufferSPtr key_data,
                        uint32_t key_len,
                        opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult& result_header,
                        BufferSPtr& result_value) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            if((++get_counter % 2) == 0) {
                //null result
                result_header.value_len = 0;
            }else{
                //right result
                CDWUniquePtr data(new CDataWrapper());
                data->addStringValue("key", "string");
                result_value = ChaosMakeSharedPtr<Buffer>();
                result_value->append(data->getBSONRawData(), data->getBSONRawSize());
                result_header.value_len = data->getBSONRawSize();
            }
        } else {
            //error
            err = -1;
        }
        return err;
    };
    
    int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode& header,
                        const ChaosStringVector& keys,
                        opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult& result_header,
                        BufferSPtr& result_value,
                        uint32_t& result_value_len) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            //right result
            DataBuffer<> data_buffer;
            for(ChaosStringVectorConstIterator it = keys.begin(),
                end = keys.end();
                it != end;
                it++) {
                CDWUniquePtr data(new CDataWrapper());
                data->addStringValue("key", *it);
                
                data_buffer.writeByte(data->getBSONRawData(),
                                      data->getBSONRawSize());
            }
            result_header.number_of_result = (uint32_t)keys.size();
            result_value_len = data_buffer.getCursorLocation();
            result_value = ChaosMakeSharedPtr<Buffer>(data_buffer.release(), result_value_len, result_value_len, true);
        } else {
            //error
            err = -1;
        }
        return err;
    };
    
    int consumeDataCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                              const std::string& search_key,
                              uint64_t search_start_ts,
                              uint64_t search_end_ts,
                              opcode_headers::SearchSequence& last_element_found_seq,
                              opcode_headers::QueryResultPage& result_page) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            if(search_start_ts != std::numeric_limits<uint32_t>::min() ||
               search_end_ts != std::numeric_limits<uint32_t>::max()) {
                err = -1;
            } else {
                //right result
                DataBuffer<> data_buffer;
                for(int idx = 0;
                    idx < query_header.field.record_for_page;
                    idx++) {
                    CDWUniquePtr data(new CDataWrapper());
                    data->addStringValue("key", CHAOS_FORMAT("%1%",%idx));
                    result_page.push_back(CDWShrdPtr(data.release()));
                }
                last_element_found_seq.datapack_counter++;
                last_element_found_seq.run_id++;
            }
        } else {
            //error
            err = -1;
        }
        return err;
    };
    
    int consumeDataCloudDelete(const std::string& search_key,
                               uint64_t start_ts,
                               uint64_t end_ts) {
        int err = 0;
        if((++req_counter % 2) == 0) {
            if(start_ts != std::numeric_limits<uint32_t>::min() ||
               end_ts != std::numeric_limits<uint32_t>::max()) {
                err = -1;
            } else if(search_key.compare("search") != 0) {
                err = 1;
            }
        } else {
            //error
            err = -1;
        }
        return err;
    }
};

TEST_F(DirectIOChannelTest, DeviceChannelTest) {
    DeviceServerHandler handler;
    DirectIODeviceClientChannel *client_channel = NULL;
    NULL;
    
    DirectIODeviceServerChannel *server_channel = (DirectIODeviceServerChannel*)endpoint->getNewChannelInstance("DirectIODeviceServerChannel");
    ASSERT_TRUE(server_channel);
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    //    client_sys_channel  = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    
    client_channel = (DirectIODeviceClientChannel*)connection->getNewChannelInstance("DirectIODeviceClientChannel");
    ASSERT_TRUE(client_channel);
    //consumePutEvent
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLive);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLive));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLive);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLive));
    }
    
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory));
    }
    
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheDataOutputChannel("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory));
    }
    //consumeHealthDataEvent
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLive);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLive));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLive);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLive));
    }
    
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeHistory));
    }
    
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_TRUE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory));
    }
    {
        CDWUniquePtr push_data(new CDataWrapper());
        push_data->addInt32Value("key", chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
        ChaosUniquePtr<SerializationBuffer> ser(push_data->getBSONData());
        ser->disposeOnDelete = false;
        ASSERT_FALSE(client_channel->storeAndCacheHealthData("key", (void*)ser->getBufferPtr(), (uint32_t)ser->getBufferLen(), chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory));
    }
    
    //consumeGetEvent
    {
        void *result = NULL;
        uint32_t size = 0;
        ASSERT_TRUE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_FALSE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_TRUE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_FALSE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_TRUE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_FALSE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
        ASSERT_TRUE(result && size);
        CDWUniquePtr result_cdw(new CDataWrapper((const char *)result));
        ASSERT_TRUE(result_cdw->hasKey("key"));
        ASSERT_TRUE(result_cdw->isStringValue("key"));
        ASSERT_STREQ(result_cdw->getStringValue("key").c_str(), "string");
        free(result);
    }
    //consumeGetEvent multiple key
    {
        ChaosStringVector keys;
        keys.push_back("key1");
        keys.push_back("key2");
        keys.push_back("key3");
        chaos::common::data::VectorCDWShrdPtr results;
        ASSERT_TRUE(client_channel->requestLastOutputData(keys, results));
        ASSERT_FALSE(client_channel->requestLastOutputData(keys, results));
        ASSERT_TRUE(keys.size() && results.size());
        
        for(int idx = 0;
            idx < keys.size();
            idx++) {
            ASSERT_TRUE(results[idx]->hasKey("key"));
            ASSERT_TRUE(results[idx]->isStringValue("key"));
            ASSERT_STREQ(results[idx]->getStringValue("key").c_str(), keys[idx].c_str());
        }
    }
    //consumeDataCloudDelete
    {
        ASSERT_FALSE(client_channel->deleteDataCloud("search", std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()));
        ASSERT_FALSE(client_channel->deleteDataCloud("search", std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()));
    }
    //consumeDataCloudQuery
    {
        QueryResultPage results;
        SearchSequence sseq = {1,2};
        int page_dimension = 100;
        ASSERT_TRUE(client_channel->queryDataCloud("search", std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), page_dimension, sseq, results));
        ASSERT_FALSE(client_channel->queryDataCloud("search", std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), page_dimension, sseq, results));
        ASSERT_EQ(page_dimension, results.size());
        ASSERT_EQ(sseq.run_id, 2);
        ASSERT_EQ(sseq.datapack_counter, 3);
        for(int idx = 0;
            idx < results.size();
            idx++) {
            const std::string test_value = CHAOS_FORMAT("%1%",%idx);
            ASSERT_TRUE(results[idx]->hasKey("key"));
            ASSERT_TRUE(results[idx]->isStringValue("key"));
            ASSERT_STREQ(results[idx]->getStringValue("key").c_str(), test_value.c_str());
        }
    }
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
    if(server_channel){
        ASSERT_NO_THROW(endpoint->deregisterChannelInstance(server_channel););
    }
}
