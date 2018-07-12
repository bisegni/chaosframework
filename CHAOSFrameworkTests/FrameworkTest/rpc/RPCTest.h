//
//  RPCTest.hpp
//  FrameworkTest
//
//  Created by Claudio Bisegni on 11/07/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#ifndef RPCTest_hpp
#define RPCTest_hpp

#include <gtest/gtest.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/action/DeclareAction.h>
class RpcHandler:
public chaos::DeclareAction {
public:
    RpcHandler();
protected:
    chaos::common::data::CDataWrapper *actionWithResult(chaos::common::data::CDataWrapper *action_data, bool& detach);
    chaos::common::data::CDataWrapper *actionWithNoResult(chaos::common::data::CDataWrapper *action_data, bool& detach);
};

class RPCTest:
public testing::Test {
public:
    RPCTest(){}
    
    ~RPCTest(){}
    
    void SetUp() {
        chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
        chaos::GlobalConfiguration::getInstance()->parseStartupParameters(0, NULL);
        std::string local_ip = chaos::common::utility::InetUtility::scanForLocalNetworkAddress();
        chaos::GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);
        ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    }
    void TearDown() {
        ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
    }
};

#endif /* RPCTest_hpp */
