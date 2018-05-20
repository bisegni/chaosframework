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

#include "DirectIODeviceChannelTest.h"
#include <chaos/common/global.h>


using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

unsigned int req_counter = 0;

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
            //right result
            CDWUniquePtr data(new CDataWrapper());
            data->addStringValue("key", "string");
            result_value = ChaosMakeSharedPtr<Buffer>();
            result_value->append(data->getBSONRawData(), data->getBSONRawSize());
            result_header.value_len = data->getBSONRawSize();
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
        return -1;
        
    };
    
    int consumeDataCloudDelete(const std::string& search_key,
                               uint64_t start_ts,
                               uint64_t end_ts) {
        return -1;
    }
};

TEST_F(DirectIODeviceChannelTest, DeviceChannelTest) {
    DeviceServerHandler handler;
    DirectIODeviceClientChannel *client_channel = NULL;
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    //    client_sys_channel  = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    
    client_channel = (DirectIODeviceClientChannel*)connection->getNewChannelInstance("DirectIODeviceClientChannel");
    ASSERT_TRUE(client_channel);
    BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
    BufferSPtr message_buffer_echo;
    
    //    client_sys_channel->echo(message_buffer, message_buffer_echo);
    
    void *result = NULL;
    uint32_t size = 0;
    int err = client_channel->requestLastOutputData("requestLastOutputData", &result, size);
    ASSERT_FALSE(client_channel->requestLastOutputData("requestLastOutputData", &result, size));
    ASSERT_TRUE(result && size);
    CDWUniquePtr result_cdw(new CDataWrapper((const char *)result));
    ASSERT_TRUE(result_cdw->hasKey("key"));
    ASSERT_TRUE(result_cdw->isStringValue("key"));
    ASSERT_STREQ(result_cdw->getStringValue("key").c_str(), "string");
    free(result);
    
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
    
}
