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

#include <chaos/common/direct_io/DirectIOServerDataPack.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/direct_io/DirectIOHandler.h>

//#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

#define ENDPOINT_READ_

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
                class DirectIOVirtualServerChannel;
            }
			//forward declaration
			class DirectIODispatcher;
			
			class DirectIOServerEndpoint : public DirectIOHandler, private utility::InstancerContainer<channel::DirectIOVirtualServerChannel> {
				friend class DirectIODispatcher;
				
				//! endpoint route index
				/*!
				 This is used by dispatcher to route the datapack to the right
				 endpoint.
				 */
				uint16_t endpoint_route_index;
				
				DirectIOServerEndpoint();
				~DirectIOServerEndpoint();
				
				// Event for a new data received
				void priorityDataReceived(void *data_buffer, uint32_t data_len);
                
                // Event for a new data received
				void serviceDataReceived(void *data_buffer, uint32_t data_len);

			public:
				template<typename C>
				void addTypedChannel(std::string channel_name) {
					
				}
				
				uint16_t getRouteIndex();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOServerEndpoint__) */
