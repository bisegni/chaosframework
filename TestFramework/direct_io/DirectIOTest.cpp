//
//  DirectIOTest.cpp
//  FrameworkTest
//
//  Created by bisegni on 15/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#include "DirectIOTest.h"
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/Random.h>

using namespace chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

ChaosAtomic<uint64_t> lost_eco_message;

#pragma mark DirectIOEchoHandler
class DirectIOEchoHandler:
public chaos::common::direct_io::channel::DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler {
protected:
    //! endpoint entry method
    int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                         chaos::common::data::BufferSPtr& output_data) {
        output_data = input_data;
        if(output_data == NULL ||
           output_data->data() == NULL) {
            return -1;
        }
        eco_count++;
        return  0;
    }
public:
    ChaosAtomic<uint64_t> eco_count;
};

#pragma mark DirectIOEchoDelayedHandler
class DirectIOEchoDelayedHandler:
public DirectIOEchoHandler {
    unsigned int echo_received_number;
protected:
    //! endpoint entry method
    int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                         chaos::common::data::BufferSPtr& output_data) {
        int err = DirectIOEchoHandler::consumeEchoEvent(input_data,
                                                        output_data);
        if((++echo_received_number % 2) == 0) {
            sleep(6);
        }
        return  err;
    }
public:
    DirectIOEchoDelayedHandler():echo_received_number(0){}
};

#pragma mark Test

TEST_F(DirectIOTest, Echo) {
    DirectIOEchoHandler handler;
    handler.eco_count=0;
    lost_eco_message = 0;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    
    std::string message_string_echo = "test_echo";
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    
    BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
    BufferSPtr message_buffer_echo;
    
    message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
    
    if(client_channel->echo(message_buffer, message_buffer_echo) == 0) {
        ASSERT_TRUE(message_buffer_echo);
        ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
    } else {
        lost_eco_message++;
    }
    
    const std::string echo_message_string(message_buffer_echo->data(), message_buffer_echo->size());
    ASSERT_STREQ(echo_message_string.c_str(), message_string_echo.c_str());
    std::cout <<"[          ] " << "eco_count = " << handler.eco_count << std::endl;
    std::cout <<"[          ] " << "lost_eco_message = " << lost_eco_message << std::endl;
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
}

void echoClientEchoMultiThreadingSameChannel(DirectIOSystemAPIClientChannel *client_channel) {
    chaos::common::utility::Random rnd(1,1000);
    std::string message_string_echo = "test_echo";
    
    for(int idx  = 0; idx < 1000; idx++) {
        BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
        BufferSPtr message_buffer_echo;
        
        message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
        
        if(client_channel->echo(message_buffer, message_buffer_echo) == 0) {
            ASSERT_TRUE(message_buffer_echo);
            ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
            
            const std::string echo_message_string(message_buffer_echo->data(), message_buffer_echo->size());
            ASSERT_STREQ(echo_message_string.c_str(), message_string_echo.c_str());
        } else {
            lost_eco_message++;
        }
        boost::this_thread::sleep_for(boost::chrono::microseconds(rnd.rand()));
    }
}

TEST_F(DirectIOTest, EchoMultiThreadingSameChannel) {
    DirectIOEchoHandler handler;
    handler.eco_count = 0;
    lost_eco_message = 0;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    //register echo handler
    server_channel->setHandler(&handler);
    
    client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    
    boost::thread_group tg;
    for(int idx = 0; idx < 10; idx++) {
        tg.add_thread(new boost::thread(echoClientEchoMultiThreadingSameChannel, client_channel));
    }
    tg.join_all();
    std::cout <<"[          ] " << "eco_count = " << handler.eco_count << std::endl;
    std::cout <<"[          ] " << "lost_eco_message = " << lost_eco_message << std::endl;
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
}

void echoClientEchoMultiThreadingDifferentChannel(chaos::common::direct_io::DirectIOClientConnection *connection) {
    chaos::common::utility::Random rnd(1,1000);
    std::string message_string_echo = "test_echo";
    DirectIOSystemAPIClientChannel *client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    for(int idx  = 0; idx < 1000; idx++) {
        BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
        BufferSPtr message_buffer_echo;
        
        message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
        
        if(client_channel->echo(message_buffer, message_buffer_echo) == 0){
            ASSERT_TRUE(message_buffer_echo);
            ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
            const std::string echo_message_string(message_buffer_echo->data(), message_buffer_echo->size());
            ASSERT_STREQ(echo_message_string.c_str(), message_string_echo.c_str());
        } else {
            lost_eco_message++;
        }
        
        boost::this_thread::sleep_for(boost::chrono::microseconds(rnd.rand()));
    }
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
}

TEST_F(DirectIOTest, EchoMultiThreadingDifferentChannel) {
    DirectIOEchoHandler handler;
    handler.eco_count = 0;
    lost_eco_message = 0;
    //register echo handler
    server_channel->setHandler(&handler);
    boost::thread_group tg;
    for(int idx = 0; idx < 100; idx++) {
        tg.add_thread(new boost::thread(echoClientEchoMultiThreadingDifferentChannel, connection));
    }
    tg.join_all();
    std::cout <<"[          ] " << "eco_count = " << handler.eco_count << std::endl;
    std::cout <<"[          ] " << "lost_eco_message = " << lost_eco_message << std::endl;
}

TEST_F(DirectIOTest, SendCicle) {
    DirectIOEchoHandler handler;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    std::string message_string_echo = "test_echo";
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    
    client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    
    BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
    BufferSPtr message_buffer_echo;
    
    message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
    for(int idx = 0; idx < 1000; idx++) {
        ASSERT_EQ(client_channel->echo(message_buffer, message_buffer_echo), 0);
        ASSERT_TRUE(message_buffer_echo);
        ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
        const std::string echo_message_string(message_buffer_echo->data(), message_buffer_echo->size());
        ASSERT_STREQ(echo_message_string.c_str(), message_string_echo.c_str());
    }
    
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
}

TEST_F(DirectIOTest, DelayedAnswer) {
    DirectIOEchoDelayedHandler handler;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    //register echo handler
    server_channel->setHandler(&handler);
    
    client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    
    BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
    BufferSPtr message_buffer_echo;
    
    for(int idx = 0; idx < 3; idx++) {
        message_buffer_echo.reset();
        std::string message_string_echo = CHAOS_FORMAT("test_echo_%1%",%idx);
        message_buffer->assign(message_string_echo.c_str(), message_string_echo.size());
        int err = client_channel->echo(message_buffer, message_buffer_echo);
        if(err) {continue;}
        ASSERT_TRUE(message_buffer_echo);
        ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
        const std::string message_string_echo_readed(message_buffer_echo->data(), message_buffer_echo->size());
        ASSERT_STREQ(message_string_echo_readed.c_str(), message_string_echo.c_str());
    }
    
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
}
