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
#ifndef RPCTest_hpp
#define RPCTest_hpp

#include <gtest/gtest.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/action/DeclareAction.h>
class RpcHandler:
public chaos::DeclareAction {
public:
    ChaosAtomic<uint32_t> actionWithResultCounter;
    ChaosAtomic<uint32_t> actionWithNoResultCounter;
    RpcHandler();
protected:
    chaos::common::data::CDWUniquePtr actionWithResult(chaos::common::data::CDWUniquePtr action_data);
    chaos::common::data::CDWUniquePtr actionWithNoResult(chaos::common::data::CDWUniquePtr action_data);
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
        ASSERT_NO_THROW(chaos::common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), NULL, "AsyncCentralManager", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    }
    void TearDown() {
        ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
        ASSERT_NO_THROW(chaos::common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__););
    }
};

#endif /* RPCTest_hpp */
