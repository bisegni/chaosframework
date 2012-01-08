//
//  LLRpcApi.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 30/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "LLRpcApi.h"
#include "../../common/data/CDataWrapper.h"

using namespace boost;

using namespace chaos;
using namespace chaos::ui;

    //initializing static initialization
shared_ptr<RpcServer> LLRpcApi::rpcServer;
shared_ptr<RpcClient> LLRpcApi::rpcClient;
shared_ptr<CommandDispatcher> LLRpcApi::commandDispatcher;

/*
 LL Rpc Api static initialization it should be called once for application
 */
void LLRpcApi::initRpcApi(CDataWrapper *initSetup)  throw (CException) {
     LAPP_ << "Init LLRpcApi";
    if(!initSetup) throw CException(0, "No Init Setup Found", "LLRpcApi::initRpcApi");
    
        //get the dispatcher
    LAPP_ << "LLRpcApi::initRpcApi Initialization";
    commandDispatcher.reset(ObjectFactoryRegister<CommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher"));
    if(!commandDispatcher) throw CException(1, "No Dispatcher Found", "LLRpcApi::initRpcApi");
    
        //initiliazing command dispatcher
    commandDispatcher->init(initSetup);
    
        //read the configuration for adapter type
    
    if(!initSetup->hasKey(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_TYPE)) 
        throw CException(2, "No RPC Adapter type found in configuration", "LLRpcApi::initRpcApi");
    
    
    string adapterType = initSetup->getStringValue(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_TYPE);
    LAPP_ << "The Adapter is the type: " << adapterType;
        //get the rpc adaptor instance
    string serverName = adapterType+"Server";
    string clientName = adapterType+"Client";
    rpcServer.reset(ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(serverName.c_str()));
    if(rpcServer.get() != NULL){
        LAPP_ << "Got RPC Server: " << rpcServer->getName();
            //set the dispatcher into the rpc adapter
        rpcServer->setCommandDispatcher(commandDispatcher);
            //if has been found the adapter, initialize it
        LAPP_ << "Init Rpc Server";
        rpcServer->init(initSetup);
    }else{
        LAPP_ << "No RPC Adapter Server";
    }
    
    rpcClient.reset(ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(clientName.c_str()));
    if(rpcClient.get() != NULL){
        LAPP_ << "Got RPC Client: " << rpcClient->getName();
            //set the dispatcher into the rpc adapter
            //rpcServer->setCommandDispatcher(cmdDispatcher);
            //if has been found the adapter, initialize it
        LAPP_ << "Init Rpc Server";
        rpcClient->init(initSetup);
        commandDispatcher->setRpcClient(rpcClient);
    }else{
        LAPP_ << "No RPC Adapter Server";
    }

}
/*
 Deinitialization of LL rpc api
 */
void LLRpcApi::deinitRpcApi()  throw (CException) {
    LAPP_ << "Deinit LLRpcApi";
    LAPP_ << "Deinit RPC Server: " << rpcServer->getName();
    rpcServer->deinit();
    LAPP_ << "Deinit RPC Client: " << rpcClient->getName();
    rpcClient->deinit();
    LAPP_ << "Deinit command dispatcher: " << commandDispatcher->getName();
    commandDispatcher->deinit();
}

LLRpcApi::LLRpcApi() {
    
}

LLRpcApi::~LLRpcApi() {
    
}