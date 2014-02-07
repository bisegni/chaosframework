/*
 *	DirectIOVirtualServerChannel.h
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
#ifndef __CHAOSFramework__DirectIOVirtualServerChannel__
#define __CHAOSFramework__DirectIOVirtualServerChannel__

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOEndpointHandler.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

#include <boost/function.hpp>
namespace chaos {
	namespace common {
		namespace direct_io {
			
			class DirectIOServerEndpoint;
			
            namespace channel {
				
				class DirectIOVirtualServerChannel : public virtual DirectIOVirtualChannel {
					friend class chaos::common::direct_io::DirectIOServerEndpoint;

				public:
					
					//typedef boost::function<void(DirectIODataPack*)> ServerChannelDelegate;
					typedef DirectIOEndpointHandler* ServerChannelDelegate;
					void setDelegate(ServerChannelDelegate delegate_function);
					void clearDelegate();
					uint16_t getEndpointRouteIndex();
				private:
					ServerChannelDelegate server_channel_delegate;
					uint16_t endpoint_route_index;
				protected:
					DirectIOVirtualServerChannel(std::string channel_name, uint8_t channel_route_index);
                    ~DirectIOVirtualServerChannel();
				};
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualServerChannel__) */
