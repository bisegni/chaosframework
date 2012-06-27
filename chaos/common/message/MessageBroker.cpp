/*	
 *	MessageBroker.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#include "MessageBroker.h"
#include <chaos/common/global.h>
#include <boost/lexical_cast.hpp>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/MessageChannel.h>

#define MB_LAPP LAPP_ << "[MessageBroker::init]- " 

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
    SetupStateManager::levelDownFrom(DEINIT_STEP, "MessageBroker already deinitialized");

    MB_LAPP  << "Deinitilizing Message Broker";
    
    
    for (map<string, MessageChannel*>::iterator channnelIter = activeChannel.begin(); 
         channnelIter != activeChannel.end(); 
         channnelIter++) {
        
        MessageChannel *messageChannelToDispose = channnelIter->second;
        
        //deinit channel
        messageChannelToDispose->deinit();
        
            //dispose it
        delete(messageChannelToDispose);
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
    request->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP, publishedHostAndPort);
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
MessageChannel *MessageBroker::getNewMessageChannelForRemoteHost(CNodeNetworkAddress *nodeNetworkAddress, EntityType type) {
    CHAOS_ASSERT(nodeNetworkAddress)
    MessageChannel *channel = NULL;
    switch (type) {
        case RAW:
            channel = new MessageChannel(this, nodeNetworkAddress->ipPort.c_str());
            delete(nodeNetworkAddress);
            break;
            
        case MDS:
            channel = new MDSMessageChannel(this, nodeNetworkAddress);
            break;
            
        case DEVICE:
            channel = new DeviceMessageChannel(this, static_cast<CDeviceNetworkAddress*>(nodeNetworkAddress));
            break;
    }
    //check if the channel has been created
    if(channel){
        channel->init();
        boost::mutex::scoped_lock lock(mapChannelAcces);
        activeChannel.insert(make_pair(channel->channelID, channel));
    }
    return channel;
}

//!Metadata server channel creation
/*!
 Performe the creation of metadata server
 */
MDSMessageChannel *MessageBroker::getMetadataserverMessageChannel(string& remoteHost) {
    CNodeNetworkAddress *mdsNodeAddr = new CNodeNetworkAddress();
    mdsNodeAddr->ipPort = remoteHost.c_str();
    mdsNodeAddr->nodeID = "system";
    return static_cast<MDSMessageChannel*>(getNewMessageChannelForRemoteHost(mdsNodeAddr, MDS));
}

//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
DeviceMessageChannel *MessageBroker::getDeviceMessageChannelFromAddress(CDeviceNetworkAddress *deviceNetworkAddress) {
    return static_cast<DeviceMessageChannel*>(getNewMessageChannelForRemoteHost(deviceNetworkAddress, DEVICE));
}

//!Channel deallocation
/*!
 Perform the message channel deallocation
 */
void MessageBroker::disposeMessageChannel(MessageChannel *messageChannelToDispose) {
    if(!messageChannelToDispose) return;
    
    boost::mutex::scoped_lock lock(mapChannelAcces);
    
    //check if the channel is active
    if(activeChannel.count(messageChannelToDispose->channelID) == 0) return;

    //remove the channel as active
    activeChannel.erase(messageChannelToDispose->channelID);
    
    //deallocate it
    messageChannelToDispose->deinit();
    
    //dispose it
    delete(messageChannelToDispose);
}
    //!Channel deallocation
/*!
 Perform the message channel deallocation
 */
void MessageBroker::disposeMessageChannel(NetworkAddressMessageChannel *messageChannelToDispose) {
    MessageBroker::disposeMessageChannel((MessageChannel*)messageChannelToDispose);
}

