/*
 *	NetworkBroker.cpp
 *	!CHAOS
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
#include <chaos/common/global.h>
#include <boost/lexical_cast.hpp>
#include <chaos/common/event/event.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/message/PerformanceNodeChannel.h>
#include <chaos/common/dispatcher/AbstractEventDispatcher.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
//-----------for metric collection---------
#include <chaos/common/rpc/RpcClientMetricCollector.h>
#include <chaos/common/rpc/RpcServerMetricCollector.h>
#include <chaos/common/direct_io/DirectIODispatcherMetricCollector.h>

#define MB_LAPP LAPP_ << "[NetworkBroker]- "

#define INIT_STEP   0
#define DEINIT_STEP 1
using namespace chaos;
using namespace chaos::common::event;
using namespace chaos::common::event::channel;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::message;
/*!
 
 */
NetworkBroker::NetworkBroker():
performance_session_managment(this),
event_client(NULL),
event_server(NULL),
event_dispatcher(NULL),
rpc_server(NULL),
rpc_client(NULL),
rpc_dispatcher(NULL),
direct_io_dispatcher(NULL),
direct_io_server(NULL),
direct_io_client(NULL),
global_request_domain(){}

/*!
 */
NetworkBroker::~NetworkBroker() {
    CHAOS_NOT_THROW(stop(););
    CHAOS_NOT_THROW(deinit(););
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
        throw CException(-1, "No global configuraiton found", __PRETTY_FUNCTION__);
    }
    
    //---------------------------- D I R E C T I/O ----------------------------
    if(globalConfiguration->hasKey(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE)) {
        MB_LAPP  << "Setup DirectIO sublayer";
        string direct_io_impl = globalConfiguration->getStringValue(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE);
        //construct the rpc server and client name
        string direct_io_server_impl = direct_io_impl+"DirectIOServer";
        direct_io_client_impl = direct_io_impl + "DirectIOClient";
        MB_LAPP  << "Trying to initilize DirectIO Server: " << direct_io_server_impl;
        direct_io_server = ObjectFactoryRegister<common::direct_io::DirectIOServer>::getInstance()->getNewInstanceByName(direct_io_server_impl);
        if(!direct_io_server) throw CException(-2, "Error creating direct io server implementation", __PRETTY_FUNCTION__);
        
        //allocate the dispatcher
        MB_LAPP  << "Allocate DirectIODispatcher";
        if(globalConfiguration->hasKey(InitOption::OPT_DIRECT_IO_LOG_METRIC) &&
           globalConfiguration->getBoolValue(InitOption::OPT_DIRECT_IO_LOG_METRIC)) {
            //the metric allocator of direct io is a direct subclass of DirectIODispatcher
            direct_io_dispatcher = new direct_io::DirectIODispatcherMetricCollector(direct_io_server->getName());
        } else {
            direct_io_dispatcher = new common::direct_io::DirectIODispatcher();
        }
        direct_io_server->setHandler(direct_io_dispatcher);
        
        
        //initialize direct io server
        StartableService::initImplementation(direct_io_server, static_cast<void*>(globalConfiguration), direct_io_server->getName(), __PRETTY_FUNCTION__);
        
        //init the my_ip variable for all client
        common::direct_io::DirectIOClientConnection::my_str_ip = GlobalConfiguration::getInstance()->getLocalServerAddress();
        common::direct_io::DirectIOClientConnection::my_i64_ip = STRIP_TO_UI64(common::direct_io::DirectIOClientConnection::my_str_ip).to_ulong();
        
        direct_io_client = ObjectFactoryRegister<common::direct_io::DirectIOClient>::getInstance()->getNewInstanceByName(direct_io_client_impl);
        if(!direct_io_client) throw CException(-3, "Error creating direct io client implementation", __PRETTY_FUNCTION__);
        
        //initialize direct io client
        InizializableService::initImplementation(direct_io_client, static_cast<void*>(globalConfiguration), direct_io_client->getName(), __PRETTY_FUNCTION__);
        
    }
    
    //---------------------------- D I R E C T I/O ----------------------------
    
    //---------------------------- E V E N T ----------------------------
    if(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE)) {
        MB_LAPP  << "Setup Event sublayer";
        event_dispatcher = ObjectFactoryRegister<AbstractEventDispatcher>::getInstance()->getNewInstanceByName("DefaultEventDispatcher");
        if(!event_dispatcher)
            throw CException(-4, "Event dispatcher implementation not found", __PRETTY_FUNCTION__);
        
        if(!StartableService::initImplementation(event_dispatcher, static_cast<void*>(globalConfiguration), "DefaultEventDispatcher", __PRETTY_FUNCTION__))
            throw CException(-5, "Event dispatcher has not been initialized due an error", __PRETTY_FUNCTION__);
        
        
        string event_adapter_type = globalConfiguration->getStringValue(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION);
        //construct the rpc server and client name
        string event_server_name = event_adapter_type+"EventServer";
        string event_client_name = event_adapter_type+"EventClient";
        
        MB_LAPP  << "Trying to initilize Event Server: " << event_server_name;
        event_server = ObjectFactoryRegister<EventServer>::getInstance()->getNewInstanceByName(event_server_name);
        if(StartableService::initImplementation(event_server, static_cast<void*>(globalConfiguration), event_server->getName(), __PRETTY_FUNCTION__)){
            //register the root handler on event server
            event_server->setEventHanlder(event_dispatcher);
        }
        
        MB_LAPP  << "Trying to initilize Event Client: " << event_client_name;
        event_client = ObjectFactoryRegister<EventClient>::getInstance()->getNewInstanceByName(event_client_name);
        StartableService::initImplementation(event_client, static_cast<void*>(globalConfiguration), event_client->getName(), __PRETTY_FUNCTION__);
    }
    //---------------------------- E V E N T ----------------------------
    
    //---------------------------- R P C ----------------------------
    if(globalConfiguration->hasKey(InitOption::OPT_RPC_IMPLEMENTATION)){
        //get the dispatcher
        MB_LAPP  << "Setup RPC sublayer";
        rpc_dispatcher = ObjectFactoryRegister<AbstractCommandDispatcher>::getInstance()->getNewInstanceByName("DefaultCommandDispatcher");
        if(!rpc_dispatcher)
            throw CException(-6, "Command dispatcher implementation not found", __PRETTY_FUNCTION__);
        
        if(!StartableService::initImplementation(rpc_dispatcher, static_cast<void*>(globalConfiguration), "DefaultCommandDispatcher", __PRETTY_FUNCTION__))
            throw CException(-7, "Command dispatcher has not been initialized due an error", __PRETTY_FUNCTION__);
        
        
        // get the rpc type to instantiate
        string rpc_impl = globalConfiguration->getStringValue(InitOption::OPT_RPC_IMPLEMENTATION);
        //construct the rpc server and client name
        string rpc_server_name = rpc_impl+"Server";
        string rpc_client_name = rpc_impl+"Client";
        
        MB_LAPP  << "Trying to initilize RPC Server: " << rpc_server_name;
        rpc_server = ObjectFactoryRegister<RpcServer>::getInstance()->getNewInstanceByName(rpc_server_name);
        if(!rpc_server) throw CException(-8, "Error allocating rpc server implementation", __PRETTY_FUNCTION__);
        if(globalConfiguration->getBoolValue(InitOption::OPT_RPC_LOG_METRIC)) {
            rpc_server = new rpc::RpcServerMetricCollector(rpc_server->getName(), rpc_server);
        }
        
        if(StartableService::initImplementation(rpc_server, static_cast<void*>(globalConfiguration), rpc_server->getName(), __PRETTY_FUNCTION__)) {
            //set the handler on the rpc server
            rpc_server->setCommandDispatcher(rpc_dispatcher);
        }
        
        
        MB_LAPP  << "Trying to initilize RPC Client: " << rpc_client_name;
        rpc_client = ObjectFactoryRegister<RpcClient>::getInstance()->getNewInstanceByName(rpc_client_name);
        if(!rpc_client) throw CException(-9, "Error allocating rpc client implementation", __PRETTY_FUNCTION__);
        if(globalConfiguration->getBoolValue(InitOption::OPT_RPC_LOG_METRIC)) {
            rpc_client = new rpc::RpcClientMetricCollector(rpc_client->getName(), rpc_client);
        }
        
        //! connect dispatch to manage error durign request forwarding
        rpc_client->setServerHandler(rpc_dispatcher);
        
        if(StartableService::initImplementation(rpc_client, static_cast<void*>(globalConfiguration), rpc_client->getName(), __PRETTY_FUNCTION__)) {
            //set the forwarder into dispatcher for answere
            rpc_dispatcher->setRpcForwarder(rpc_client);
        }
    } else {
        throw CException(-10, "No RPC Adapter type found in configuration", __PRETTY_FUNCTION__);
    }
    MB_LAPP  << "Initialize performance session manager";
    StartableService::initImplementation(performance_session_managment, static_cast<void*>(globalConfiguration), "PerformanceManagment",  __PRETTY_FUNCTION__);
    
    //get host and port for fastly set it into the requests
    published_host_and_port.clear();
    getPublishedHostAndPort(published_host_and_port);
    
    //create the shared message request domain
    global_request_domain.reset(new MessageRequestDomain());
}

/*!
 * All rpc adapter and command siaptcer are deinitilized
 */
void NetworkBroker::deinit() throw(CException) {
    //delete the shared mds channel instance
    
    MB_LAPP  << "Deinitialize performance session manager";
    CHAOS_NOT_THROW(StartableService::deinitImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);)
    
    //---------------------------- D I R E C T I/O ----------------------------
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(direct_io_client, direct_io_client->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(direct_io_client);
    
    CHAOS_NOT_THROW(StartableService::deinitImplementation(direct_io_server, direct_io_server->getName(), "NetworkBroker::deinit");)
    
    DELETE_OBJ_POINTER(direct_io_server);
    //---------------------------- D I R E C T I/O ----------------------------
    
    //---------------------------- E V E N T ----------------------------
    if(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE)) {
        MB_LAPP  << "Deallocate all event channel";
        for (map<string, event::channel::EventChannel*>::iterator channnelIter = active_event_channel.begin();
             channnelIter != active_event_channel.end();
             channnelIter++) {
            
            event::channel::EventChannel *eventChannelToDispose = channnelIter->second;

	    if(eventChannelToDispose){
            
	      //deinit channel
	      eventChannelToDispose->deinit();
	      
	      //dispose it
	      delete(eventChannelToDispose);
	    }
        }
        MB_LAPP  << "Clear event channel map";
        active_event_channel.clear();
        
        CHAOS_NOT_THROW(StartableService::deinitImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);)
        DELETE_OBJ_POINTER(event_client);
        
        CHAOS_NOT_THROW(StartableService::deinitImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);)
        DELETE_OBJ_POINTER(event_server);
        
        MB_LAPP  << "Deinit Event dispatcher";
        CHAOS_NOT_THROW(StartableService::deinitImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);)
        DELETE_OBJ_POINTER(event_dispatcher);
    }
    //---------------------------- E V E N T ----------------------------
    
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
    
    CHAOS_NOT_THROW(StartableService::deinitImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(rpc_client);
    
    CHAOS_NOT_THROW(StartableService::deinitImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(rpc_server);
    
    MB_LAPP  << "Deinit Command Dispatcher";
    CHAOS_NOT_THROW(StartableService::deinitImplementation(rpc_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);)
    DELETE_OBJ_POINTER(rpc_dispatcher);
    //---------------------------- R P C ----------------------------
    
}

/*!
 * all part are started
 */
void NetworkBroker::start() throw(CException){
    StartableService::startImplementation(direct_io_server, direct_io_server->getName(), __PRETTY_FUNCTION__);
    if(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE)) {
        StartableService::startImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);
        StartableService::startImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);
        StartableService::startImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);
    }
    StartableService::startImplementation(rpc_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);
    StartableService::startImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);
    StartableService::startImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);
    StartableService::startImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);
}

/*!
 * all part are started
 */
void NetworkBroker::stop() throw(CException) {
    CHAOS_NOT_THROW(StartableService::stopImplementation(performance_session_managment, "PerformanceManagment",  __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(StartableService::stopImplementation(rpc_client, rpc_client->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(StartableService::stopImplementation(rpc_server, rpc_server->getName(), __PRETTY_FUNCTION__);)
    CHAOS_NOT_THROW(StartableService::stopImplementation(rpc_dispatcher, "DefaultCommandDispatcher", __PRETTY_FUNCTION__);)
    if(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE)) {
        CHAOS_NOT_THROW(StartableService::stopImplementation(event_client, event_client->getName(), __PRETTY_FUNCTION__);)
        CHAOS_NOT_THROW(StartableService::stopImplementation(event_server, event_server->getName(), __PRETTY_FUNCTION__);)
        CHAOS_NOT_THROW(StartableService::stopImplementation(event_dispatcher, "DefaultEventDispatcher", __PRETTY_FUNCTION__);)
    }
    CHAOS_NOT_THROW(StartableService::stopImplementation(direct_io_server, direct_io_server->getName(), __PRETTY_FUNCTION__);)
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

#pragma mark Event Registration and forwarding
//! event Action registration for the current instance of NetworkBroker
/*!
 Register an event actions defined for a detgerminated event type
 \param eventAction the actio to register
 \param eventType a type for the event for which the user want to register
 */
void NetworkBroker::registerEventAction(EventAction *eventAction, event::EventType eventType, const char * const identification) {
    CHAOS_ASSERT(event_dispatcher && eventAction && !GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
    event_dispatcher->registerEventAction(eventAction, eventType, identification);
}

//!Event Action deregistration
/*!
 Deregister an event action
 */
void NetworkBroker::deregisterEventAction(EventAction *eventAction) {
    CHAOS_ASSERT(event_dispatcher && eventAction && !GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
    event_dispatcher->deregisterEventAction(eventAction);
}

//!Event channel creation
/*!
 Performe the creation of an event channel of a desidered type
 \param eventType is one of the value listent in EventType enum that specify the
 type of the eventfor wich we want a channel
 */
channel::EventChannel *NetworkBroker::getNewEventChannelFromType(event::EventType  event_type) {
    CHAOS_ASSERT(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
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
AlertEventChannel *NetworkBroker::getNewAlertEventChannel() {
  
  if(GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE)){
    MB_LAPP<<"No Event Alert Instanced, EVENTS are DISABLED";
    return NULL;
  }
  
    return static_cast<event::channel::AlertEventChannel*>(NetworkBroker::getNewEventChannelFromType(event::EventTypeAlert));
}

//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
InstrumentEventChannel *NetworkBroker::getNewInstrumentEventChannel() {
    CHAOS_ASSERT(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
    return static_cast<event::channel::InstrumentEventChannel*>(NetworkBroker::getNewEventChannelFromType(event::EventTypeInstrument));
}

//!Event channel deallocation
/*!
 Perform the event channel deallocation
 */
void NetworkBroker::disposeEventChannel(event::channel::EventChannel *event_channel_to_dispose) {
    CHAOS_ASSERT(!GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
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
    CHAOS_ASSERT(event_client && !GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_EVENT_DISABLE));
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
    rpc_dispatcher->registerAction(declare_action_class);
}

/*
 Deregister actions for a determianted domain
 */
void NetworkBroker::deregisterAction(DeclareAction* declare_action_class) {
    CHAOS_ASSERT(declare_action_class)
    rpc_dispatcher->deregisterAction(declare_action_class);
}

#pragma mark Message Submission

/*!
 Submit a message specifing the destination
 */
bool NetworkBroker::submitMessage(const string& host,
                                  CDataWrapper *message,
                                  bool on_this_thread) {
    CHAOS_ASSERT(message && rpc_client)
    NetworkForwardInfo *nfi = new NetworkForwardInfo(false);
    nfi->destinationAddr = host;
    nfi->setMessage(message);
    //add answer id to datawrapper
    return rpc_client->submitMessage(nfi, on_this_thread);
}

//!send interparocess message
/*!
 forward the message directly to the dispatcher for broadcasting it
 to the registered rpc domain
 */
chaos::common::data::CDataWrapper *NetworkBroker::submitInterProcessMessage(chaos::common::data::CDataWrapper *message,
                                                                            bool onThisThread) {
    CHAOS_ASSERT(rpc_dispatcher)
    if(onThisThread) {
        return rpc_dispatcher->executeCommandSync(message);
    }else{
        return rpc_dispatcher->dispatchCommand(message);
    }
}

/*!
 Submite a new request to send to the remote host
 */
bool NetworkBroker::submiteRequest(const string& host,
                                   CDataWrapper *request,
                                   std::string sender_node_id,
                                   uint32_t sender_request_id,
                                   bool on_this_thread) {
    CHAOS_ASSERT(request && rpc_client)
    request->addStringValue(RpcActionDefinitionKey::CS_CMDM_ANSWER_HOST_IP, published_host_and_port);
    NetworkForwardInfo *nfi = new NetworkForwardInfo(true);
    nfi->destinationAddr = host;
    nfi->sender_node_id = sender_node_id;
    nfi->sender_request_id = sender_request_id;
    nfi->setMessage(request);
    return rpc_client->submitMessage(nfi, on_this_thread);
}

/*
 */
MessageChannel *NetworkBroker::getNewMessageChannelForRemoteHost(CNetworkAddress *node_network_address,
                                                                 EntityType type,
                                                                 bool use_shared_request_domain) {
    MessageChannel *channel = NULL;
    CHAOS_ASSERT(getServiceState() == 2)
    if(getServiceState() != 2) return NULL;
    switch (type) {
        case RAW:{
            CHAOS_ASSERT(!node_network_address)
            channel = use_shared_request_domain?
            new MessageChannel(this,
                               global_request_domain):
            new MessageChannel(this);
            break;
        }
        case RAW_MULTI_ADDRESS:{
            CHAOS_ASSERT(!node_network_address)
            channel = use_shared_request_domain?
            new MultiAddressMessageChannel(this,
                                           global_request_domain):
            new MultiAddressMessageChannel(this);
            break;
        }
        case DEVICE:{
            if(!node_network_address) return NULL;
            channel = use_shared_request_domain?
            new DeviceMessageChannel(this,
                                     static_cast<CDeviceNetworkAddress*>(node_network_address),
                                     false,
                                     global_request_domain):
            new DeviceMessageChannel(this, static_cast<CDeviceNetworkAddress*>(node_network_address));
            break;
        }
        case PERFORMANCE:{
            if(!node_network_address) return NULL;
            channel = use_shared_request_domain?
            new common::message::PerformanceNodeChannel(this,
                                                        node_network_address,
                                                        performance_session_managment.getLocalDirectIOClientInstance(),
                                                        global_request_domain):
            new common::message::PerformanceNodeChannel(this,
                                                        node_network_address,
                                                        performance_session_managment.getLocalDirectIOClientInstance());
            break;
        }
    }
    //check if the channel has been created
    if(channel){
        channel->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(channel->getChannelUUID(), channel));
    }
    return channel;
}

//!Metadata server channel creation
/*!
 Performe the creation of metadata server
 */
MDSMessageChannel *NetworkBroker::getMetadataserverMessageChannel(MessageRequestDomainSHRDPtr shared_request_domain) {
    MDSMessageChannel *channel = (shared_request_domain.get() == NULL)?
    (new MDSMessageChannel(this, GlobalConfiguration::getInstance()->getMetadataServerAddressList())):
    (new MDSMessageChannel(this, GlobalConfiguration::getInstance()->getMetadataServerAddressList(), shared_request_domain));
    if(channel){
        channel->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(channel->getChannelUUID(), static_cast<MessageChannel*>(channel)));
    }
    return channel;
}

//!Metadata server channel creation
/*!
 Performe the creation of metadata server
 */
MultiAddressMessageChannel *NetworkBroker::getMultiMetadataServiceRawMessageChannel() {
    MultiAddressMessageChannel *mc = getRawMultiAddressMessageChannel();
    if(mc){
        mc->addNode(GlobalConfiguration::getInstance()->getMetadataServerAddress());
    }
    if(mc){
        mc->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(mc->getChannelUUID(), mc));
    }
    return mc;
}

//!Device channel creation
/*!
 Performe the creation of device channel
 \param deviceNetworkAddress device node address
 */
DeviceMessageChannel *NetworkBroker::getDeviceMessageChannelFromAddress(CDeviceNetworkAddress *node_network_address,
                                                                        bool self_managed,
                                                                        bool use_shared_request_domain) {
    DeviceMessageChannel *channel = (use_shared_request_domain?
                                    new DeviceMessageChannel(this,
                                                             node_network_address,
                                                             self_managed,
                                                             global_request_domain):
                                    new DeviceMessageChannel(this,
                                                             node_network_address,
                                                             self_managed));
    
    
    if(channel){
        channel->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(channel->getChannelUUID(), static_cast<MessageChannel*>(channel)));
    }
    return channel;
}

//!performance channel creation
PerformanceNodeChannel *NetworkBroker::getPerformanceChannelFromAddress(CNetworkAddress  *node_network_address) {
    return static_cast<chaos::common::message::PerformanceNodeChannel*>(getNewMessageChannelForRemoteHost(node_network_address, PERFORMANCE));
}

//! Return a raw message channel
MessageChannel *NetworkBroker::getRawMessageChannel() {
   	return getNewMessageChannelForRemoteHost(NULL, RAW);
}

//! Return a raw multinode message channel
MultiAddressMessageChannel *NetworkBroker::getRawMultiAddressMessageChannel() {
   	return static_cast<MultiAddressMessageChannel*>(getNewMessageChannelForRemoteHost(NULL, RAW_MULTI_ADDRESS));
}

//! Return a raw multinode message channel
/*!
 Performe the creation of a raw multinode message channel
 */
chaos::common::message::MultiAddressMessageChannel *NetworkBroker::getRawMultiAddressMessageChannel(const std::vector<chaos::common::network::CNetworkAddress>& node_address) {
    MultiAddressMessageChannel *mc = new MultiAddressMessageChannel(this, node_address);
    if(mc){
        mc->init();
        boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
        active_rpc_channel.insert(make_pair(mc->getChannelUUID(), mc));
    }
    return mc;
}

//!Channel deallocation
/*!
 Perform the message channel deallocation
 */
void NetworkBroker::disposeMessageChannel(MessageChannel *message_channel_to_dispose) {
    if(!message_channel_to_dispose) return;
    
    //deallocate it
    CHAOS_NOT_THROW(message_channel_to_dispose->deinit(););
    
    boost::mutex::scoped_lock lock(mutex_map_rpc_channel_acces);
    //check if the channel is active
    if(active_rpc_channel.count(message_channel_to_dispose->getChannelUUID()) == 0) return;
    
    //remove the channel as active
    active_rpc_channel.erase(message_channel_to_dispose->getChannelUUID());
    
    //dispose it
    delete(message_channel_to_dispose);
}
//!Channel deallocation
void NetworkBroker::disposeMessageChannel(NodeMessageChannel *messageChannelToDispose) {
    NetworkBroker::disposeMessageChannel((MessageChannel*)messageChannelToDispose);
}

//!Rpc Channel deallocation
void NetworkBroker::disposeMessageChannel(chaos::common::message::MultiAddressMessageChannel *messageChannelToDispose) {
    NetworkBroker::disposeMessageChannel((MessageChannel*)messageChannelToDispose);
}

//!Rpc Channel deallocation
void NetworkBroker::disposeMessageChannel(chaos::common::message::MDSMessageChannel *messageChannelToDispose) {
    NetworkBroker::disposeMessageChannel(static_cast<MessageChannel*>(messageChannelToDispose));
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
chaos_direct_io::DirectIOClient *NetworkBroker::getSharedDirectIOClientInstance() {
    return direct_io_client;
}

//Return a new direct io client instance
chaos_direct_io::DirectIOClient *NetworkBroker::getNewDirectIOClientInstance() {
    MB_LAPP  << "Allocate a new DirectIOClient of type " << direct_io_client_impl;
    return ObjectFactoryRegister<common::direct_io::DirectIOClient>::getInstance()->getNewInstanceByName(direct_io_client_impl);
}
