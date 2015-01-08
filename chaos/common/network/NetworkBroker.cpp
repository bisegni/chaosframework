/*
 *	NetworkBroker.cpp
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
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/global.h>
#include <boost/lexical_cast.hpp>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/message/PerformanceNodeChannel.h>
#include <chaos/common/event/EventServer.h>
#include <chaos/common/event/EventClient.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/dispatcher/AbstractEventDispatcher.h>
#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/utility/InetUtility.h>

#define MB_LAPP LAPP_ << "[NetworkBroker]- "

#define INIT_STEP   0
#define DEINIT_STEP 1
using namespace chaos;
using namespace chaos::event;
using namespace chaos::common::data;
using namespace chaos::common::network;

/*!
 
 */
NetworkBroker::NetworkBroker():
performance_session_managment(this),
event_client(NULL),
event_server(NULL),
rpc_server(NULL),
rpc_client(NULL),
sync_rpc_server(NULL),
command_dispatcher(NULL),
direct_io_dispatcher(NULL),
direct_io_server(NULL) {
    can_use_metadata_server = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
    if(can_use_metadata_server){
        metadata_server_address = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    }
}

/*!
 */
NetworkBroker::~NetworkBroker() {
    
}

/*!
 * Initzialize the Message Broker. In this step are taken the configured implementation
 * for the rpc client and server and for the dispatcher. All these are here initialized
 */
void NetworkBroker::init(void *initData) throw(CException) {
    MB_LAPP << "Init phase";
	//get global configuration reference
    CDataWrapper *globalConfiguration = GlobalConfiguration::getInstance()->getConfiguration();
    
    
    if(!globalConfiguration) {
        throw CException(1, "No global configuraiton found", __PRETTY_FUNCTION__);
    }
    
	//---------------------------- D I R E C T I/O ----------------------------
	if(globalConfiguration->hasKey(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE)) {
        string direct_io_impl = globalConfiguration->getStringValue(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE);
		//construct the rpc server and client name
        string direct_io_server_impl = direct_io_impl+"DirectIOServer";
        direct_io_client_impl = direct_io_impl + "DirectIOClient";
        MB_LAPP  << "Trying to initilize DirectIO Server: " << direct_io_server_impl;
        direct_io_server = ObjectFactoryRegister<common::direct_io::DirectIOServer>::getInstance()->getNewInstanceByName(direct_io_server_impl);
		if(!direct_io_server) throw CException(1, "Error creating direct io server implementation", __PRETTY_FUNCTION__);
		
		//allocate the dispatcher
		MB_LAPP  << "Allocate DirectIODispatcher";
		direct_io_server->setHandler(direct_io_dispatcher = new common::direct_io::DirectIODispatcher());
		
		//initialize direct io server
		chaos::utility::StartableService::initImplementation(direct_io_server, static_cast<void*>(globalConfiguration), direct_io_server->getName(), __PRETTY_FUNCTION__);
		
		//init the my_ip variable for all client
		common::direct_io::DirectIOClientConnection::my_str_ip = GlobalConfiguration::getInstance()->getLocalServerAddress();
		common::direct_io::DirectIOClientConnection::my_i64_ip = STRIP_TO_UI64(common::direct_io::DirectIOClientConnection::my_str_ip).to_ulong();
    }
	//---------------------------- D I R E C T I/O ----------------------------
	
	//---------------------------- E V E N T ----------------------------
    if(globalConfiguration->hasKey(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION)) {
        event_dispatcher = ObjectFactoryRegister<AbstractEventDispatcher>::getInstance()->getNewInstanceByName("DefaultEventDispatcher");
        if(!event_dispatcher)
            throw CException(2, "Event dispatcher implementation not found", __PRETTY_FUNCTION__);
        
        if(!chaos::utility::StartableService::initImplementation(event_dispatcher, static_cast<void*>(globalConfiguration), "DefaultEventDispatcher", __PRETTY_FUNCTION__))
            throw CException(3, "Event dispatcher has not been initialized due an error", __PRETTY_FUNCTION__);
        
        
        string event_adapter_type = globalConfiguration->getStringValue(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION);
		//construct the rpc server and client name
        string event_server_name = event_adapter_type+"EventServer";
        string event_client_name = event_adapter_type+"EventClient";
        
        MB_LAPP  << "Trying to initilize Event Server: " << event_server_name;
        event_server = ObjectFactoryRegister<EventServer>::getInstance()->getNewInstanceByName(event_server_name);
        if(chaos::utility::StartableService::initImplementation(event_server, static_cast<void*>(globalConfiguration), event_server->getName(), __PRETTY_FUNCTION__)){
			//register the root handler on event server
            event_server->setEventHanlder(event_dispatcher);
        }
        
        MB_LAPP  << "Trying to initilize Event Client: " << event_client_name;
        event_client = ObjectFactoryRegister<EventClient>::getInstance()->getNewInstanceByName(event_client_name);
        chaos::utility::StartableService::initImplementation(event_client, static_cast<void*>(globalConfiguration), event_client->getName(), __PRETTY_FUNCTION__);
    }
	//---------------------------- E V E N T ----------------------------
    
	//---------------------------- R P C ----------------------------
    if(globalConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE)){
		//get the dispatcher
        MB_LAPP  << "Get DefaultCommandDispatcher implementation";
        command_dispatcher = ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher");
        if(!command_dispatcher)
            throw CException(2, "Command dispatcher implementation not found", __PRETTY_FUNCTION__);
        
        if(!chaos::utility::StartableService::initImplementation(command_dispatcher, static_cast<void*>(globalConfiguration), "DefaultCommandDispatcher", __PRETTY_FUNCTION__))
            throw CException(3, "Command dispatcher has not been initialized due an error", __PRETTY_FUNCTION__);
        
        
		// get the rpc type to instantiate
        string rpc_adapter_type = globalConfiguration->getStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE);
		//construct the rpc server and client name
        string rpc_server_name = rpc_adapter_type+"Server";
        string rpc_client_name = rpc_adapter_type+"Client";
        
        MB_LAPP  << "Trying to initilize RPC Server: " << rpc_server_name;
        rpc_server = ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(rpc_server_name);
		if(!rpc_server) throw CException(4, "Error allocating rpc server implementation", __PRETTY_FUNCTION__);
		
        if(chaos::utility::StartableService::initImplementation(rpc_server, static_cast<void*>(globalConfiguration), rpc_server->getName(), __PRETTY_FUNCTION__)) {
			//set the handler on the rpc server
            rpc_server->setCommandDispatcher(command_dispatcher);
        }
        
        
        MB_LAPP  << "Trying to initilize RPC Client: " << rpc_client_name;
        rpc_client = ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpc_client_name);
		if(!rpc_client) throw CException(4, "Error allocating rpc client implementation", __PRETTY_FUNCTION__);

        if(chaos::utility::StartableService::initImplementation(rpc_client, static_cast<void*>(globalConfiguration), rpc_client->getName(), __PRETTY_FUNCTION__)) {
			//set the forwarder into dispatcher for answere
            command_dispatcher->setRpcForwarder(rpc_client);
        }
    } else {
        throw CException(4, "No RPC Adapter type found in configuration", __PRETTY_FUNCTION__);
    }
	//---------------------------- R P C ----------------------------
    //---------------------------- R P C SYNC ----------------------------
    if(globalConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_SYNC_ENABLE)){
        //get the dispatcher
        MB_LAPP  << "Setup RPC Sync implementation";
        
        // get the rpc type to instantiate
        string rpc_sync_impl = globalConfiguration->getStringValue(RpcConfigurationKey::CS_CMDM_RPC_SYNC_ADAPTER_TYPE);
        //construct the rpc server and client name
        string rpc_sync_impl_name = rpc_sync_impl+"RpcSyncServer";
        
        MB_LAPP  << "Trying to initilize RPC Server: " << rpc_sync_impl_name;
        sync_rpc_server = ObjectFactoryRegister<sync_rpc::RpcSyncServer>::getInstance()->getNewInstanceByName(rpc_sync_impl_name);
        if(!sync_rpc_server) throw CException(4, "Error allocating rpc sync server implementation", __PRETTY_FUNCTION__);
        
        if(chaos::utility::StartableService::initImplementation(sync_rpc_server, static_cast<void*>(globalConfiguration), sync_rpc_server->getName(), __PRETTY_FUNCTION__)) {
            //set the handler on the rpc server
            sync_rpc_server->setCommandDispatcher(command_dispatcher);
        }
    } else {
        throw CException(4, "No RPC Sync Adapter type found in configuration", __PRETTY_FUNCTION__);
    }
    //---------------------------- R P C SYNC ----------------------------
	MB_LAPP  << "Initialize performance session manager";
	chaos::utility::StartableService::initImplementation(performance_session_managment, static_cast<void*>(globalConfiguration), "PerformanceManagment",  __PRETTY_FUNCTION__);
    
    //get host and port for fastly set it into the requests
    published_host_and_port.clear();
    getPublishedHostAndPort(published_host_and_port);
}

/*!
 * All rpc adapter and command siaptcer are deinitilized
 */
void NetworkBroker::deinit() throw(CException) {
	MB_LAPP  << "Deinitialize performance session manager";
	chaos::utility::StartableService::deinitImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);

	//---------------------------- D I R E C T I/O ----------------------------
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(direct_io_server, direct_io_server->getName(), "NetworkBroker::deinit");)
	DELETE_OBJ_POINTER(direct_io_server);
	//---------------------------- D I R E C T I/O ----------------------------
	
	//---------------------------- E V E N T ----------------------------
    MB_LAPP  << "Deallocate all event channel";
    for (map<string, event::channel::EventChannel*>::iterator channnelIter = active_event_channel.begin();
         channnelIter != active_event_channel.end();
         channnelIter++) {
        
        event::channel::EventChannel *eventChannelToDispose = channnelIter->second;
        
		//deinit channel
        eventChannelToDispose->deinit();
        
		//dispose it
        delete(eventChannelToDispose);
    }
    MB_LAPP  << "Clear event channel map";
    active_event_channel.clear();

    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(event_client);
    
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(event_server);
    
    MB_LAPP  << "Deinit Event dispatcher";
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(event_dispatcher);
	//---------------------------- E V E N T ----------------------------
	
    //---------------------------- R P C SYNC ----------------------------
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(sync_rpc_server, sync_rpc_server->getName(), __PRETTY_FUNCTION__);)
    //---------------------------- R P C SYNC ----------------------------

    
	//---------------------------- R P C ----------------------------
    MB_LAPP  << "Deallocate all rpc channel";
    for (map<string, MessageChannel*>::iterator it = active_rpc_channel.begin();
         it != active_rpc_channel.end();
         it++) {
        
        MessageChannel *messageChannelToDispose = it->second;
        
		//deinit channel
        messageChannelToDispose->deinit();
        
		//dispose it
        delete(messageChannelToDispose);
    }
    MB_LAPP  << "Clear rpc channel map";
    active_rpc_channel.clear();
    
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(rpc_client);
    
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);)
	DELETE_OBJ_POINTER(rpc_server);
    
    MB_LAPP  << "Deinit Command Dispatcher";
    CHAOS_NOT_THROW(chaos::utility::StartableService::deinitImplementation(command_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(command_dispatcher);
	//---------------------------- R P C ----------------------------
	
}

/*!
 * all part are started
 */
void NetworkBroker::start() throw(CException){
    chaos::utility::StartableService::startImplementation(direct_io_server, direct_io_server->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(command_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(sync_rpc_server, sync_rpc_server->getName(), __PRETTY_FUNCTION__);
    chaos::utility::StartableService::startImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);
}

/*!
 * all part are started
 */
void NetworkBroker::stop() throw(CException) {
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(sync_rpc_server, sync_rpc_server->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(command_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(chaos::utility::StartableService::stopImplementation(direct_io_server, direct_io_server->getName(), __PRETTY_FUNCTION__);)
}

/*!
 Return the port where the rpc server has been published
 */
int NetworkBroker::getPublishedPort() {
    CHAOS_ASSERT(rpc_server);
    return rpc_server->getPublishedPort();
}

/*!
 Fill the parameter withe rigth value of host and port for the internale
 rpc server of message broker
 */
void NetworkBroker::getPublishedHostAndPort(string& hostAndPort) {
    CHAOS_ASSERT(rpc_server);
    hostAndPort = GlobalConfiguration::getInstance()->getLocalServerAddress();
    hostAndPort.append(":");
    hostAndPort.append(lexical_cast<string>(rpc_server->getPublishedPort()));
}

std::string NetworkBroker::getRPCUrl() {
	std::string rpc_endpoint;
	getPublishedHostAndPort(rpc_endpoint);
	return rpc_endpoint;
}

std::string NetworkBroker::getDirectIOUrl() {
    CHAOS_ASSERT(rpc_server);
	return direct_io_server->getUrl();
}

//!Return the sync rpc url
std::string NetworkBroker::getSyncRPCUrl() {
	if(sync_rpc_server) {
		return sync_rpc_server->getUrl();
	} else {
		return string("Sync rpc not started");
	}
}

#pragma mark Event Registration and forwarding
//! event Action registration for the current instance of NetworkBroker
/*!
 Register an event actions defined for a detgerminated event type
 \param eventAction the actio to register
 \param eventType a type for the event for which the user want to register
 */
void NetworkBroker::registerEventAction(EventAction *eventAction, event::EventType eventType, const char * const identification) {
    CHAOS_ASSERT(event_dispatcher && eventAction);
    event_dispatcher->registerEventAction(eventAction, eventType, identification);
}

//!Event Action deregistration
/*!
 Deregister an event action
 */
void NetworkBroker::deregisterEventAction(EventAction *eventAction) {
    CHAOS_ASSERT(event_dispatcher && eventAction);
    event_dispatcher->deregisterEventAction(eventAction);
}

//!Event channel creation
/*!
 Performe the creation of an event channel of a desidered type
 \param eventType is one of the value listent in EventType enum that specify the
 type of the eventfor wich we want a channel
 */
event::channel::EventChannel *NetworkBroker::getNewEventChannelFromType(event::EventType  event_type) {
    event::channel::EventChannel *new_event_channel = NULL;
    switch (event_type) {
        case event::EventTypeAlert:
            new_event_channel = new event::channel::AlertEventChannel(this);
            break;
        case event::EventTypeInstrument:
            new_event_channel = new event::channel::InstrumentEventChannel(this);
            break;
        default:
            break;
    }
	//check if the channel has been created
    if(new_event_channel){
        new_event_channel->init();
        boost::mutex::scoped_lock lock(muext_map_event_channel_access);
        active_event_channel.insert(make_pair(new_event_channel->channelID, new_event_channel));
    }
    
    return new_event_channel;
}
//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
event::channel::AlertEventChannel *NetworkBroker::getNewAlertEventChannel() {
    return static_cast<event::channel::AlertEventChannel*>(NetworkBroker::getNewEventChannelFromType(event::EventTypeAlert));
}

//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
event::channel::InstrumentEventChannel *NetworkBroker::getNewInstrumentEventChannel() {
    return static_cast<event::channel::InstrumentEventChannel*>(NetworkBroker::getNewEventChannelFromType(event::EventTypeInstrument));
}

//!Event channel deallocation
/*!
 Perform the event channel deallocation
 */
void NetworkBroker::disposeEventChannel(event::channel::EventChannel *event_channel_to_dispose) {
    if(!event_channel_to_dispose) return;
    
    boost::mutex::scoped_lock lock(muext_map_event_channel_access);
    
	//check if the channel is active
    if(active_event_channel.count(event_channel_to_dispose->channelID) == 0) return;
    
	//remove the channel as active
    active_event_channel.erase(event_channel_to_dispose->channelID);
    
	//deallocate it
    event_channel_to_dispose->deinit();
    
	//dispose it
    delete(event_channel_to_dispose);
}

//!message event
/*!
 Submit an event
 \param event the new evento to submit
 */
bool NetworkBroker::submitEvent(event::EventDescriptor *event) {
    CHAOS_ASSERT(event_client)
    bool result = true;
    try{
        event_client->submitEvent(event);
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
void NetworkBroker::registerAction(DeclareAction* declare_action_class) {
    CHAOS_ASSERT(declare_action_class)
    command_dispatcher->registerAction(declare_action_class);
}

/*
 Deregister actions for a determianted domain
 */
void NetworkBroker::deregisterAction(DeclareAction* declare_action_class) {
    CHAOS_ASSERT(declare_action_class)
    command_dispatcher->deregisterAction(declare_action_class);
}

#pragma mark Message Submission

/*!
 Submit a message specifing the destination
 */
bool NetworkBroker::submitMessage(string& host,
                                  CDataWrapper *message,
                                  NetworkErrorHandler handler,
                                  const char * sender_identifier,
                                  int64_t sendet_tag,
                                  bool on_this_thread) {
    CHAOS_ASSERT(message && rpc_client)
    NetworkForwardInfo *nfi = new NetworkForwardInfo();
    nfi->destinationAddr = host;
    nfi->message = message;
	//add answer id to datawrapper
    return rpc_client->submitMessage(nfi, on_this_thread);
}

/*!
 Submite a new request to send to the remote host
 */
bool NetworkBroker::submiteRequest(string& host,
                                   CDataWrapper *request,
                                   NetworkErrorHandler handler,
                                   const char * sender_identifier,
                                   int64_t sender_tag,
                                   bool on_this_thread) {
    CHAOS_ASSERT(request && rpc_client)
    request->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP, published_host_and_port);
    NetworkForwardInfo *nfi = new NetworkForwardInfo();
    nfi->destinationAddr = host;
    nfi->message = request;
    return rpc_client->submitMessage(nfi, on_this_thread);
}

/*
 */
MessageChannel *NetworkBroker::getNewMessageChannelForRemoteHost(CNetworkAddress *node_network_aAddress,
                                                                 EntityType type) {
    CHAOS_ASSERT(node_network_aAddress)
    MessageChannel *channel = NULL;
    switch (type) {
        case RAW:
            channel = new MessageChannel(this, node_network_aAddress->ipPort);
            delete(node_network_aAddress);
            break;
            
        case MDS:
            channel = new MDSMessageChannel(this, static_cast<CNodeNetworkAddress*>(node_network_aAddress));
            break;
            
        case DEVICE:
            channel = new DeviceMessageChannel(this, static_cast<CDeviceNetworkAddress*>(node_network_aAddress));
            break;
		case PERFORMANCE:
			channel = new common::message::PerformanceNodeChannel(this, node_network_aAddress, performance_session_managment.getLocalDirectIOClientInstance());
            break;
    }
	//check if the channel has been created
    if(channel){
        channel->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(channel->channelID, channel));
    }
    return channel;
}

//!Metadata server channel creation
/*!
 Performe the creation of metadata server
 */
MDSMessageChannel *NetworkBroker::getMetadataserverMessageChannel() {
    CNodeNetworkAddress *mdsNodeAddr = new CNodeNetworkAddress();
    mdsNodeAddr->ipPort = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    mdsNodeAddr->nodeID = ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN;
    return static_cast<MDSMessageChannel*>(getNewMessageChannelForRemoteHost(mdsNodeAddr, MDS));
}

//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
DeviceMessageChannel *NetworkBroker::getDeviceMessageChannelFromAddress(CDeviceNetworkAddress *deviceNetworkAddress) {
    return static_cast<DeviceMessageChannel*>(getNewMessageChannelForRemoteHost(deviceNetworkAddress, DEVICE));
}

//!performance channel creation
chaos::common::message::PerformanceNodeChannel *NetworkBroker::getPerformanceChannelFromAddress(CNetworkAddress  *node_network_address) {
	return static_cast<chaos::common::message::PerformanceNodeChannel*>(getNewMessageChannelForRemoteHost(node_network_address, PERFORMANCE));
}

//!Channel deallocation
/*!
 Perform the message channel deallocation
 */
void NetworkBroker::disposeMessageChannel(MessageChannel *message_channel_to_dispose) {
    if(!message_channel_to_dispose) return;
    
    boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
    
	//check if the channel is active
    if(active_rpc_channel.count(message_channel_to_dispose->channelID) == 0) return;
    
	//remove the channel as active
    active_rpc_channel.erase(message_channel_to_dispose->channelID);
    
	//deallocate it
    message_channel_to_dispose->deinit();
    
	//dispose it
    delete(message_channel_to_dispose);
}
//!Channel deallocation
/*!
 Perform the message channel deallocation
 */
void NetworkBroker::disposeMessageChannel(NodeMessageChannel *messageChannelToDispose) {
    NetworkBroker::disposeMessageChannel((MessageChannel*)messageChannelToDispose);
}
//Allocate a new endpoint in the direct io server
chaos_direct_io::DirectIOServerEndpoint *NetworkBroker::getDirectIOServerEndpoint() {
    CHAOS_ASSERT(direct_io_dispatcher)
    chaos_direct_io::DirectIOServerEndpoint *result_endpoint = direct_io_dispatcher->getNewEndpoint();
    return result_endpoint;
}
//Dispose an endpoint of the direct io server
void NetworkBroker::releaseDirectIOServerEndpoint(chaos_direct_io::DirectIOServerEndpoint *end_point) {
    direct_io_dispatcher->releaseEndpoint(end_point);
}
//Return a new direct io client instance
chaos_direct_io::DirectIOClient *NetworkBroker::getDirectIOClientInstance() {
    MB_LAPP  << "Allocate a new DirectIOClient of type " << direct_io_client_impl;
    return ObjectFactoryRegister<common::direct_io::DirectIOClient>::getInstance()->getNewInstanceByName(direct_io_client_impl);
}
