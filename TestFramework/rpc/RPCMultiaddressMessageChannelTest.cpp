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

#include "RPCMultiaddressMessageChannelTest.h"

#include<chaos/common/global.h>
#include<chaos/common/chaos_constants.h>
#include<chaos/common/message/MultiAddressMessageChannel.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::message;
using namespace chaos::common::utility;
#pragma mark RpcHandler2
RpcHandler2::RpcHandler2():
actionWithResultCounter(0){
    //register the action for the response
    DeclareAction::addActionDescritionInstance<RpcHandler2>(this,
                                                            &RpcHandler2::actionWithResult,
                                                            "test_rpc",
                                                            "actionWithResult",
                                                            "actionWithResult");
}
RpcHandler2::~RpcHandler2(){}
CDWUniquePtr RpcHandler2::actionWithResult(CDWUniquePtr action_data) {
    actionWithResultCounter++;
    return action_data;
}

#pragma mark RpcServerInstance
RpcServerInstance::RpcServerInstance() {
    int32_t freeFoundPort = InetUtility::scanForLocalFreePort(8888);
    rpc_dispatcher.reset(ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher"));
    rpc_dispatcher->registerAction(&rpc_handler);
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_dispatcher.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_dispatcher.get(), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    
    // get the rpc type to instantiate
    rpc_server.reset(ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Server"));
    rpc_server->setAlternatePortAddress(freeFoundPort);
    rpc_server->setCommandDispatcher(rpc_dispatcher.get());
    EXPECT_NO_THROW({StartableService::initImplementation(rpc_server.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_server->getName(), __PRETTY_FUNCTION__);});
    
    string rpc_client_name = chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Client";
    rpc_client.reset(ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpc_client_name));
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_client.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_client->getName(), __PRETTY_FUNCTION__));
    rpc_dispatcher->setRpcForwarder(rpc_client.get());
    
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__));
}

RpcServerInstance::~RpcServerInstance() {
    EXPECT_NO_THROW({StartableService::stopImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__);});
    EXPECT_NO_THROW(StartableService::stopImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    EXPECT_NO_THROW({StartableService::stopImplementation(rpc_dispatcher.get(), "DefaultCommandDispatcher", __PRETTY_FUNCTION__);});
    
    EXPECT_NO_THROW({StartableService::deinitImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__);});
    EXPECT_NO_THROW(StartableService::deinitImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    EXPECT_NO_THROW({StartableService::deinitImplementation(rpc_dispatcher.get(), "DefaultCommandDispatcher", __PRETTY_FUNCTION__);});
}

const CNetworkAddress RpcServerInstance::getAddress() {
    return CNetworkAddress(CHAOS_FORMAT("127.0.0.1:%1%",%rpc_server->getPublishedPort()));
}

#pragma mark MultiaddressMessageChannelTest
RPCMultiaddressMessageChannelTest::RPCMultiaddressMessageChannelTest(){}
RPCMultiaddressMessageChannelTest::~RPCMultiaddressMessageChannelTest(){}

ChaosUniquePtr<RpcServerInstance> RPCMultiaddressMessageChannelTest::startRpcServer() {
    return ChaosUniquePtr<RpcServerInstance>(new RpcServerInstance());
}


void RPCMultiaddressMessageChannelTest::SetUp() {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(0, NULL);
    std::string local_ip = chaos::common::utility::InetUtility::scanForLocalNetworkAddress();
    chaos::GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);
    ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
}
void RPCMultiaddressMessageChannelTest::TearDown() {
    ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
}

#pragma mark Tests
TEST_F(RPCMultiaddressMessageChannelTest, BaseTest) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer();
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer();
    
    //allocate multiaddress message channel
    std::vector<CNetworkAddress> node_address = {ist_1->getAddress(), ist_2->getAddress()};
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel(node_address);
    
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    // first server respose
    ASSERT_TRUE(future->wait());
    
    // second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    
    ASSERT_TRUE(future->wait());
    
    //kill first server
    ist_1.reset();
    
    //try to sent message to second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    ASSERT_TRUE(future->wait());

    
    //recreate one
    ist_1 = startRpcServer();
    while(msg_chnl->checkIfAddressIsOnline(node_address[0]) == false) {
        sleep(1);
    }
    
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    ASSERT_TRUE(future->wait());
    ist_1.reset();
    ist_2.reset();
}
