//
//  DirectIOTest.cpp
//  FrameworkTest
//
//  Created by bisegni on 15/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#include "DirectIOTest.h"
#include <chaos/common/global.h>
#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#pragma mark DirectIOEchoHandler
int DirectIOEchoHandler::consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                                          chaos::common::data::BufferSPtr& output_data) {
    output_data = ChaosMakeSharedPtr<Buffer>(*input_data);
    if(output_data == NULL ||
       output_data->data() == NULL) {
        return -1;
    }
    return  0;
}
#pragma mark DirectIOEchoDelayedHandler
DirectIOEchoDelayedHandler::DirectIOEchoDelayedHandler():echo_received_number(0){}
int DirectIOEchoDelayedHandler::consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                                                 chaos::common::data::BufferSPtr& output_data) {
    int err = DirectIOEchoHandler::consumeEchoEvent(input_data,
                                                    output_data);
    if((++echo_received_number % 2) == 0) {
        sleep(6);
    }
    return  err;
}


#pragma mark DirectIOTest
DirectIOTest::DirectIOTest():
server_channel(NULL),
endpoint(NULL){}
DirectIOTest::~DirectIOTest(){}
void DirectIOTest::SetUp(){
    ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    
    endpoint = NetworkBroker::getInstance()->getDirectIOServerEndpoint();
    ASSERT_TRUE(endpoint);
    server_channel = (DirectIOSystemAPIServerChannel*)endpoint->getNewChannelInstance("DirectIOSystemAPIServerChannel");
    ASSERT_TRUE(server_channel);
}
void DirectIOTest::TearDown(){
    if(server_channel){
        ASSERT_NO_THROW(endpoint->deregisterChannelInstance(server_channel););
    }
    if(endpoint){
        ASSERT_NO_THROW(NetworkBroker::getInstance()->releaseDirectIOServerEndpoint(endpoint));
    }
    ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
}

TEST_F(DirectIOTest, Echo) {
    DirectIOEchoHandler handler;
    DirectIOClientConnection *connection = NULL;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    
    std::string message_string_echo = "test_echo";
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    connection = chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection("localhost:1972:30175|0");
    ASSERT_TRUE(connection);
    
    client_channel = (DirectIOSystemAPIClientChannel*)connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
    ASSERT_TRUE(client_channel);
    
    BufferSPtr message_buffer = ChaosMakeSharedPtr<Buffer>();
    BufferSPtr message_buffer_echo;
    
    message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
    
    ASSERT_EQ(client_channel->echo(message_buffer, message_buffer_echo), 0);
    
    ASSERT_TRUE(message_buffer_echo);
    ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
    
    const std::string echo_message_string(message_buffer_echo->data(), message_buffer_echo->size());
    ASSERT_STREQ(echo_message_string.c_str(), message_string_echo.c_str());
    
    if(client_channel){
        ASSERT_NO_THROW(connection->releaseChannelInstance(client_channel););
        client_channel = NULL;
    }
    if(connection) {
        chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->releaseConnection(connection);
        connection = NULL;
    }
}

TEST_F(DirectIOTest, SendCicle) {
    DirectIOEchoHandler handler;
    DirectIOClientConnection *connection = NULL;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    std::string message_string_echo = "test_echo";
    
    //register echo handler
    server_channel->setHandler(&handler);
    
    connection = chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection("localhost:1972:30175|0");
    ASSERT_TRUE(connection);
    
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
    if(connection) {
        chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->releaseConnection(connection);
        connection = NULL;
    }
}

TEST_F(DirectIOTest, DelayedAnswer) {
    DirectIOEchoDelayedHandler handler;
    DirectIOClientConnection *connection = NULL;
    DirectIOSystemAPIClientChannel *client_channel = NULL;
    //register echo handler
    server_channel->setHandler(&handler);
    
    connection = chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection("localhost:1972:30175|0");
    ASSERT_TRUE(connection);
    
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
    if(connection) {
        chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->releaseConnection(connection);
        connection = NULL;
    }
}
