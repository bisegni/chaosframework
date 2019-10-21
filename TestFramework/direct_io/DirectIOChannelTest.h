//
//  DirectIOChannelTest.h
//  CHAOSFrameworkTests
//
//  Created by Claudio Bisegni on 19/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#ifndef DirectIOChannelTest_h
#define DirectIOChannelTest_h

#include <gtest/gtest.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/network/NetworkBroker.h>

class DirectIOChannelTest:
public testing::Test {
protected:
    chaos::common::direct_io::DirectIOServerEndpoint *endpoint;
    chaos::common::direct_io::DirectIOClientConnection *connection;
public:
    DirectIOChannelTest():
    endpoint(NULL),
    connection(NULL){}
    
    ~DirectIOChannelTest(){}
    
    void SetUp() {
        ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
        
        endpoint = chaos::common::network::NetworkBroker::getInstance()->getDirectIOServerEndpoint();
        ASSERT_TRUE(endpoint);
        
        connection = chaos::common::network::NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection("localhost:1672:30175|0");
        ASSERT_TRUE(connection);
    }
    void TearDown() {
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
#endif /* DirectIOChannelTest_h */
