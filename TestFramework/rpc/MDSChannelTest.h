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

#ifndef MDSChannelTest_hpp
#define MDSChannelTest_hpp
#include "RpcServerInstance.h"
#include <gtest/gtest.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/network/URLHAServiceFeeder.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/action/DeclareAction.h>

class BestEndpointRPCHandler:
public chaos::DeclareAction {
    const VectorNetworkAddress node_rpc_url;
public:
    BestEndpointRPCHandler(const VectorNetworkAddress& _node_rpc_url);
    ~BestEndpointRPCHandler();
protected:
    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr action_data);
};

class MDSChannelTest:
public testing::Test {
public:
    MDSChannelTest();
    ~MDSChannelTest();
    void SetUp();
    void TearDown();
    void evitionHandler(const chaos::common::network::ServiceRetryInformation& sri);
};

#endif /* MDSChannelTest_hpp */
