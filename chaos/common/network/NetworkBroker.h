/*
 *	NetworkBroker.h
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
#ifndef ChaosFramework_MessageBroker_h
#define ChaosFramework_MessageBroker_h

#include <map>
#include <string>
#include <boost/thread/mutex.hpp>

#include <chaos/common/rpc/ChaosRpc.h>
#include <chaos/common/sync_rpc/ChaosSyncRpc.h>
#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/action/EventAction.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/network/NetworkForwardInfo.h>
#include <chaos/common/network/PerformanceManagment.h>
#include <chaos/common/utility/StartableService.h>

#include <chaos/common/direct_io/DirectIO.h>


namespace chaos {
	
	//forward declaration
	class AbstractCommandDispatcher;
	class AbstractEventDispatcher;
	
	namespace event {
		namespace channel {
			class AlertEventChannel;
			class InstrumentEventChannel;
		}
		class EventServer;
		class EventClient;
	}

	namespace common {

		namespace message {
			class MessageChannel;
			class NodeMessageChannel;
			class MDSMessageChannel;
			class DeviceMessageChannel;
			class PerformanceNodeChannel;
		}

		namespace network {
			using namespace std;
			using namespace boost;
			using namespace chaos::common::message;
			using namespace chaos::common::data;
			using namespace chaos::common::direct_io;
			
			//! Channel Type Enumeration
			/*!
			 Constants that identify the type of the channel to create
			 */
			typedef enum {
				RAW = 0,		/*!< Identify a raw channel used to send data pack to remote server */
				MDS,			/*!< Identify a mds specific channel used to send data pack to the metadataserver */
				DEVICE,			/*!< Identify a device specific channel used to send data pack to the target control unit */
				PERFORMANCE		/*!< Identify a performance specific channel used to send and receive various performance information and test between two chaos node using directio system */
			} EntityType;
			
			//! Message Broker
			/*!
			 The NetworkBroker is the manager for the message in chaos framework. It contains the reference to
			 chaos rpc client and server abstract class and to the message dispatcher abstract class.
			 It abstract the !CHAOS rule for sending message and wait for answer and other facility.
			 */
			class NetworkBroker:
			public utility::StartableService {
				//! performance session managment
				chaos::common::network::PerformanceManagment performance_session_managment;
				
                //! point to current host and port
                std::string published_host_and_port;
                
                //! has the server address of mds for fast access
                std::string metadata_server_address;
                
                bool can_use_metadata_server;
                
                //!name of direct io client implementation to use
                std::string direct_io_client_impl;
                
				//!Direct IO server interface
				direct_io::DirectIOServer *direct_io_server;
                
				//! Direct IO dispatcher
				direct_io::DirectIODispatcher *direct_io_dispatcher;
				
				//!Event Client for event forwarding
				event::EventClient *event_client;
				
				//!Event server for event handlind
				event::EventServer *event_server;
				
				//! Rpc client for message forwarding
				RpcClient *rpc_client;
				
				//! Rpc server for message listening
				RpcServer *rpc_server;

                //! Rpc sync interface
                sync_rpc::RpcSyncServer *sync_rpc_server;
                
				//rpc action dispatcher
				AbstractCommandDispatcher *command_dispatcher;
				
				//! Rpc server for message dispatcher
				AbstractEventDispatcher *event_dispatcher;
				
				//!keep track of active channel
				map<string, MessageChannel*> active_rpc_channel;
				//!Mutex for rpc channel managment
				boost::mutex mutex_map_rpc_channel_acces;
				
				//!keep track of active channel
				map<string, chaos::event::channel::EventChannel*> active_event_channel;
				
				//!Mutex for event channel managment
				boost::mutex muext_map_event_channel_access;
				
				
				//! private raw channel creation
				/*!
				 Get new message channel for comunicate with remote host
				 \param nodeNetworkAddress node address info
				 \param type channel type to create
				 */
				MessageChannel *getNewMessageChannelForRemoteHost(CNetworkAddress *nodeNetworkAddress, EntityType type);
				
			public:
				
				//! Basic Constructor
				NetworkBroker();
				
				//! Basic Destructor
				virtual ~NetworkBroker();
				
				//!Message Broker initialization
				/*!
				 * Initzialize the Message Broker. In this step are taken the configured implementation
				 * for the rpc client and server and for the dispatcher. All these are here initialized
				 */
				virtual void init(void *initData = NULL) throw(CException);
				
				//!NetworkBroker deinitialization
				/*!
				 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
				 */
				virtual void deinit() throw(CException);
				
				//!start all internal engine for client, server and message dispatcher
				/*!
				 * all part are started
				 */
				virtual void start() throw(CException);
				
				//!start all internal engine for client, server and message dispatcher
				/*!
				 * all part are started
				 */
				virtual void stop() throw(CException);
				
				//!Get the published port
				/*!
				 Return the port where the rpc server has been published
				 */
				int getPublishedPort();
				
				//!Get the serv:port description for the rpc server
				/*!
				 Return the host and port where rpc server has benn published
				 */
				void getPublishedHostAndPort(string&);
				
				//!Return the sync rpc url
				/*!
				 return the rpc url
				 */
				std::string getSyncRPCUrl();
				
				//!Return the rpc url
				/*!
				 return the rpc url
				 */
				std::string getRPCUrl();
				
				//!Return the direct io url
				/*!
				 Return the direction server base url (without endpoint)
				 */
				std::string getDirectIOUrl();
				
				//! event Action registration for the current instance of NetworkBroker
				/*!
				 Register an event actions defined for a detgerminated event type
				 \param eventAction the actio to register
				 \param eventType a type for the event for which the user want to register
				 */
				void registerEventAction(EventAction *eventAction, chaos::event::EventType eventType, const char * const identification = NULL);
				
				//!Event Action deregistration
				/*!
				 Deregister an event action
				 */
				void deregisterEventAction(EventAction *eventAction);
				
				//!Event channel creation
				/*!
				 Performe the creation of an event channel of a desidered type
				 \param eventType is one of the value listent in EventType enum that specify the
				 type of the eventfor wich we want a channel
				 */
				chaos::event::channel::EventChannel *getNewEventChannelFromType(chaos::event::EventType  eventType);
				
				//! Alert Event Creation
				chaos::event::channel::AlertEventChannel *getNewAlertEventChannel();
				
				//! Instrument Event Creation
				chaos::event::channel::InstrumentEventChannel *getNewInstrumentEventChannel();
				
				//!Event channel deallocation
				/*!
				 Perform the event channel deallocation
				 */
				void disposeEventChannel(chaos::event::channel::EventChannel *eventChannelToDispose);
				
				
				//! event submission
				/*!
				 Submit an event
				 \param event the new evento to submit
				 */
				bool submitEvent(chaos::event::EventDescriptor *event);
				
				//! Action registration for the current isntance of NetworkBroker
				/*!
				 Register actions defined by AbstractActionDescriptor instance contained in the array
				 */
				void registerAction(DeclareAction*);
				
				//!Action deregistration
				/*!
				 Deregister actions owned by input parameter that are hosted in this current instance of message broker
				 */
				void deregisterAction(DeclareAction*);
				
				//!message submition
				/*!
				 Submit a message specifing the destination
				 \param serverAndPort server:port addres of remote endpoint
				 \param message the message coded into key/value semantics
				 \param onThisThread if true the message is forwarded in the same thread of the caller
				 */
				bool submitMessage(string& serverAndPort,
								   chaos_data::CDataWrapper *message,
								   NetworkErrorHandler handler = NULL,
								   const char * senderIdentifier = NULL,
								   int64_t senderTag = (int64_t)0,
								   bool onThisThread=false);
				
				//!message request
				/*!
				 Submite a new request to send to the remote host
				 \param serverAndPort server:port addres of remote endpoint
				 \param request the request coded into key/value semantics
				 \param onThisThread if true the message is forwarded in the same thread of the caller
				 */
				bool submiteRequest(string& serverAndPort,
									chaos_data::CDataWrapper *request,
									NetworkErrorHandler handler = NULL,
									const char * senderIdentifier = NULL,
									int64_t senderTag = (int64_t)0,
									bool onThisThread=false);
				
				//!message submition
				/*!
				 Submit a message to the metadata server
				 
				 bool submitMessageToMetadataServer(CDataWrapper*, bool onThisThread=false);*/
				
				//!Metadata server channel creation
				/*!
				 Performe the creation of metadata server
				 */
				MDSMessageChannel *getMetadataserverMessageChannel();
				
				//!Device channel creation
				/*!
				 Performe the creation of device channel
				 \param deviceNetworkAddress device node address
				 */
				DeviceMessageChannel *getDeviceMessageChannelFromAddress(CDeviceNetworkAddress  *deviceNetworkAddress);
    
				//!performance channel creation
				/*!
				 Performe the creation of performance channel thowards a network node
				 \param note_network_address the address of the chaos node(network broker)
				 */
				chaos::common::message::PerformanceNodeChannel *getPerformanceChannelFromAddress(CNetworkAddress  *node_network_address);
				
				//!Rpc Channel deallocation
				/*!
				 Perform the message channel deallocation
				 */
				void disposeMessageChannel(MessageChannel *messageChannelToDispose);
				
				//!Rpc Channel deallocation
				/*!
				 Perform the node message channel deallocation
				 */
				void disposeMessageChannel(NodeMessageChannel *messageChannelToDispose);
				
				//!Allocate a new endpoint in the direct io server
				/*!
				 Allcoate a new endpoint into the server act to receive directio data pack.
				 \return The endpoint class or NULL in case the maximum number of endpoint is reached.
				 */
				DirectIOServerEndpoint *getDirectIOServerEndpoint();
				
				//!Dispose an endpoint of the direct io server
				/*!
				 Allcoate a new endpoint into the server act to receive directio data pack.
				 \param end_point The endpoint to relase
				 */
				void releaseDirectIOServerEndpoint(DirectIOServerEndpoint *end_point);
				
				//!Return a new direct io client instance
				/*!
				 Allcoate and return a new direct io client instance. This isntance is totaly managed
				 by the class that request for it. His deallocation is not done in automatic.
				 \return A new instance of the direct io client
				 */
				DirectIOClient *getDirectIOClientInstance();
			};
		}
	}
}
#endif
