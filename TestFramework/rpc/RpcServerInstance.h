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

#ifndef RpcServerInstance_hpp
#define RpcServerInstance_hpp

#include <set>
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

class RpcServerInstance {
    std::vector< ChaosSharedPtr<chaos::DeclareAction> > actions;
    ChaosUniquePtr<chaos::AbstractCommandDispatcher> rpc_dispatcher;
    ChaosUniquePtr<chaos::RpcClient> rpc_client;
    ChaosUniquePtr<chaos::RpcServer> rpc_server;
    void startup(int32_t port, std::vector< ChaosSharedPtr<chaos::DeclareAction> > actions);
public:
    RpcServerInstance(int32_t port, std::vector< ChaosSharedPtr<chaos::DeclareAction> > _actions);
    RpcServerInstance(int32_t port, ChaosSharedPtr<chaos::DeclareAction> _actions);
    RpcServerInstance(const CNetworkAddress& forced_address, ChaosSharedPtr<chaos::DeclareAction> _actions);
    ~RpcServerInstance();
    const CNetworkAddress getAddress();
};

#endif /* RpcServerInstance_hpp */
