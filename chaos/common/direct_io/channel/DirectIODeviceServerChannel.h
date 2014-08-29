/*
 *	DirectIODeviceServerChannel.h
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
#ifndef __CHAOSFramework__DirectIODeviceServerChannel__
#define __CHAOSFramework__DirectIODeviceServerChannel__


#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIODeviceServerChannel, DirectIOVirtualServerChannel), public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIODeviceServerChannel)
				public:
					//! Device handler definition
					typedef class DirectIODeviceServerChannelHandler {
                    public:
						//! Receive the CDataWrapper forwarded by the channel
						/*!
						 Receive the CdataWrapper forwarded by the channel, the deallocation is demanded to the handler
                         \param channel_opcode the opcode of the comand received by the channel
                         \param channel_header_ptr the abstract pointr for the channel header. Implementations need to cast it according to opcode value
                         \param channel_data the data (if speified by channel)
						 */
						virtual int consumePutEvent(opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
													 void *channel_data,
													 uint32_t channel_data_len,
													 DirectIOSynchronousAnswerPtr synchronous_answer) = 0;
						virtual int consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
													 void *channel_data,
													 uint32_t channel_data_len,
													 DirectIOSynchronousAnswerPtr synchronous_answer) = 0;
					} DirectIODeviceServerChannelHandler;

                    void setHandler(DirectIODeviceServerChannelHandler *_handler);
				protected:
					//! Handler for the event
					DirectIODeviceServerChannelHandler *handler;
					
					DirectIODeviceServerChannel(std::string alias);
					
					int consumeDataPack(DirectIODataPack *dataPack, DirectIOSynchronousAnswerPtr synchronous_answer);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIODeviceServerChannel__) */
