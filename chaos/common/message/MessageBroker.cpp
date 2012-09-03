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
#include <chaos/common/event/EventServer.h>
#include <chaos/common/event/EventClient.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/dispatcher/AbstractEventDispatcher.h>
#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#define MB_LAPP LAPP_ << "[MessageBroker]- "

#define INIT_STEP   0
#define DEINIT_STEP 1

using namespace chaos;
using namespace chaos::event;

/*!
 
 */
MessageBroker::MessageBroker(){
    eventClient = NULL;
    eventServer = NULL;
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
    
    
    
    MB_LAPP << "Init pahse";
        //get global configuration reference
    CDataWrapper *globalConfiguration = GlobalConfiguration::getInstance()->getConfiguration();
    
    
    if(!globalConfiguration) {
        throw CException(1, "No global configuraiton found", "MessageBroker::init");
    }
    
        //---------------------------- E V E N T ----------------------------
    if(globalConfiguration->hasKey(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION)) {
        eventDispatcher = ObjectFactoryRegister<AbstractEventDispatcher>::getInstance()->getNewInstanceByName("DefaultEventDispatcher");
        if(!eventDispatcher)
            throw CException(2, "Event dispatcher implementation not found", "MessageBroker::init");
        
        if(!utility::ISDInterface::initImplementation(eventDispatcher, globalConfiguration, "DefaultEventDispatcher", "MessageBroker::init"))
            throw CException(3, "Event dispatcher has not been initialized due an error", "MessageBroker::init");
        
        
        string eventAdapterType = globalConfiguration->getStringValue(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION);
            //construct the rpc server and client name
        string eventServerName = eventAdapterType+"EventServer";
        string eventClientName = eventAdapterType+"EventClient";
        
        MB_LAPP  << "Trying to initilize Event Server: " << eventServerName;
        eventServer = ObjectFactoryRegister<EventServer>::getInstance()->getNewInstanceByName(eventServerName.c_str());
        if(utility::ISDInterface::initImplementation(eventServer, globalConfiguration, eventServer->getName(), "MessageBroker::init")){
                //register the root handler on event server
            eventServer->setEventHanlder(eventDispatcher);
        }
        
        
        MB_LAPP  << "Trying to initilize Event Client: " << eventClientName;
        eventClient = ObjectFactoryRegister<EventClient>::getInstance()->getNewInstanceByName(eventClientName.c_str());
        utility::ISDInterface::initImplementation(eventClient, globalConfiguration, eventClientName.c_str(), "MessageBroker::init");
    }
        //---------------------------- E V E N T ----------------------------
    
        //---------------------------- R P C ----------------------------
    if(globalConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE)){
            //get the dispatcher
        MB_LAPP  << "Get DefaultCommandDispatcher implementation";
        commandDispatcher = ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher");
        if(!commandDispatcher)
            throw CException(2, "Command dispatcher implementation not found", "MessageBroker::init");
        
        if(!utility::ISDInterface::initImplementation(commandDispatcher, globalConfiguration, "DefaultCommandDispatcher", "MessageBroker::init"))
            throw CException(3, "Command dispatcher has not been initialized due an error", "MessageBroker::init");
        
        
            // get the rpc type to instantiate
        string rpcRapterType = globalConfiguration->getStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE);
            //construct the rpc server and client name
        string rpcServerName = rpcRapterType+"Server";
        string rpcClientName = rpcRapterType+"Client";
        
        MB_LAPP  << "Trying to initilize RPC Server: " << rpcServerName;
        rpcServer = ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(rpcServerName.c_str());
        if(utility::ISDInterface::initImplementation(rpcServer, globalConfiguration, rpcServer->getName(), "MessageBroker::init")) {
                //set the handler on the rpc server
            rpcServer->setCommandDispatcher(commandDispatcher);
        }
        
        
        MB_LAPP  << "Trying to initilize RPC Client: " << rpcClientName;
        rpcClient = ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpcClientName.c_str());
        if(utility::ISDInterface::initImplementation(rpcClient, globalConfiguration, rpcClient->getName(), "MessageBroker::init")) {
                //set the forwarder into dispatcher for answere
            if(commandDispatcher) commandDispatcher->setRpcForwarder(rpcClient);
        }
    } else {
        throw CException(4, "No RPC Adapter type found in configuration", "MessageBroker::init");
    }
        //---------------------------- R P C ----------------------------
    MB_LAPP  << "Message Broker Initialized";
}

/*!
 * All rpc adapter and command siaptcer are deinitilized
 */
void MessageBroker::deinit() throw(CException) {
    
        //lock esclusive access to init phase
    SetupStateManager::levelDownFrom(DEINIT_STEP, "MessageBroker already deinitialized");
    
    MB_LAPP  << "Deinitilizing Message Broker";
    
        //---------------------------- E V E N T ----------------------------  
    MB_LAPP  << "Deallocate all event channel";
    for (map<string, event::channel::EventChannel*>::iterator channnelIter = activeEventChannel.begin();
         channnelIter != activeEventChannel.end();
         channnelIter++) {
        
        event::channel::EventChannel *eventChannelToDispose = channnelIter->second;
        
            //deinit channel
        eventChannelToDispose->deinit();
        
            //dispose it
        delete(eventChannelToDispose);
    }
    MB_LAPP  << "Clear event channel map";
    activeEventChannel.clear();
    
    MB_LAPP  << "Deinit event client: " << eventClient->getName();
    utility::ISDInterface::deinitImplementation(eventClient, eventClient->getName(), "MessageBroker::deinit");
    
    MB_LAPP  << "Deinit event server: " << eventServer->getName();
    utility::ISDInterface::deinitImplementation(eventServer, eventServer->getName(), "MessageBroker::deinit");
    
    MB_LAPP  << "Deinit Event dispatcher";
    utility::ISDInterface::deinitImplementation(eventDispatcher, "DefaultEventDispatcher", "MessageBroker::deinit");
        //---------------------------- E V E N T ----------------------------

    
        //---------------------------- R P C ----------------------------
    MB_LAPP  << "Deallocate all rpc channel";
    for (map<string, MessageChannel*>::iterator channnelIter = activeRpcChannel.begin();
         channnelIter != activeRpcChannel.end();
         channnelIter++) {
        
        MessageChannel *messageChannelToDispose = channnelIter->second;
        
            //deinit channel
        messageChannelToDispose->deinit();
        
            //dispose it
        delete(messageChannelToDispose);
    }
    MB_LAPP  << "Clear rpc channel map";
    activeRpcChannel.clear();
    
    MB_LAPP  << "Deinit rpc client: " << rpcClient->getName();
    utility::ISDInterface::deinitImplementation(rpcClient, rpcClient->getName(), "MessageBroker::deinit");
    
    MB_LAPP  << "Deinit rpc server: " << rpcServer->getName();
    utility::ISDInterface::deinitImplementation(rpcServer, rpcServer->getName(), "MessageBroker::deinit");

    MB_LAPP  << "Deinit Command Dispatcher";
    utility::ISDInterface::deinitImplementation(commandDispatcher, "DefaultCommandDispatcher", "MessageBroker::deinit");
        //---------------------------- R P C ----------------------------

}

/*!
 * all part are started
 */
void MessageBroker::start() throw(CException){
    MB_LAPP  << "Start event dispathcer ";
    utility::ISDInterface::startImplementation(eventDispatcher, "DefaultEventDispatcher", "MessageBroker::start");
    
    MB_LAPP  << "Start event server: " << eventServer->getName();
    utility::ISDInterface::startImplementation(eventServer, eventServer->getName(), "MessageBroker::start");
    
    MB_LAPP  << "Start event client: " << eventClient->getName();
    utility::ISDInterface::startImplementation(eventClient, eventClient->getName(), "MessageBroker::start");
    
    MB_LAPP  << "Start command dispathcer ";
    utility::ISDInterface::startImplementation(commandDispatcher, "DefaultCommandDispatcher", "MessageBroker::start");

    MB_LAPP  << "Start rpc server: " << rpcServer->getName();
    utility::ISDInterface::startImplementation(rpcServer, rpcServer->getName(), "MessageBroker::start");
    
    MB_LAPP  << "Start rpc server: " << rpcClient->getName();
    utility::ISDInterface::startImplementation(rpcClient, rpcClient->getName(), "MessageBroker::start");
    
    MB_LAPP << "get the published host and port from rpc server";
    getPublishedHostAndPort(publishedHostAndPort);
    MB_LAPP << "Rpc server has been published in: " << publishedHostAndPort;
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

#pragma mark Event Registration and forwarding
    //! event Action registration for the current instance of MessageBroker
/*!
 Register an event actions defined for a detgerminated event type
 \param eventAction the actio to register
 \param eventType a type for the event for which the user want to register
 */
void MessageBroker::registerEventAction(EventAction *eventAction, event::EventType eventType, const char * const identification) {
    CHAOS_ASSERT(eventDispatcher && eventAction);
    eventDispatcher->registerEventAction(eventAction, eventType, identification);
}

    //!Event Action deregistration
/*!
 Deregister an event action
 */
void MessageBroker::deregisterEventAction(EventAction *eventAction) {
    CHAOS_ASSERT(eventDispatcher && eventAction);
    eventDispatcher->deregisterEventAction(eventAction);
}

    //!Event channel creation
/*!
 Performe the creation of an event channel of a desidered type
 \param eventType is one of the value listent in EventType enum that specify the
 type of the eventfor wich we want a channel
 */
event::channel::EventChannel *MessageBroker::getNewEventChannelFromType(event::EventType  eventType) {
    event::channel::EventChannel *newEventChannel = NULL;
    switch (eventType) {
        case event::EventTypeAlert:
            newEventChannel = new event::channel::AlertEventChannel(this);
            break;
        case event::EventTypeInstrument:
            newEventChannel = new event::channel::InstrumentEventChannel(this);
            break;
        default:
            break;
    }
        //check if the channel has been created
    if(newEventChannel){
        newEventChannel->init();
        boost::mutex::scoped_lock lock(mapEventChannelAccess);
        activeEventChannel.insert(make_pair(newEventChannel->channelID, newEventChannel));
    }
    
    return newEventChannel;
}
    //!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
event::channel::AlertEventChannel *MessageBroker::getNewAlertEventChannel() {
    return static_cast<event::channel::AlertEventChannel*>(MessageBroker::getNewEventChannelFromType(event::EventTypeAlert));
}


    //!Event channel deallocation
/*!
 Perform the event channel deallocation
 */
void MessageBroker::disposeEventChannel(event::channel::EventChannel *eventChannelToDispose) {
    if(!eventChannelToDispose) return;
    
    boost::mutex::scoped_lock lock(mapEventChannelAccess);
    
        //check if the channel is active
    if(activeEventChannel.count(eventChannelToDispose->channelID) == 0) return;
    
        //remove the channel as active
    activeEventChannel.erase(eventChannelToDispose->channelID);
    
        //deallocate it
    eventChannelToDispose->deinit();
    
        //dispose it
    delete(eventChannelToDispose);
}

    //!message event
/*!
 Submit an event
 \param event the new evento to submit
 */
bool MessageBroker::submitEvent(event::EventDescriptor *event) {
    CHAOS_ASSERT(eventClient)
    bool result = true;
    try{
        eventClient->submitEvent(event);
    } catch(CException& ex) {
        result = false;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
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
        boost::mutex::scoped_lock lock(mapRpcChannelAcces);
        activeRpcChannel.insert(make_pair(channel->channelID, channel));
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
    
    boost::mutex::scoped_lock lock(mapRpcChannelAcces);
    
        //check if the channel is active
    if(activeRpcChannel.count(messageChannelToDispose->channelID) == 0) return;
    
        //remove the channel as active
    activeRpcChannel.erase(messageChannelToDispose->channelID);
    
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

