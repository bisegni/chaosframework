//
//  DirectIOTest.cpp
//  FrameworkTest
//
//  Created by bisegni on 15/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#include "DirectIOTest.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#pragma mark DirectIOEchoHandler
int DirectIOEchoHandler::consumeDataPack(DirectIODataPackSPtr data_pack,
                                         DirectIODataPackSPtr& synchronous_answer) {
    return -1;
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
    
    BufferSPtr message_buffer;
    BufferSPtr message_buffer_echo;
    
    message_buffer->append(message_string_echo.c_str(), message_string_echo.size());
    
    ASSERT_EQ(client_channel->echo(message_buffer, message_buffer_echo), 0);
    
    ASSERT_TRUE(message_buffer_echo);
    ASSERT_EQ(message_buffer_echo->size(), message_string_echo.size());
    ASSERT_STREQ(message_buffer_echo->data(), message_string_echo.c_str());
}
