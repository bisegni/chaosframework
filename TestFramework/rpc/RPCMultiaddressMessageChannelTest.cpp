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
using namespace chaos::common::async_central;

#define DEBUG_PRINTER(x) std::cout << "[          ] " << x <<std::endl;

#pragma mark util
static bool portInUse(unsigned short port) {
    using namespace boost::asio;
    using ip::tcp;
    io_service svc;
    tcp::acceptor a(svc);
    boost::system::error_code ec;
    a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), port }, ec);
    return ec == boost::system::errc::address_in_use;
}

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
int32_t RpcServerInstance::freeFoundPort = 10000;
RpcServerInstance::RpcServerInstance() {
    freeFoundPort = InetUtility::scanForLocalFreePort(freeFoundPort++);
    startup(freeFoundPort);
}
RpcServerInstance::RpcServerInstance(int32_t port) {
    startup(port);
}

RpcServerInstance::RpcServerInstance(const CNetworkAddress& forced_address) {
    std::string port_str = forced_address.ip_port.substr(forced_address.ip_port.find(":")+1);
    startup(std::atoi(port_str.c_str()));
}

void RpcServerInstance::startup(int32_t port) {
    DEBUG_PRINTER("Open new server on port" << port);
    rpc_dispatcher.reset(ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher"));
    rpc_dispatcher->registerAction(&rpc_handler);
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_dispatcher.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_dispatcher.get(), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    
    // get the rpc type to instantiate
    rpc_server.reset(ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Server"));
    rpc_server->setAlternatePortAddress(port);
    
    EXPECT_NO_THROW({StartableService::initImplementation(rpc_server.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_server->getName(), __PRETTY_FUNCTION__);});
    
    string rpc_client_name = chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Client";
    rpc_client.reset(ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpc_client_name));
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_client.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_client->getName(), __PRETTY_FUNCTION__));
    
    rpc_dispatcher->setRpcForwarder(rpc_client.get());
    rpc_server->setCommandDispatcher(rpc_dispatcher.get());
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__));
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    
    //wait for open port
    while(!portInUse(port));
    DEBUG_PRINTER("Server opened on port" << port);
}

RpcServerInstance::~RpcServerInstance() {
    EXPECT_NO_THROW(StartableService::stopImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    EXPECT_NO_THROW({StartableService::stopImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__);});
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
ChaosUniquePtr<RpcServerInstance> RPCMultiaddressMessageChannelTest::startRpcServer(int32_t port) {
    return ChaosUniquePtr<RpcServerInstance>(new RpcServerInstance(port));
}
ChaosUniquePtr<RpcServerInstance> RPCMultiaddressMessageChannelTest::startRpcServer(const CNetworkAddress& forced_address) {
    return ChaosUniquePtr<RpcServerInstance>(new RpcServerInstance(forced_address));
}

void RPCMultiaddressMessageChannelTest::evitionHandler(const chaos::common::network::ServiceRetryInformation& sri) {
    evicted_url.insert(sri.service_url);
}

void RPCMultiaddressMessageChannelTest::SetUp() {
    evicted_url.clear();
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
TEST_F(RPCMultiaddressMessageChannelTest, AddRemoteURL) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(10000);
    DEBUG_PRINTER("server_1 opened on " << ist_1->getAddress().ip_port)
    //allocate multiaddress message channel
    
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    DEBUG_PRINTER("got channel")
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    DEBUG_PRINTER("Sent request")
    // false because no server are set
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("Wait end got what expcted")
    //add first
    msg_chnl->addNode(ist_1->getAddress());
    DEBUG_PRINTER("Add address")
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    // false because no server are set
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait end got what expcted")
    DEBUG_PRINTER("remove server_1 [" << ist_1->getAddress().ip_port << "]")
    msg_chnl->removeNode(ist_1->getAddress());
    
    //allocate second remote server
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer();
    //stop first server because now second has different port
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    msg_chnl->addNode(ist_2->getAddress());
    DEBUG_PRINTER("server_2 opened on " << ist_2->getAddress().ip_port)
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    // false because no server are set
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait end got what expcted")
    ist_2.reset();
    DEBUG_PRINTER("Destroied server 2")
}

TEST_F(RPCMultiaddressMessageChannelTest, RemoveRemoteURL) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(10000);
    ASSERT_EQ(ist_1->getAddress().ip_port, "127.0.0.1:10000");
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer(10001);
    ASSERT_EQ(ist_2->getAddress().ip_port, "127.0.0.1:10001");
    const CNetworkAddress addr_srv_1 = ist_1->getAddress();
    const CNetworkAddress addr_srv_2 = ist_2->getAddress();
    
    ASSERT_NE(addr_srv_1.ip_port, addr_srv_2.ip_port);
    //allocate multiaddress message channel
    //    std::vector<CNetworkAddress> node_address = {ist_1->getAddress(), ist_2->getAddress()};
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->addNode(ist_1->getAddress());
    msg_chnl->addNode(ist_2->getAddress());
    DEBUG_PRINTER("- server_1 opened on " << addr_srv_1.ip_port);
    DEBUG_PRINTER("- server_2 opened on " << addr_srv_2.ip_port);
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    DEBUG_PRINTER("Sent request")
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    // second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    //kill first server
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    //remove it
    msg_chnl->removeNode(addr_srv_1);
    DEBUG_PRINTER("remove server_1 [" << addr_srv_1.ip_port << "]")
    //try to sent message to second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_2.reset();
    DEBUG_PRINTER("Destroied server 2")
}

TEST_F(RPCMultiaddressMessageChannelTest, AutoEviction) {
    //    ChaosBind(&PropertyCollector::changeHandler, this, ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3))
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(10000);
    const CNetworkAddress addr_srv_1 = ist_1->getAddress();
    DEBUG_PRINTER("server_1 opened on " << addr_srv_1.ip_port);
    //allocate multiaddress message channel
    
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->setEvitionHandler(ChaosBind(&RPCMultiaddressMessageChannelTest::evitionHandler, this, ChaosBindPlaceholder(_1)));
    msg_chnl->setAutoEvitionForDeadUrl(true);
    msg_chnl->addNode(addr_srv_1);
    DEBUG_PRINTER("Got channel");
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    DEBUG_PRINTER("Sent request");
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request");
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("Wait and got answer")
    //waith for evicction
    DEBUG_PRINTER("wait for eviciton server_1 [" << addr_srv_1.ip_port << "]");
    while(msg_chnl->hasNode(addr_srv_1)) {
        sleep(1);
    }
    DEBUG_PRINTER("evicted server_1 [" << addr_srv_1.ip_port << "]");
    ASSERT_EQ(evicted_url.size(), 1);
    ASSERT_NE(evicted_url.find(addr_srv_1.ip_port), evicted_url.end());
}

TEST_F(RPCMultiaddressMessageChannelTest, Reconnection) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(10000);
    ASSERT_EQ(ist_1->getAddress().ip_port, "127.0.0.1:10000");
    const CNetworkAddress addr1 = ist_1->getAddress();
    // allocate multiaddress message channel
    //    std::vector<CNetworkAddress> node_address = {ist_1->getAddress(), ist_2->getAddress()};
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->addNode(addr1);
    DEBUG_PRINTER("server_1 opened on " << ist_1->getAddress().ip_port);
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    DEBUG_PRINTER("Sent request");
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    //kill first server
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    //try to sent message to second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("Wait and got answer")
    //recreate one
    ist_1 = startRpcServer(10000);
    ASSERT_EQ(ist_1->getAddress().ip_port, "127.0.0.1:10000");
    DEBUG_PRINTER("restarted server_1 on same port [" << ist_1->getAddress().ip_port << "]");
    //wait some time for permit messagechannel reconnection
    do{
        sleep(1);
        
        future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                 "actionWithResult",
                                                 ChaosMoveOperator(pack),
                                                 1000);
        DEBUG_PRINTER("Sent request")
        ASSERT_TRUE(future->wait());
        //    ASSERT_EQ(future->getError(),  0);
    } while (future->getError() == chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("reconnected server_1 [" << ist_1->getAddress().ip_port << "]");
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
}

TEST_F(RPCMultiaddressMessageChannelTest, Failover) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(10000);
    ASSERT_EQ(ist_1->getAddress().ip_port, "127.0.0.1:10000");
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer(10001);
    ASSERT_EQ(ist_2->getAddress().ip_port, "127.0.0.1:10001");
    const CNetworkAddress addr1 = ist_1->getAddress();
    const CNetworkAddress addr2 = ist_2->getAddress();
    // allocate multiaddress message channel
    //    std::vector<CNetworkAddress> node_address = {ist_1->getAddress(), ist_2->getAddress()};
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->addNode(addr1);
    msg_chnl->addNode(addr2);
    DEBUG_PRINTER("server_1 opened on " << addr1.ip_port);
    DEBUG_PRINTER("server_2 opened on " << addr2.ip_port);
    //call to first server
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(pack),
                                                                                              1000);
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    
    
    //try to sent message to second server respose
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    //kill first server
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(pack),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_2.reset();
    DEBUG_PRINTER("Destroied server 2")
}
