/*
 *	DirectIOClientConnection.h
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

#ifndef __CHAOSFramework__DirectIOClientStartPoint__
#define __CHAOSFramework__DirectIOClientStartPoint__

#include <map>
#include <string>

#include <boost/thread.hpp>

#include <chaos/common/utility/TemplatedKeyObjectContainer.h>

#include <chaos/common/direct_io/DirectIOTypes.h>

#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/DirectIOURLManagment.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos {
	
	namespace common {
		
		namespace network {
			class NetworkBroker;
		}
		
		namespace direct_io {
            //forward delcarations
            class DirectIOClient;
            class DirectIOClientConnection;
			namespace channel{
				class DirectIOVirtualClientChannel;
			}
 
			//--------------connection event handler-----------------------
			//! Connection event enumeration
			namespace DirectIOClientConnectionStateType {
				typedef enum DirectIOClientConnectionStateType {
					DirectIOClientConnectionEventConnected,
					DirectIOClientConnectionEventDisconnected
				} DirectIOClientConnectionStateType;
            }

            //! Direct io client connection event handler
            class DirectIOClientConnectionEventHandler {
				friend class DirectIOClientConnection;
            protected:
                //! han
                virtual void handleEvent(DirectIOClientConnection *client_connection, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) = 0;
            };
			
			typedef  utility::TemplatedKeyObjectContainer< unsigned int, channel::DirectIOVirtualClientChannel*> DICKeyObjectContainer;
			
			
            //! Represent the start point of a messaget towards an endpoint
            /*!
                This class is the start point for a comunication winth a server endpoint,
                In definitive it can be viwed as a socket connected to a server::port that
                need to send the data towards an endpoint
             */
            class DirectIOClientConnection  :
			public DirectIOForwarder,
			public DirectIOURLManagment,
            protected DICKeyObjectContainer::FreeHandler {
                friend class DirectIOClient;
				friend class chaos::common::network::NetworkBroker;
								
			protected:
				std::string		url;
				std::string     server_description;
				std::string     custom_string_identification;
                uint16_t        endpoint;
				std::string     unique_uuid;
                DICKeyObjectContainer map_client_channels;
				DirectIOClientConnectionStateType::DirectIOClientConnectionStateType current_state;

				//! current client ip in string form
				static std::string my_str_ip;
				
				//! current client ip in 32 bit form
				static uint64_t my_i64_ip;

                DirectIOClientConnectionEventHandler *event_handler;
				
				//overriding ofr free object fuunction for the tempalted key object container superclass
                void freeObject(const DICKeyObjectContainer::TKOCElement& element);
				
				//! callend by client or by implementation when some event occour from socket conenction
				void lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionStateType state_type);
                
                // prepare header for defaut connection data
                inline DirectIODataPack *completeDataPack(DirectIODataPack *data_pack) {
                    data_pack->header.dispatcher_header.fields.route_addr = endpoint;
                    return data_pack;
                }
				
            public:
				
				DirectIOClientConnection(const std::string& _server_description,
                                         uint16_t _endpoint);
                virtual ~DirectIOClientConnection();
				
				//get client ip information
                static std::string getStrIp();
                static uint64_t getI64Ip();

				std::string	getUniqueUUID();
				
				std::string	getCustomStringIdentification();
				void		setCustomStringIdentification(std::string _custom_string_identificaiton);
				
				void setEventHandler(DirectIOClientConnectionEventHandler *_event_handler);
				
				const char * getServerDescription();
				
				std::string getURL();
				
				//! return the state of the connection
				DirectIOClientConnectionStateType::DirectIOClientConnectionStateType getState();
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				channel::DirectIOVirtualClientChannel *getNewChannelInstance(std::string channel_name);
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				void releaseChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOClientStartPoint__) */
