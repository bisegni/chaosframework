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
#include "RpcServerInstance.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::message;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

#define DEBUG_PRINTER(x) std::cout << "[          ] " << x <<std::endl;

RpcServerInstance::RpcServerInstance(int32_t port, std::vector< ChaosSharedPtr<chaos::DeclareAction> > _actions):
actions(_actions){
     startup(port, actions);
}

RpcServerInstance::RpcServerInstance(int32_t port, ChaosSharedPtr<chaos::DeclareAction> action) {
    actions.push_back(action);
    startup(port, actions);
}

RpcServerInstance::RpcServerInstance(const CNetworkAddress& forced_address, ChaosSharedPtr<chaos::DeclareAction> action) {
    actions.push_back(action);
    std::string port_str = forced_address.ip_port.substr(forced_address.ip_port.find(":")+1);
    startup(std::atoi(port_str.c_str()), actions);
}

void RpcServerInstance::startup(int32_t port, std::vector< ChaosSharedPtr<chaos::DeclareAction> > actions) {
    DEBUG_PRINTER("Open new server on port " << port);
//    DEBUG_PRINTER("Create dispatcher")
    rpc_dispatcher.reset(ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher"));
    
//    DEBUG_PRINTER("Create rpc client")
    string rpc_client_name = chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Client";
    rpc_client.reset(ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpc_client_name));
//    DEBUG_PRINTER("Create rpc server")
    rpc_server.reset(ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(chaos::GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION)+"Server"));
    rpc_server->setAlternatePortAddress(port);
    
    
//    DEBUG_PRINTER("Init rpc dispatcher")
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_dispatcher.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    
//    DEBUG_PRINTER("register action")
    for(std::vector< ChaosSharedPtr<chaos::DeclareAction> >::const_iterator it = actions.begin(),
        end = actions.end();
        it != end;
        it++) {
      rpc_dispatcher->registerAction((*it).get());
    }
//    DEBUG_PRINTER("Set client in dispatcher")
    rpc_dispatcher->setRpcForwarder(rpc_client.get());
//    DEBUG_PRINTER("Set dispatcher in server")
    rpc_server->setCommandDispatcher(rpc_dispatcher.get());
    
//    DEBUG_PRINTER("Init rpc Server")
    EXPECT_NO_THROW({StartableService::initImplementation(rpc_server.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_server->getName(), __PRETTY_FUNCTION__);});
//    DEBUG_PRINTER("Init rpc client")
    EXPECT_NO_THROW(StartableService::initImplementation(rpc_client.get(), static_cast<void*>(chaos::GlobalConfiguration::getInstance()->getConfiguration()), rpc_client->getName(), __PRETTY_FUNCTION__));
    
//    DEBUG_PRINTER("Start rpc dispatcher")
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_dispatcher.get(), "DefaultCommandDispatcher", __PRETTY_FUNCTION__));
    
//    DEBUG_PRINTER("Start RPC Client")
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_client.get(), rpc_client->getName(), __PRETTY_FUNCTION__));
//    DEBUG_PRINTER("Start RPC Server")
    EXPECT_NO_THROW(StartableService::startImplementation(rpc_server.get(), rpc_server->getName(), __PRETTY_FUNCTION__));
    
    //wait for open port
    // while(!portInUse(port));
    DEBUG_PRINTER("Server opened on port " << port);
}

RpcServerInstance::~RpcServerInstance() {
    for(std::vector< ChaosSharedPtr<chaos::DeclareAction> >::const_iterator it = actions.begin(),
        end = actions.end();
        it != end;
        it++) {
      rpc_dispatcher->deregisterAction((*it).get());
    }
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
