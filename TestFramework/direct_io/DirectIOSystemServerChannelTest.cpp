/*
 *    DirectIOChannelTest.cpp
 *
 *    !CHAOS [CHAOSFrameworkTests]
 *    Created by Claudio Bisegni.
 *
 *        Copyright 19/05/2018 INFN, National Institute of Nuclear Physics
 *
 *        Licensed under the Apache License, Version 2.0 (the "License");
 *        you may not use this file except in compliance with the License.
 *        You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Unless required by applicable law or agreed to in writing, software
 *        distributed under the License is distributed on an "AS IS" BASIS,
 *        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *        See the License for the specific language governing permissions and
 *        limitations under the License.
 */

#include "DirectIOChannelTest.h"
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>

#include <limits>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

unsigned int SystemApiServerHandler_req_counter = 0;

class SystemApiServerHandler:
public DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler {
public:
    
    int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                         chaos::common::data::BufferSPtr& output_data) {
        int err = 0;
        if((++SystemApiServerHandler_req_counter % 2) == 0) {
            output_data = ChaosMakeSharedPtr<Buffer>(*input_data);
            if(output_data == NULL ||
               output_data->data() == NULL) {
                return -1;
            }
        } else {
            //error
            err = -1;
        }
        return err;
    }
    
    int consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader& header,
                                       const std::string& producer_id,
                                       chaos::common::data::BufferSPtr& channel_found_data,
                                       DirectIOSystemAPISnapshotResultHeader &result_header) {
        int err = 0;
        if((++SystemApiServerHandler_req_counter % 2) == 0) {
            CDataWrapper snap_data;
            snap_data.addStringValue(producer_id, std::string(header.field.snap_name));
            
            channel_found_data = ChaosMakeSharedPtr<Buffer>();
            channel_found_data->append(snap_data.getBSONRawData(), snap_data.getBSONRawSize());
        } else {
            //error
            err = -1;
        }
        return err;
    }
    
    int consumeLogEntries(const std::string& node_name,
                          const ChaosStringVector& log_entries) {
        int err = 0;
        if((++SystemApiServerHandler_req_counter % 2) == 0) {
            if(node_name.compare("producer") != 0) {
                err = -1;
            } else if(log_entries.size() != 100 ) {
                err = -2;
            } else {
                for(ChaosStringVectorConstIterator it = log_entries.begin(),
                    end = log_entries.end();
                    it != end;
                    it++) {
                    if(it->compare("log entry") != 0) {
                        err = -3;
                        break;
                    }
                }
            }
        } else {
            //error
            err = -1;
        }
        return err;
    }
};

TEST_F(DirectIOChannelTest, SystemChannelTest) {
    SystemApiServerHandler handler;
    DirectIOSystemAPIServerChannel *server_channel = (DirectIOSystemAPIServerChannel*)endpoint->getNewChannelInstance("DirectIOSystemAPIServerChannel");
    ASSERT_TRUE(server_channel);
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    DirectIOSystemAPIClientChannel *client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    //--------------------perform test----------------------
    //echo
    for(int idx = 0; idx < 1000; idx++)
    {
        SystemApiServerHandler_req_counter = 0;
        BufferSPtr message_buffer_echo;
        const std::string message = "test_echo";
        BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
        
        message_buffer->append(message.c_str(), message.size());
        ASSERT_TRUE(client_channel->echo(message_buffer, message_buffer_echo));
        ASSERT_FALSE(client_channel->echo(message_buffer, message_buffer_echo));
        ASSERT_TRUE(message_buffer_echo);
        ASSERT_EQ(message_buffer_echo->size(), message.size());
        const std::string message_echo(message_buffer_echo->data(), message_buffer_echo->size());
        ASSERT_STREQ(message.c_str(), message_echo.c_str());
    }
    //snapshot data
    for(int idx = 0; idx < 1000; idx++)
    {
        SystemApiServerHandler_req_counter = 0;
        DirectIOSystemAPIGetDatasetSnapshotResult api_result_handle;
        ASSERT_TRUE(client_channel->getDatasetSnapshotForProducerKey("snap_name", "producer_name", 0, api_result_handle));
        ASSERT_FALSE(client_channel->getDatasetSnapshotForProducerKey("snap_name", "producer_name", 0, api_result_handle));
      
        ASSERT_EQ(api_result_handle.channel_data->getBSONRawSize(), api_result_handle.api_result.channel_data_len);
        ASSERT_TRUE(api_result_handle.channel_data->hasKey("producer_name"));
        ASSERT_TRUE(api_result_handle.channel_data->isStringValue("producer_name"));
        ASSERT_STREQ(api_result_handle.channel_data->getStringValue("producer_name").c_str(), "snap_name");
    }
    //log
    for(int idx = 0; idx < 1000; idx++)
    {
        SystemApiServerHandler_req_counter = 0;
        ChaosStringVector log_entries;
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        log_entries.push_back("log entry");
        ASSERT_FALSE(client_channel->pushLogEntries("producer", log_entries));
        ASSERT_FALSE(client_channel->pushLogEntries("producer", log_entries));
    }
    //--------------------perform test----------------------
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
    
    if(server_channel){
        ASSERT_NO_THROW(endpoint->releaseChannelInstance(server_channel););
    }
}
