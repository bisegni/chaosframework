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

using namespace chaos::common::data;

#define BASE_TEST_TCP_PORT 20000

BestEndpointRPCHandler::BestEndpointRPCHandler(const std::vector<std::string>& _node_rpc_url):
node_rpc_url(_node_rpc_url){}
BestEndpointRPCHandler::~BestEndpointRPCHandler() {}

chaos::common::data::CDWUniquePtr BestEndpointRPCHandler::execute(CDWUniquePtr action_data) {
    CDWUniquePtr result(new CDataWrapper());
    
    for(std::vector<std::string>::const_iterator it = node_rpc_url.begin(),
        end = node_rpc_url.end();
        it != end;
        it++){
        result->appendStringToArray(*it);
    }
    result->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
    return result;
}

RPCMDSChannelTest::RPCMDSChannelTest() {}

RPCMDSChannelTest::~RPCMDSChannelTest() {}

void RPCMDSChannelTest::SetUp() {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(0, NULL);
    
    std::string local_ip = chaos::common::utility::InetUtility::scanForLocalNetworkAddress();
    chaos::GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);
    ASSERT_NO_THROW(chaos::common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), NULL, "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
}

void RPCMDSChannelTest::TearDown() {
    ASSERT_NO_THROW(chaos::common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
}

#pragma mark Tests
TEST_F(RPCMDSChannelTest, AutoFillEndpoint) {
//    CDWUniquePtr pack(new CDataWrapper());
//    pack->addStringValue("echo", "value");
//    std::vector<std::string> node_rpc_url;
//    //allocate the number of endpoint for test
//    node_rpc_url.push_back("localhost:20000");
//    node_rpc_url.push_back("localhost:20001");
//    node_rpc_url.push_back("localhost:20002");
//    BestEndpointRPCHandler bep_srv_1(node_rpc_url);
//    
//    //allcoate one remote server
//    ChaosUniquePtr<RpcServerInstance> ist_1(new RpcServerInstance(BASE_TEST_TCP_PORT, bep_srv_1);
//    //allocate multiaddress message channel
//    
//    ist_1.reset();
}
