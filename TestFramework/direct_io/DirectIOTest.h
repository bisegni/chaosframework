//
//  DirectIOTest.hpp
//  FrameworkTest
//
//  Created by bisegni on 15/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#ifndef DirectIOTest_hpp
#define DirectIOTest_hpp

#include <gtest/gtest.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/network/NetworkBroker.h>

class DirectIOTest:
public testing::Test {
protected:
    chaos::common::direct_io::DirectIOServerEndpoint *endpoint;
    chaos::common::direct_io::channel::DirectIOSystemAPIServerChannel *server_channel;
    chaos::common::direct_io::DirectIOClientConnection *connection;
public:
    DirectIOTest():
    server_channel(NULL),
    endpoint(NULL),
    connection(NULL){}

    ~DirectIOTest(){}

    void SetUp() {
        ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););

        endpoint = chaos::common::network::NetworkBroker::getInstance()->getDirectIOServerEndpoint();
        ASSERT_TRUE(endpoint);
        server_channel = (chaos::common::direct_io::channel::DirectIOSystemAPIServerChannel*)endpoint->getNewChannelInstance("DirectIOSystemAPIServerChannel");
        ASSERT_TRUE(server_channel);

        connection = chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection("localhost:1672:30175|0");
        ASSERT_TRUE(connection);
    }
    void TearDown() {
        if(server_channel){
            ASSERT_NO_THROW(endpoint->releaseChannelInstance(server_channel););
        }
        if(endpoint){
            ASSERT_NO_THROW(chaos::common::network::NetworkBroker::getInstance()->releaseDirectIOServerEndpoint(endpoint));
        }

        if(connection) {
            chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->releaseConnection(connection);
            connection = NULL;
        }

        ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
    }
};
#endif /* DirectIOTest_hpp */
