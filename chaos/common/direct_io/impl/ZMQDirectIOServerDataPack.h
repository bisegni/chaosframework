/*
 *	ZMQDirectIOServerDataPack.h
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
#ifndef __CHAOSFramework__ZMQDirectIOServerDataPack__
#define __CHAOSFramework__ZMQDirectIOServerDataPack__

#include <chaos/common/direct_io/DirectIOServerDataPack.h>

#include <zmq.h>

namespace chaos {
	namespace common {
		namespace direct_io {
            
            class ZMQDirectIOServer;
            namespace impl {
				
				typedef struct ZMQSocketAndMessage {
					void *zmq_socket;
					zmq_msg_t msg;
				} ZMQSocketAndMessage;
				
				//! ZMQ implementation for the server data pack
				class ZMQDirectIOServerDataPack : public DirectIOServerDataPack {
					friend class ZMQDirectIOServer;
					
					ZMQSocketAndMessage *socket_message;
					//! dealer/rep socket
					ZMQDirectIOServerDataPack(ZMQSocketAndMessage *_socket_message);
					
					~ZMQDirectIOServerDataPack();
					
				public:
					
					void getDataPtr();
					
					uint32_t getDataSize();
					
					uint32_t sendAnswer(void *data_ptr, uint32_t data_len);
				};
			}
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ZMQDirectIOServerDataPack__) */
