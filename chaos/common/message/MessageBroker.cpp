//
//  MessageBroker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "MessageBroker.h"
#include <chaos/common/global.h>
#include <boost/lexical_cast.hpp>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#define MB_LAPP LAPP_ << "[MessageBroker::init]-" 

#define INIT_STEP   0
#define DEINIT_STEP 1

using namespace chaos;

   
/*!
 
 */
MessageBroker::MessageBroker(){
    rpcServer = NULL;
    rpcClient = NULL;
    commandDispatcher = NULL;
    canUseMetadataServer = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
    if(canUseMetadataServer){
        metadataServerAddress = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    }
}

/*!
 */
MessageBroker::~MessageBroker() {
    
}

/*!
 * Initzialize the Message Broker. In this step are taken the configured implementation
 * for the rpc client and server and for the dispatcher. All these are here initialized
 */
void MessageBroker::init() throw(CException) {
    //check if initialized
    SetupStateManager::levelUpFrom(INIT_STEP, "MessageBroker already initialized");
    
    MB_LAPP << "Init Message Broker";
    
    //get the dispatcher
    MB_LAPP  << "Get DefaultCommandDispatcher implementation";
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
    MB_LAPP  << "The Adapter type is: " << adapterType;
    //construct the rpc server and client name
    string serverName = adapterType+"Server";
    string clientName = adapterType+"Client";
    
    
    MB_LAPP  << "Trying to startup RPC Server: " << serverName;
    rpcServer = ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(serverName.c_str());
    if(rpcServer){
        MB_LAPP  << "Got RPC Server: " << rpcServer->getName();
        //set the dispatcher into the rpc adapter
        rpcServer->setCommandDispatcher(commandDispatcher);
        //if has been found the adapter, initialize it
        MB_LAPP  << "Init Rpc Server";
        rpcServer->init(globalConfiguration);
    }else{
        MB_LAPP  << "No RPC Adapter Server";
    }
    
    MB_LAPP  << "Trying to startup RPC Client: " << clientName;
    rpcClient = ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(clientName.c_str());
    if(rpcClient){
        MB_LAPP  << "Got RPC Client: " << rpcClient->getName();
        //set the dispatcher into the rpc adapter
        //rpcServer->setCommandDispatcher(cmdDispatcher);
        //if has been found the adapter, initialize it
        MB_LAPP  << "Init Rpc Server";
        rpcClient->init(globalConfiguration);
        commandDispatcher->setRpcClient(rpcClient);
    }else{
        MB_LAPP  << "No RPC Adapter Server";
    }
    
    
    MB_LAPP  << "Message Broker Initialized";
}

/*!
 * All rpc adapter and command siaptcer are deinitilized
 */
void MessageBroker::deinit() throw(CException) {
    
    //lock esclusive access to init phase
    SetupStateManager::levelUpFrom(DEINIT_STEP, "MessageBroker already deinitialized");

    MB_LAPP  << "Deinitilizing Message Broker";
    
    
    for (map<string, MessageChannel*>::iterator channnelIter = activeChannel.begin(); 
         channnelIter != activeChannel.end(); 
         channnelIter++) {
        //deinit channel
        MB_LAPP  << "Deinit channel: " << channnelIter->second->channelID;
        channnelIter->second->deinit();
        MB_LAPP  << "Deleting channel: " << channnelIter->second->channelID;
        delete(channnelIter->second);
    }
    MB_LAPP  << "Clear channel map";
    activeChannel.clear();

    MB_LAPP  << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcServer->deinit();
    MB_LAPP  << "RPC Adapter deinitialized: " << rpcServer->getName();
    
    MB_LAPP  << "Deinit RPC Adapter: " << rpcServer->getName();
    rpcClient->deinit();
    MB_LAPP  << "RPC Adapter deinitialized: " << rpcServer->getName();
    
    MB_LAPP  << "Deinit Command Dispatcher";
    commandDispatcher->deinit();
    MB_LAPP  << "Command Dispatcher deinitialized";

}

/*!
 * all part are started
 */
void MessageBroker::start() throw(CException){
    MB_LAPP  << "Start RPC Adapter: " << rpcServer->getName();
    rpcServer->start();
    
    MB_LAPP << "get the published host and port from rpc server";
    getPublishedHostAndPort(publishedHostAndPort);
    MB_LAPP << "Rpc server has been published in: " << publishedHostAndPort;
    
    MB_LAPP  << "RPC Adapter started: " << rpcServer->getName();
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

/*!
 Return the port where the rpc server has been published
 */
int MessageBroker::getPublishedPort() {
    CHAOS_ASSERT(rpcServer);
    return rpcServer->getPublishedPort();
}

/*!
 Fill the parameter withe rigth value of host and port for the internale
 rpc server of message broker
 */
void MessageBroker::getPublishedHostAndPort(string& hostAndPort) {
    CHAOS_ASSERT(rpcServer);

    hostAndPort = GlobalConfiguration::getInstance()->getLocalServerAddress();
    hostAndPort.append(":");
    hostAndPort.append(lexical_cast<string>(rpcServer->getPublishedPort()));
}

#pragma mark Message Submission

/*!
 Submit a message and information for the destination servers are already setupped into CDataWrapper
 */
bool MessageBroker::submitMessage(CDataWrapper *message, bool onThisThread) {
    CHAOS_ASSERT(message && rpcClient)
    return rpcClient->submitMessage(message, onThisThread);
}

/*!
 Submit a message specifing the destination
 */
bool MessageBroker::submitMessage(string& serveAndPort, CDataWrapper *message, bool onThisThread) {
    CHAOS_ASSERT(message && rpcClient)
    //add answer id to datawrapper
    return rpcClient->submitMessage(serveAndPort, message, onThisThread);
}

/*!
 Submite a new request to send to the remote host
 */
bool MessageBroker::submiteRequest(string& serveAndPort,  CDataWrapper *request, bool onThisThread) {
    CHAOS_ASSERT(request && rpcClient)
    request->addStringValue(RpcActionDefinitionKey::CS_CMDM_RESPONSE_HOST_IP, publishedHostAndPort);
    return rpcClient->submitMessage(serveAndPort, request, onThisThread);
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

/*
 */
MessageChannel *MessageBroker::getNewMessageChannelForremoteHost(string& remoteHost, EntityType type) {
    MessageChannel *channel = new MessageChannel(this, metadataServerAddress);
    
    channel->init();
    
    activeChannel.insert(make_pair(channel->channelID, channel));
    return channel;
}
