/*
 *	DirectIOVirtualChannel
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
#ifndef __CHAOSFramework__DirectIOVirtualChannel__
#define __CHAOSFramework__DirectIOVirtualChannel__

#include <string>
#include <stdint.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/direct_io/channel/DirectIOChannelGlobal.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClient;
			
			namespace channel {
				class DirectIOVirtualChannel : public  NamedService {
					friend class chaos::common::direct_io::DirectIOClient;
					
				protected:
					uint8_t channel_route_index;
					DirectIOVirtualChannel(std::string _channel_name, uint8_t _channel_route_index);
					virtual ~DirectIOVirtualChannel();
				public:
					uint8_t getChannelRouterIndex();
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
