//
//  MessageBroker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MessageBroker.h"
#include <chaos/common/global.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>


#define MB_HEAD "[MessageBroker::init]-" 

using namespace chaos;

MessageBroker::MessageBroker(){
    canUseMetadataServer = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
    if(canUseMetadataServer){
        metadataServerAddress = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    }
}

MessageBroker::~MessageBroker() {
    
}

/*!
 * Initzialize the Message Broker. In this step are taken the configured implementation
 * for the rpc client and server and for the dispatcher. All these are here initialized
 */
void MessageBroker::init() throw(CException) {
    LAPP_ <<  MB_HEAD << "Init Message Broker";
    
    //get the dispatcher
    LAPP_ <<  MB_HEAD  << "Get DefaultCommandDispatcher implementation";
    commandDispatcher = ObjectFactoryRegister<CommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher");
    
    if(!commandDispatcher) throw CException(0, "CommandDispatcher implementation not found", "MessageBroker::init"); 
    
    //get global configuration reference
    CDataWrapper *globalConfiguration = GlobalConfiguration::getInstance()->getConfiguration();
    
    
    if(!globalConfiguration) {
        throw CException(0, "No global configuraiton found", "MessageBroker::init");
    }
    
    //read the configuration for adapter type
    if(!globalConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE)) {
        throw CException(0, "No RPC Adapter type found in configuration", "MessageBroker::init");
    }
    
    // get the rpc type to instantiate
    string adapterType = globalConfiguration->getStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE);
    LAPP_ <<  MB_HEAD  << "The Adapter type is: " << adapterType;
    //construct the rpc server and client name
    string serverName = adapterType+"Server";
    string clientName = adapterType+"Client";
    
    
    LAPP_ <<  MB_HEAD  << "Trying to startup RPC Server: " << serverName;
    rpcServer = ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(serverName.c_str());
    if(rpcServer){
        LAPP_ <<  MB_HEAD  << "Got RPC Server: " << rpcServer->getName();
        //set the dispatcher into the rpc adapter
        rpcServer->setCommandDispatcher(commandDispatcher);
        //if has been found the adapter, initialize it
        LAPP_ <<  MB_HEAD  << "Init Rpc Server";
        rpcServer->init(globalConfiguration);
    }else{
        LAPP_ <<  MB_HEAD  << "No RPC Adapter Server";
    }
    
    LAPP_ <<  MB_HEAD  << "Trying to startup RPC Client: " << clientName;
    rpcClient = ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(clientName.c_str());
    if(rpcClient){
        LAPP_ <<  MB_HEAD  << "Got RPC Client: " << rpcClient->getName();
        //set the dispatcher into the rpc adapter
        //rpcServer->setCommandDispatcher(cmdDispatcher);
        //if has been found the adapter, initialize it
        LAPP_ <<  MB_HEAD  << "Init Rpc Server";
        rpcClient->init(globalConfiguration);
        commandDispatcher->setRpcClient(rpcClient);
    }else{
        LAPP_ <<  MB_HEAD  << "No RPC Adapter Server";
    }
    
    LAPP_ <<  MB_HEAD  << "Message Broker Initialized";
}

/*!
 * All rpc adapter and command siaptcer are deinitilized
 */
void MessageBroker::deinit() throw(CException) {
    LAPP_ <<  MB_HEAD  << "Deinitilizing Message Broker";
    
    LAPP_ <<  MB_HEAD  << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcServer->deinit();
    LAPP_ <<  MB_HEAD  << "RPC Adapter deinitialized: " << rpcServer->getName();
    
    LAPP_ <<  MB_HEAD  << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcClient->deinit();
    LAPP_ <<  MB_HEAD  << "RPC Adapter deinitialized: " << rpcServer->getName();
    
    LAPP_ <<  MB_HEAD  << "Deinit Command Dispatcher";
    commandDispatcher->deinit();
    LAPP_ <<  MB_HEAD  << "Command Dispatcher deinitialized";

}

/*!
 * all part are started
 */
void MessageBroker::start() throw(CException){
    LAPP_ <<  MB_HEAD  << "Start RPC Adapter: " << rpcServer->getName();
    rpcServer->start();
    LAPP_ <<  MB_HEAD  << "RPC Adapter started: " << rpcServer->getName();
}

#pragma mark Action Registration
/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void MessageBroker::registerAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(commandDispatcher)
    commandDispatcher->registerAction(declareActionClass);
}

/*
 Deregister actions for a determianted domain
 */
void MessageBroker::deregisterAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(commandDispatcher)
    commandDispatcher->deregisterAction(declareActionClass);
}


#pragma mark Message Submission

/*!
 Submit a message and information for the destination servers are already setupped into CDataWrapper
 */
bool MessageBroker::submitMessage(CDataWrapper *message, bool onThisThread) {
    CHAOS_ASSERT(message)
    return rpcClient->submitMessage(message, onThisThread);
}

/*!
 Submit a message specifing the destination
 */
bool MessageBroker::submitMessage(string& serveAndPort, CDataWrapper *message, bool onThisThread) {
    return rpcClient->submitMessage(serveAndPort, message, onThisThread);
}


/*
 Submite a message specifieng the address
 */
bool MessageBroker::submitMessageToMetadataServer(CDataWrapper *message, bool onThisThread) {
    if(!canUseMetadataServer) return false;
    //check in debug for pointer
    CHAOS_ASSERT(message)
    // add the address to the message
    message->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, metadataServerAddress);
    //submite the message
    return submitMessage(message,onThisThread);
}

