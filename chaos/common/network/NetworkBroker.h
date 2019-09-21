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
#ifndef ChaosFramework_MessageBroker_h
#define ChaosFramework_MessageBroker_h

#include <map>
#include <string>
#include <boost/thread/mutex.hpp>


#include <chaos/common/direct_io/DirectIO.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/action/EventAction.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/event/event.h>
#include <chaos/common/network/NetworkForwardInfo.h>
#include <chaos/common/network/PerformanceManagment.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/message/MessageRequestDomain.h>
#include <chaos/common/direct_io/DirectIO.h>


namespace chaos {
	
	//forward declaration
	class AbstractCommandDispatcher;
	class AbstractEventDispatcher;
	class RpcClient;
	class RpcServer;

	namespace common {
        namespace event {
            namespace channel {
                class AlertEventChannel;
                class InstrumentEventChannel;
            }
            class EventServer;
            class EventClient;
        }
        
		namespace message {
			class MessageChannel;
			class NodeMessageChannel;
            class MultiAddressMessageChannel;
			class MDSMessageChannel;
			class DeviceMessageChannel;
            class DeviceMessageChannelListener;
			class PerformanceNodeChannel;
		}

		namespace network {
			
			//! Channel Type Enumeration
			/*!
			 Constants that identify the type of the channel to create
			 */
			typedef enum {
				RAW = 0,            /*!< Identify a raw channel used to send data pack to remote server */
                RAW_MULTI_ADDRESS,	/*!< Identify a multinode raw channel used to send data pack to one or more remote server */
				DEVICE,             /*!< Identify a device specific channel used to send data pack to the target control unit */
				PERFORMANCE         /*!< Identify a performance specific channel used to send and receive various performance information and test between two chaos node using directio system */
			} EntityType;
			
			//! Message Broker
			/*!
			 The NetworkBroker is the manager for the message in chaos framework. It contains the reference to
			 chaos rpc client and server abstract class and to the message dispatcher abstract class.
			 It abstract the !CHAOS rule for sending message and wait for answer and other facility.
			 */
			class NetworkBroker:
			public chaos::common::utility::StartableService,
			public chaos::common::utility::Singleton<NetworkBroker> {
				friend class chaos::common::utility::Singleton<NetworkBroker>;
				//! performance session managment
				chaos::common::network::PerformanceManagment performance_session_managment;
				
                //! point to current host and port
                std::string published_host_and_port;
                
                //!name of direct io client implementation to use
                std::string direct_io_client_impl;
                
				//!Direct IO server interface
				chaos::common::direct_io::DirectIOServer *direct_io_server;
                
				//! Direct IO dispatcher
				chaos::common::direct_io::DirectIODispatcher *direct_io_dispatcher;
				
                //! global direct io client instance
                chaos::common::direct_io::DirectIOClient *direct_io_client;
                
				//!Event Client for event forwarding
				event::EventClient *event_client;
				
				//!Event server for event handlind
                chaos::common::event::EventServer *event_server;
				
				//! Rpc client for message forwarding
                chaos::RpcClient *rpc_client;
				
				//! Rpc server for message listening
                chaos::RpcServer *rpc_server;
                
				//rpc action dispatcher
				AbstractCommandDispatcher *rpc_dispatcher;
				
				//! Rpc server for message dispatcher
				AbstractEventDispatcher *event_dispatcher;
				
				//!keep track of active channel
                map<std::string, chaos::common::message::MessageChannel*> active_rpc_channel;
				//!Mutex for rpc channel managment
				boost::mutex mutex_map_rpc_channel_acces;
				
				//!keep track of active channel
                map<std::string, chaos::common::event::channel::EventChannel*> active_event_channel;
				
				//!Mutex for event channel managment
				boost::mutex muext_map_event_channel_access;

                    //! global shared message request domain
                chaos::common::message::MessageRequestDomainSHRDPtr global_request_domain;

				//! private raw channel creation
				/*!
				 Get new message channel for comunicate with remote host
				 \param nodeNetworkAddress node address info
				 \param type channel type to create
				 */
                chaos::common::message::MessageChannel *getNewMessageChannelForRemoteHost(chaos::common::network::CNetworkAddress *node_network_address,
                                                                                          EntityType type,
                                                                                          bool use_shared_request_domain = true);
                                //! Basic Constructor
				NetworkBroker();
				
                                                                                            
			public:
				
				
				//! Basic Destructor
				virtual ~NetworkBroker();
				
				//!Message Broker initialization
				/*!
				 * Initzialize the Message Broker. In this step are taken the configured implementation
				 * for the rpc client and server and for the dispatcher. All these are here initialized
				 */
				virtual void init(void *initData = NULL);
				
				//!NetworkBroker deinitialization
				/*!
				 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
				 */
				virtual void deinit();
				
				//!start all internal engine for client, server and message dispatcher
				/*!
				 * all part are started
				 */
				virtual void start();
				
				//!start all internal engine for client, server and message dispatcher
				/*!
				 * all part are started
				 */
				virtual void stop();
				
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
                void registerEventAction(EventAction *eventAction,
                                         chaos::common::event::EventType eventType,
                                         const char * const identification = NULL);
				
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
				chaos::common::event::channel::EventChannel *getNewEventChannelFromType(chaos::common::event::EventType  eventType);
				
				//! Alert Event Creation
				chaos::common::event::channel::AlertEventChannel *getNewAlertEventChannel();
				
				//! Instrument Event Creation
				chaos::common::event::channel::InstrumentEventChannel *getNewInstrumentEventChannel();
				
				//!Event channel deallocation
				/*!
				 Perform the event channel deallocation
				 */
				void disposeEventChannel(chaos::common::event::channel::EventChannel *eventChannelToDispose);
				
				
				//! event submission
				/*!
				 Submit an event
				 \param event the new evento to submit
				 */
				bool submitEvent(chaos::common::event::EventDescriptor *event);
				
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
				bool submitMessage(const string& serverAndPort,
								   chaos::common::data::CDWUniquePtr message);
				
				//!message request
				/*!
				 Submite a new request to send to the remote host
				 \param serverAndPort server:port addres of remote endpoint
				 \param request the request coded into key/value semantics
				 \param onThisThread if true the message is forwarded in the same thread of the caller
				 */
				bool submiteRequest(const string& serverAndPort,
                                    chaos::common::data::CDWUniquePtr request,
                                    std::string sender_node_id,
                                    uint32_t sender_request_id);
				
                
                //!send interparocess message
                /*!
                 forward the message directly to the dispatcher for broadcasting it 
                 to the registered rpc domain
                 */
                chaos::common::data::CDWUniquePtr submitInterProcessMessage(chaos::common::data::CDWUniquePtr message,
                                                                             bool onThisThread=false);
                
				//!message submition
				/*!
				 Submit a message to the metadata server
				 
				 bool submitMessageToMetadataServer(CDataWrapper*, bool onThisThread=false);*/
				
				//!Metadata server channel creation
				/*!
				 Performe the creation of metadata server
				 */
                chaos::common::message::MDSMessageChannel *getMetadataserverMessageChannel(chaos::common::message::MessageRequestDomainSHRDPtr shared_request_domain = chaos::common::message::MessageRequestDomainSHRDPtr());

				
				//!Device channel creation
				/*!
				 Performe the creation of device channel
				 \param deviceNetworkAddress device node address
				 */
				chaos::common::message::DeviceMessageChannel *getDeviceMessageChannelFromAddress(chaos::common::network::CDeviceNetworkAddress  *deviceNetworkAddress,
                                                                                                 bool self_managed = false,
                                                                                                 bool use_shared_request_domain = true);
    
                //! Return a raw message channel
                /*!
                 Performe the creation of a raw channel
                 */
                chaos::common::message::MessageChannel *getRawMessageChannel();
                
                    //! Return a raw multinode message channel
                /*!
                 Performe the creation of a raw multinode message channel
                 */
                chaos::common::message::MultiAddressMessageChannel *getRawMultiAddressMessageChannel();

                //! Return a raw multinode message channel
                /*!
                 Performe the creation of a raw multinode message channel
                 */
                chaos::common::message::MultiAddressMessageChannel *getRawMultiAddressMessageChannel(const std::vector<chaos::common::network::CNetworkAddress>& node_address);
                
                    //!return a multi node channel for all metadata service
                chaos::common::message::MultiAddressMessageChannel *getMultiMetadataServiceRawMessageChannel();

				//!performance channel creation
				/*!
				 Performe the creation of performance channel thowards a network node
				 \param note_network_address the address of the chaos node(network broker)
				 */
				chaos::common::message::PerformanceNodeChannel *getPerformanceChannelFromAddress(chaos::common::network::CNetworkAddress  *node_network_address);
				
				//!Rpc Channel deallocation
				/*!
				 Perform the message channel deallocation
				 */
				void disposeMessageChannel(chaos::common::message::MessageChannel *messageChannelToDispose);
				
				//!Rpc Channel deallocation
				/*!
				 Perform the node message channel deallocation
				 */
				void disposeMessageChannel(chaos::common::message::NodeMessageChannel *messageChannelToDispose);

                //!Rpc Channel deallocation
                /*!
                 Perform the node message channel deallocation
                 */
                void disposeMessageChannel(chaos::common::message::MultiAddressMessageChannel *messageChannelToDispose);

                //!Rpc Channel deallocation
                /*!
                 Perform the node message channel deallocation
                 */
                void disposeMessageChannel(chaos::common::message::MDSMessageChannel *messageChannelToDispose);
                
				//!Allocate a new endpoint in the direct io server
				/*!
				 Allcoate a new endpoint into the server act to receive directio data pack.
				 \return The endpoint class or NULL in case the maximum number of endpoint is reached.
				 */
				chaos::common::direct_io::DirectIOServerEndpoint *getDirectIOServerEndpoint();
				
				//!Dispose an endpoint of the direct io server
				/*!
				 Allcoate a new endpoint into the server act to receive directio data pack.
				 \param end_point The endpoint to relase
				 */
                void releaseDirectIOServerEndpoint(chaos::common::direct_io::DirectIOServerEndpoint *end_point);
				
				//!Return a new direct io client instance
				/*!
				 Allocate and return a new direct io client instance. This isntance is totaly managed
				 by the class that request for it. His deallocation is not done in automatic.
				 \return A new instance of the direct io client
				 */
				chaos::common::direct_io::DirectIOClient *getNewDirectIOClientInstance();
                
                //!Return a new direct io client instance
                /*!
                 Allcoate and return a new direct io client instance. This isntance is totaly managed
                 by the class that request for it. His deallocation is not done in automatic.
                 \return A new instance of the direct io client
                 */
                chaos::common::direct_io::DirectIOClient *getSharedDirectIOClientInstance();
			};
		}
	}
}
#endif
