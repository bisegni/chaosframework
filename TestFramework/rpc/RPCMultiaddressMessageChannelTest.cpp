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

#define BASE_TEST_TCP_PORT 20000
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
    DEBUG_PRINTER("Message received:" << actionWithResultCounter);
    return action_data;
}

#pragma mark MultiaddressMessageChannelTest
RPCMultiaddressMessageChannelTest::RPCMultiaddressMessageChannelTest(){}
RPCMultiaddressMessageChannelTest::~RPCMultiaddressMessageChannelTest(){}

ChaosUniquePtr<RpcServerInstance> RPCMultiaddressMessageChannelTest::startRpcServer(int32_t port, ChaosSharedPtr<RpcHandler2> handler) {
    return ChaosUniquePtr<RpcServerInstance>(new RpcServerInstance(port, handler));
}
ChaosUniquePtr<RpcServerInstance> RPCMultiaddressMessageChannelTest::startRpcServer(const CNetworkAddress& forced_address, ChaosSharedPtr<RpcHandler2> handler) {
    return ChaosUniquePtr<RpcServerInstance>(new RpcServerInstance(forced_address, handler));
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

CDWUniquePtr getDatapack() {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    return pack;
}

TEST_F(RPCMultiaddressMessageChannelTest, AddRemoteURL) {
    ChaosSharedPtr<RpcHandler2> handler1 = ChaosMakeSharedPtr<RpcHandler2>();
    ChaosSharedPtr<RpcHandler2> handler2 = ChaosMakeSharedPtr<RpcHandler2>();
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(BASE_TEST_TCP_PORT, handler1);
    //allocate multiaddress message channel
    
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    DEBUG_PRINTER("got channel")
    //call to first server
    CDWUniquePtr data = getDatapack();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(data),
                                                                                              1000);
    DEBUG_PRINTER("Sent request")
    // false because no server are set
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("Wait end got what expcted")
    //add first
    msg_chnl->addNode(ist_1->getAddress());
    DEBUG_PRINTER("Add address")
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
                                             1000);
    // false because no server are set
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait end got what expcted")
    DEBUG_PRINTER("remove server_1 [" << ist_1->getAddress().ip_port << "]")
    msg_chnl->removeNode(ist_1->getAddress());
    ist_1.reset();
    //allocate second remote server
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer(BASE_TEST_TCP_PORT+1, handler2);
    DEBUG_PRINTER("Destroied server 1")
    msg_chnl->addNode(ist_2->getAddress());
    DEBUG_PRINTER("channel association to server [" << ist_2->getAddress().ip_port << "]")
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
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
    ChaosSharedPtr<RpcHandler2> handler1 = ChaosMakeSharedPtr<RpcHandler2>();
    ChaosSharedPtr<RpcHandler2> handler2 = ChaosMakeSharedPtr<RpcHandler2>();
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(BASE_TEST_TCP_PORT, handler1);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer(BASE_TEST_TCP_PORT+1, handler2);
    ASSERT_EQ(ist_2->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %(BASE_TEST_TCP_PORT+1)));
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
    CDWUniquePtr data = getDatapack();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(data),
                                                                                              1000);
    DEBUG_PRINTER("Sent request")
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    // second server respose
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
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
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_2.reset();
    DEBUG_PRINTER("Destroied server 2")
}

TEST_F(RPCMultiaddressMessageChannelTest, AutoEviction) {
    ChaosSharedPtr<RpcHandler2> handler1 = ChaosMakeSharedPtr<RpcHandler2>();
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(BASE_TEST_TCP_PORT, handler1);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
    const CNetworkAddress addr_srv_1 = ist_1->getAddress();
    //allocate multiaddress message channel
    
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->setEvitionHandler(ChaosBind(&RPCMultiaddressMessageChannelTest::evitionHandler, this, ChaosBindPlaceholder(_1)));
    msg_chnl->setAutoEvitionForDeadUrl(true);
    msg_chnl->addNode(addr_srv_1);
    DEBUG_PRINTER("Got channel");
    //call to first server
    CDWUniquePtr data = getDatapack();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(data),
                                                                                              1000);
    DEBUG_PRINTER("Sent request");
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
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
    ChaosSharedPtr<RpcHandler2> handler1 = ChaosMakeSharedPtr<RpcHandler2>();
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(BASE_TEST_TCP_PORT, handler1);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
    const CNetworkAddress addr1 = ist_1->getAddress();
    // allocate multiaddress message channel
    //    std::vector<CNetworkAddress> node_address = {ist_1->getAddress(), ist_2->getAddress()};
    MultiAddressMessageChannel *msg_chnl = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    msg_chnl->addNode(addr1);
    DEBUG_PRINTER("server_1 opened on " << ist_1->getAddress().ip_port);
    //call to first server
    CDWUniquePtr data = getDatapack();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(data),
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
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  chaos::ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE);
    DEBUG_PRINTER("Wait and got answer")
    //recreate one
    ist_1 = startRpcServer(BASE_TEST_TCP_PORT, handler1);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
    DEBUG_PRINTER("restarted server_1 on same port [" << ist_1->getAddress().ip_port << "]");
    //wait some time for permit messagechannel reconnection
    do{
        sleep(1);
        CDWUniquePtr data = getDatapack();
        future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                 "actionWithResult",
                                                 ChaosMoveOperator(data),
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
    ChaosSharedPtr<RpcHandler2> handler1 = ChaosMakeSharedPtr<RpcHandler2>();
    ChaosSharedPtr<RpcHandler2> handler2 = ChaosMakeSharedPtr<RpcHandler2>();
    //allcoate two remote server
    ChaosUniquePtr<RpcServerInstance> ist_1 = startRpcServer(BASE_TEST_TCP_PORT,handler1);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
    ChaosUniquePtr<RpcServerInstance> ist_2 = startRpcServer(BASE_TEST_TCP_PORT+1,handler2);
    ASSERT_EQ(ist_1->getAddress().ip_port, CHAOS_FORMAT("127.0.0.1:%1%", %BASE_TEST_TCP_PORT));
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
    CDWUniquePtr data = getDatapack();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture("test_rpc",
                                                                                              "actionWithResult",
                                                                                              ChaosMoveOperator(data),
                                                                                              1000);
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    
    
    //try to sent message to second server respose
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    //kill first server
    ist_1.reset();
    DEBUG_PRINTER("Destroied server 1")
    data = getDatapack();
    future = msg_chnl->sendRequestWithFuture("test_rpc",
                                             "actionWithResult",
                                             ChaosMoveOperator(data),
                                             1000);
    DEBUG_PRINTER("Sent request")
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    ist_2.reset();
    DEBUG_PRINTER("Destroied server 2")
}
