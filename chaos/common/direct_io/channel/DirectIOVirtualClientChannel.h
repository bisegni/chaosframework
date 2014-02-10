/*
 *	DirectIOVirtualChannel.h
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
#ifndef __CHAOSFramework__DirectIOVirtualClientChannel__
#define __CHAOSFramework__DirectIOVirtualClientChannel__

#include <stdint.h>
//#include <chaos/common/utility/FastDelegate.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClient;
			class DirectIOForwarder;
			struct DirectIODataPack;
			
            namespace channel {
                
                class DirectIOVirtualClientChannel : public DirectIOVirtualChannel {
                    friend class chaos::common::direct_io::DirectIOClient;
					
					typedef uint32_t (chaos::common::direct_io::DirectIOForwarder::*ForwardDelegate)(chaos::common::direct_io::DirectIODataPack *data_pack);
					ForwardDelegate  forward_handler;
					DirectIOForwarder *client_instance;
				protected:
					uint32_t sendData(chaos::common::direct_io::DirectIODataPack *data_pack);

                    DirectIOVirtualClientChannel(std::string channel_name, uint8_t channel_route_index, bool priority);
                    ~DirectIOVirtualClientChannel();
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
