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
#ifndef __CHAOSFramework__DirectIOClient__
#define __CHAOSFramework__DirectIOClient__

#include <map>
#include <string>
#include <inttypes.h>

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/TemplatedKeyObjectContainer.h>


#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/DirectIOURLManagment.h>

#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <chaos/common/data/cache/FastHash.h>

#include <boost/thread.hpp>

namespace chaos {
	
	namespace common {
		
		//forward declaration
		namespace network {
			class NetworkBroker;
		}
		namespace direct_io {
			//forward declaration
            namespace channel {
                class DirectIOVirtualClientChannel;
            }

            class DirectIOClientConnectionSharedMetricIO;
            
            // typedef std::pair<std::string, uint16_t> SharedCollectorKey;
            typedef  std::string SharedCollectorKey;
            typedef std::map< unsigned int, channel::DirectIOVirtualClientChannel* > ChannelMap;
            typedef std::map< unsigned int, channel::DirectIOVirtualClientChannel* >::iterator ChannelMapIterator;
            typedef utility::TemplatedKeyObjectContainer<std::string, DirectIOClientConnection*> DCKeyObjectContainer;
			//! Direct IO client base class
			/*!
			 This class represent the base interface for the operation on direct io output channel,
			 The client layer will connect to the server one to send data. Client can connect to
			 may server and the same data will be forwarded to all server
			 dio_client---> data message -->dio_server
			 */
			class DirectIOClient:
			public utility::NamedService,
			public utility::InizializableService,
			public DirectIOURLManagment,
            protected DCKeyObjectContainer::FreeHandler {
				friend class chaos::common::network::NetworkBroker;
				
				std::string			impl_alias;
                boost::atomic_uint	channel_counter;
                
                boost::mutex mutex_map_shared_collectors;
                std::map<SharedCollectorKey, ChaosSharedPtr<DirectIOClientConnectionSharedMetricIO> > map_shared_collectors;
			protected:
				void forwardEventToClientConnection(DirectIOClientConnection *client, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event_type);
                
                //! get new connection implementation
                virtual DirectIOClientConnection *_getNewConnectionImpl(std::string server_description, uint16_t endpoint) = 0;
                
                //! Release the connection
                virtual void _releaseConnectionImpl(DirectIOClientConnection *connection_to_release) = 0;
                
                DCKeyObjectContainer map_connections;
                void freeObject(const DCKeyObjectContainer::TKOCElement& element);
			public:
                DirectIOClient(std::string alias);
				virtual ~DirectIOClient();
				
				//! create a new connection for a server
				/*!
				 Allocate a new connection from server description with endpoitn ex: ip:p-port:s_port|endpoint
				 */
				DirectIOClientConnection *getNewConnection(const std::string& server_description_with_endpoint);
                
                //DirectIOClientConnection *getNewConnection(std::string server_description, uint16_t endpoint);
				
				//! Release the connection
				void releaseConnection(DirectIOClientConnection *connection_to_release);
                
                
                //! Initialize instance
                void init(void *init_data) ;
                
                
                //! Deinit the implementation
                void deinit() ;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOClient__) */
