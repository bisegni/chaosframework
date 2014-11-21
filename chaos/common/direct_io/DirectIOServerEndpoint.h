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
#include <chaos/common/thread/SpinLock.h>
#include <chaos/common/exception/CException.h>
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
			
			class DirectIOServerEndpoint : public DirectIOHandler {
				friend class DirectIODispatcher;
				//boost::shared_mutex mutex_channel_slot;
				chaos::common::thread::Spinlock spinlock;
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
				int priorityDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer);
                
                // Event for a new data received
				int serviceDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer);

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
