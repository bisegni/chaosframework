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
#ifndef __CHAOSFramework__DirectIOServerEndpoint__
#define __CHAOSFramework__DirectIOServerEndpoint__

#include <string>
#include <chaos/common/chaos_types.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/direct_io/DirectIOHandler.h>
#include <chaos/common/direct_io/DirectIOServerPublicInterface.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
                class DirectIOVirtualServerChannel;
            }
			//forward declaration
			class DirectIODispatcher;
			
#define MAX_ENDPOINT_CHANNEL 256
			
			class DirectIOServerEndpoint:
            public DirectIOHandler {
				friend class DirectIODispatcher;
				//boost::shared_mutex mutex_channel_slot;
                ChaosSharedMutex shared_mutex;
				channel::DirectIOVirtualServerChannel **channel_slot;
				
				//! endpoint route index
				/*!
				 This is used by dispatcher to route the datapack to the right
				 endpoint.
				 */
				uint16_t endpoint_route_index;
                
                DirectIOServerPublicInterface *server_public_interface;
                
				DirectIOServerEndpoint();
				~DirectIOServerEndpoint();
				
				// Event for a new data received
                int priorityDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                         chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                
                // Event for a new data received
                int serviceDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);

			public:
				//! Add a new channel instantiator
                channel::DirectIOVirtualServerChannel *registerChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance);
                
                //! Dispose the channel
                void deregisterChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance);
				
				uint16_t getRouteIndex();
				DirectIOServerPublicInterface * getPublicServerInterface() const;
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				channel::DirectIOVirtualServerChannel *getNewChannelInstance(std::string channel_name) throw (CException);
				
				//! New channel allocation by name
				/*!
				 Allocate a new channel and initialize it
				 */
				void releaseChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance) throw (CException);
				
				std::string getUrl();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOServerEndpoint__) */
