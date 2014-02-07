/*
 *	DirectIOServerEndpoint.h
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
#ifndef __CHAOSFramework__DirectIOServerEndpoint__
#define __CHAOSFramework__DirectIOServerEndpoint__

#include <string>

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/direct_io/DirectIOHandler.h>
#include <chaos/common/utility/FastSlotArray.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

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
			
			class DirectIOServerEndpoint : public DirectIOHandler {
				friend class DirectIODispatcher;
				
				boost::shared_mutex mutex_channel_slot;
				
				channel::DirectIOVirtualServerChannel **channel_slot;
				
				//! endpoint route index
				/*!
				 This is used by dispatcher to route the datapack to the right
				 endpoint.
				 */
				uint16_t endpoint_route_index;
				
				DirectIOServerEndpoint();
				~DirectIOServerEndpoint();
				
				// Event for a new data received
				void priorityDataReceived(DirectIODataPack *data_pack);
                
                // Event for a new data received
				void serviceDataReceived(DirectIODataPack *data_pack);

			public:
				//! Add a new channel instantiator
                channel::DirectIOVirtualServerChannel *registerChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance);
                
                //! Dispose the channel
                void deregisterChannelInstance(channel::DirectIOVirtualServerChannel *channel_instance);
				
				uint16_t getRouteIndex();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOServerEndpoint__) */
