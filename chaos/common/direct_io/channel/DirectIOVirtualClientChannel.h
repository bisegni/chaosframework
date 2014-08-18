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
#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClient;
			class DirectIOForwarder;
            class DirectIOClientConnection;
            
			struct DirectIODataPack;
			
            namespace channel {
                
				//!strucutre to maintaine the information about sent buffer and free it
				struct DisposeSentMemoryInfo {
					uint8_t		sent_part;
					uint16_t	sent_opcode;
					channel::DirectIOVirtualClientChannel *channel;
					DisposeSentMemoryInfo(channel::DirectIOVirtualClientChannel *_channel,
										  uint8_t _sent_part,
										  uint16_t _sent_opcode):channel(_channel), sent_part(_sent_part), sent_opcode(_sent_opcode){};
				};
				
                class DirectIOVirtualClientChannel : protected DirectIOVirtualChannel {
					friend class chaos::common::direct_io::DirectIOClientConnection;
                    
					DirectIOForwarderHandler  forward_handler;

				protected:
					//uint16_t endpoint;
					DirectIOForwarder *client_instance;
					
					int64_t sendData(chaos::common::direct_io::DirectIODataPack *data_pack);
					virtual void freeSentData(void *data, DisposeSentMemoryInfo& dispose_memory_info);
					
					// prepare header for defaut connection data
					inline DirectIODataPack *completeDataPack(DirectIODataPack *data_pack) {
						data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
						return data_pack;
					}
					
                    DirectIOVirtualClientChannel(std::string channel_name, uint8_t channel_route_index, bool priority);
                    ~DirectIOVirtualClientChannel();
				public:
					//void setEndpoint(uint16_t _endpoint);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
