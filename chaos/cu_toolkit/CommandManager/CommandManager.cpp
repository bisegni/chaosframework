//
//  CommandManager.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 21/02/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include <vector>
#include <iostream>

#include "../../Common/global.h"
#include "CommandManager.h"
#include "../../Common/cconstants.h"
#include "../ConfigurationManager/ConfigurationManager.h"
#include "../../Common/configuration/GlobalConfiguration.h"


using namespace chaos;
using namespace std;
using namespace boost;

#pragma mark Public Method
/*
 */
CommandManager::CommandManager(){
}

/*
 */
CommandManager::~CommandManager(){
}

/*
 * Initzialize the datamanager
 */
void CommandManager::init() throw(CException) {
    LAPP_ << "Init Command Manager";
    
        //auto_ptr<CDataWrapper> masterConfiguration(ConfigurationManager::getInstance()->getDefaultDomainConfigurationAsDataWrapper("master_configuration"));

        
    //get the dispatcher
    LAPP_ << "Get Global Command Dispatcher";
    cmdDispatcher.reset(ObjectFactoryRegister<CommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher"));
    
        //initiliazing command dispatcher
    cmdDispatcher->init(GlobalConfiguration::getInstance()->getConfiguration());
    
    startupRPCAdapter(GlobalConfiguration::getInstance()->getConfiguration());
    
    LAPP_ << "Command Manager system action registration";
    
    AbstActionDescShrPtr 
    actionDescription = DeclareAction::addActionDescritionInstance<CommandManager>(this, 
                                                                                   &CommandManager::shutdown, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_NAME, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_SHUTDOWN, 
                                                                                   "Shutdown the chaos control unit library instance");
        //registering the comman manager action
    cmdDispatcher->registerAction(this);
}

/*
 * Deinitzialize the command manager
 */
void CommandManager::deinit() throw(CException) {
    LAPP_ << "Deinit Command Manager";
    LAPP_ << "Command Manager system action deregistration";
        //deregistering the action
    cmdDispatcher->deregisterAction(this);
    LAPP_ << "Command Manager system action deregistered";
    
        //
    LAPP_ << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcServer->deinit();
    LAPP_ << "RPC Adapter deinitialized: " << rpcServer->getName();
    
    LAPP_ << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcClient->deinit();
    LAPP_ << "RPC Adapter deinitialized: " << rpcServer->getName();

        //deinitilizing Global command dispatcher
    cmdDispatcher->deinit();
}

/*
 * Start all sub process
 */
void CommandManager::start() throw(CException) {
    LAPP_ << "Start RPC Adapter: " << rpcServer->getName();
    rpcServer->start();
    LAPP_ << "RPC Adapter started: " << rpcServer->getName();
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* CommandManager::updateConfiguration(CDataWrapper*) {
    return NULL;
}

/*
 Send message over rpc channel
 */
void CommandManager::sendMessage(string& serverAndPort, CDataWrapper *message, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(rpcClient.get())
    rpcClient->submitMessage(serverAndPort, message, onThisThread);
}

/*
 Send message over rpc channel
 */
void CommandManager::sendMessageToMetadataServer(CDataWrapper *message, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(rpcClient.get())
    rpcClient->submitMessageToMetadataServer(message, onThisThread);
}

/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void CommandManager::registerAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(cmdDispatcher.get())
    cmdDispatcher->registerAction(declareActionClass);
}

/*
 Deregister actions for a determianted domain
 */
void CommandManager::deregisterAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(cmdDispatcher.get())
    cmdDispatcher->deregisterAction(declareActionClass);
}

/*
 Init the sandbox
 */
CDataWrapper* CommandManager::shutdown(CDataWrapper *actionParam) throw (CException) {
     CHAOS_ASSERT(privLibControllerPtr)
    if(privLibControllerPtr) privLibControllerPtr->stop();
    return NULL;
}
#pragma mark Private Method

  /*
   take care to startup an rpc adapter according to configuration
   */
void CommandManager::startupRPCAdapter(CDataWrapper *cmdMgrConfiguration) throw(CException) {
    
    if(!cmdMgrConfiguration) {
        throw CException(0, "No CommandManager RPC configuration passed", "CommandManager::init");
    }
    
    //read the configuration for adapter type
    
    if(!cmdMgrConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE)) {
        throw CException(0, "No RPC Adapter type found in configuration", "CommandManager::startupRPCAdapter");
    }
    
    string adapterType = cmdMgrConfiguration->getStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE);
    LAPP_ << "The Adapter is the type: "<<adapterType;
        //get the rpc adaptor instance
    string serverName = adapterType+"Server";
    string clientName = adapterType+"Client";
    LAPP_ << "Trying to startup RPC Server: " << serverName;
    rpcServer.reset(ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(serverName.c_str()));
    if(rpcServer.get() != NULL){
        LAPP_ << "Got RPC Server: " << rpcServer->getName();
            //set the dispatcher into the rpc adapter
            rpcServer->setCommandDispatcher(cmdDispatcher);
            //if has been found the adapter, initialize it
        LAPP_ << "Init Rpc Server";
        rpcServer->init(cmdMgrConfiguration);
    }else{
        LAPP_ << "No RPC Adapter Server";
    }
    LAPP_ << "Trying to startup RPC Client: " << clientName;
    rpcClient.reset(ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(clientName.c_str()));
    if(rpcClient.get() != NULL){
        LAPP_ << "Got RPC Client: " << rpcClient->getName();
            //set the dispatcher into the rpc adapter
            //rpcServer->setCommandDispatcher(cmdDispatcher);
            //if has been found the adapter, initialize it
        LAPP_ << "Init Rpc Server";
        rpcClient->init(cmdMgrConfiguration);
        cmdDispatcher->setRpcClient(rpcClient);
    }else{
        LAPP_ << "No RPC Adapter Server";
    }

}

