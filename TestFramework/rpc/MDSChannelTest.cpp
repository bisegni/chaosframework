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

#include "MDSChannelTest.h"
#include <chaos/common/message/MDSMessageChannel.h>

#define DEBUG_PRINTER(x) std::cout << "[          ] " << x <<std::endl;

using namespace chaos::common::data;
using namespace chaos::common::message;

#define BASE_TEST_TCP_PORT 20000

BestEndpointRPCHandler::BestEndpointRPCHandler(const VectorNetworkAddress& _node_rpc_url):
node_rpc_url(_node_rpc_url){
    //register the action for the response
    DeclareAction::addActionDescritionInstance<BestEndpointRPCHandler>(this,
                                                            &BestEndpointRPCHandler::execute,
                                                            chaos::DataServiceNodeDomainAndActionRPC::RPC_DOMAIN,
                                                            "getBestEndpoints",
                                                            "getBestEndpoints");
}
BestEndpointRPCHandler::~BestEndpointRPCHandler() {}

chaos::common::data::CDWUniquePtr BestEndpointRPCHandler::execute(CDWUniquePtr action_data) {
    CDWUniquePtr result(new CDataWrapper());
    
    for(VectorNetworkAddressConstIterator it = node_rpc_url.begin(),
        end = node_rpc_url.end();
        it != end;
        it++){
        result->appendStringToArray(it->ip_port);
    }
    result->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
    return result;
}

MDSChannelTest::MDSChannelTest() {}

MDSChannelTest::~MDSChannelTest() {}

void MDSChannelTest::SetUp() {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(0, NULL);
    
    std::string local_ip = chaos::common::utility::InetUtility::scanForLocalNetworkAddress();
    chaos::GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);
    ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
}

void MDSChannelTest::TearDown() {
    ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
}

#pragma mark Tests
CDWUniquePtr getDatapackForMds() {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    return pack;
}

TEST_F(MDSChannelTest, AutoFillEndpoint) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    VectorNetworkAddress url_for_bep;
    //allocate the number of endpoint for test
    url_for_bep.push_back(CNetworkAddress("localhost:20000"));
    url_for_bep.push_back(CNetworkAddress("localhost:20001"));
    url_for_bep.push_back(CNetworkAddress("localhost:20002"));
    ChaosSharedPtr<BestEndpointRPCHandler> bepRPC = ChaosMakeSharedPtr<BestEndpointRPCHandler>(url_for_bep);
    
    VectorNetworkAddress init_mds;
    init_mds.push_back(CNetworkAddress("localhost:20000"));
    
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1(new RpcServerInstance(BASE_TEST_TCP_PORT, bepRPC));
    ChaosUniquePtr<RpcServerInstance> ist_2(new RpcServerInstance(BASE_TEST_TCP_PORT+1, bepRPC));
    ChaosUniquePtr<RpcServerInstance> ist_3(new RpcServerInstance(BASE_TEST_TCP_PORT+2, bepRPC));
    
    //allocate multiaddress message channel
    MDSMessageChannel *msg_chnl = NetworkBroker::getInstance()->getMetadataserverMessageChannel(init_mds);
    msg_chnl->setEndpointAutoConfiguration(true);
    DEBUG_PRINTER("got channel")
    
    while(msg_chnl->getNumberOfManagedNodes() != 3){
        sleep(1);
        DEBUG_PRINTER("Wait for new node auto configuration:" << msg_chnl->getNumberOfManagedNodes());
    }
    DEBUG_PRINTER("MDSChannel has autoconfigurato up to maximum server");
    NetworkBroker::getInstance()->disposeMessageChannel(msg_chnl);
    ist_1.reset();
    ist_2.reset();
    ist_3.reset();
}

TEST_F(MDSChannelTest, AutoFillEndpointAfterEviction) {
    int wait_iteration = 40;
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("echo", "value");
    VectorNetworkAddress url_for_bep;
    //allocate the number of endpoint for test
    url_for_bep.push_back(CNetworkAddress("localhost:20000"));
    url_for_bep.push_back(CNetworkAddress("localhost:20001"));
    url_for_bep.push_back(CNetworkAddress("localhost:20002"));
    ChaosSharedPtr<BestEndpointRPCHandler> bepRPC = ChaosMakeSharedPtr<BestEndpointRPCHandler>(url_for_bep);
    
    VectorNetworkAddress init_mds;
    init_mds.push_back(CNetworkAddress("localhost:20000"));
    init_mds.push_back(CNetworkAddress("localhost:20001"));
    init_mds.push_back(CNetworkAddress("localhost:20002"));
    //allcoate one remote server
    ChaosUniquePtr<RpcServerInstance> ist_1(new RpcServerInstance(BASE_TEST_TCP_PORT, bepRPC));
    ChaosUniquePtr<RpcServerInstance> ist_2(new RpcServerInstance(BASE_TEST_TCP_PORT+1, bepRPC));
    ChaosUniquePtr<RpcServerInstance> ist_3(new RpcServerInstance(BASE_TEST_TCP_PORT+2, bepRPC));
    
    //allocate multiaddress message channel
    MDSMessageChannel *msg_chnl = NetworkBroker::getInstance()->getMetadataserverMessageChannel(init_mds);
    msg_chnl->setEndpointAutoConfiguration(true);
    DEBUG_PRINTER("got channel")
    ASSERT_EQ(msg_chnl->getNumberOfManagedNodes(), 3);
    
    //stop one server and wait for eviction
    ist_1.reset();
    //send some request sto let channel discover the death server
    CDWUniquePtr data = getDatapackForMds();
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = msg_chnl->sendRequestWithFuture(chaos::NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                              chaos::NodeDomainAndActionRPC::ACTION_ECHO_TEST,
                                                                                              ChaosMoveOperator(data),
                                                                                              1000);
    DEBUG_PRINTER("Sent request")
    // first server respose
    ASSERT_TRUE(future->wait());
    ASSERT_EQ(future->getError(),  0);
    DEBUG_PRINTER("Wait and got answer")
    while(msg_chnl->getNumberOfManagedNodes() == 3 && wait_iteration){
        sleep(1);
        wait_iteration--;
        DEBUG_PRINTER("Wait for eviction");
    }
    ASSERT_NE(wait_iteration, 0);
    DEBUG_PRINTER("Restart server adn wait for auto configuration");
    ist_1.reset(new RpcServerInstance(BASE_TEST_TCP_PORT, bepRPC));
    wait_iteration = 40;
    while(msg_chnl->getNumberOfManagedNodes() != 3 && wait_iteration){
        sleep(1);
        wait_iteration--;
        DEBUG_PRINTER("Wait for autoconfiguration");
    }
    ASSERT_NE(wait_iteration, 0);
    DEBUG_PRINTER("MDSChannel has autoconfigurato up to maximum server");
    NetworkBroker::getInstance()->disposeMessageChannel(msg_chnl);
    ist_1.reset();
    ist_2.reset();
    ist_3.reset();
}
