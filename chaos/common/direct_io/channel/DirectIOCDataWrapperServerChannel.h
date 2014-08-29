/*
 *	DirectIOCDataWrapperServerChannel.h
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
#ifndef __CHAOSFramework__DirectIOCDataWrapperServerChannel__
#define __CHAOSFramework__DirectIOCDataWrapperServerChannel__

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIOCDataWrapperServerChannel, DirectIOVirtualServerChannel), public chaos::common::direct_io::DirectIOEndpointHandler {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOCDataWrapperServerChannel)
				public:
					typedef class DirectIOCDataWrapperServerChannelHandler {
                    public:
						//! Receive the CDataWrapper forwarded by the channel
						/*!
						 Receive the CdataWrapper forwarded by the channel, the deallocation is demanded to the handler
						 */
						virtual void consumeCDataWrapper(uint8_t channel_opcode, chaos::common::data::CDataWrapper *data_wrapper) = 0;
					} DirectIOCDataWrapperServerChannelHandler;
					
					uint32_t pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t tag, chaos_data::SerializationBuffer *data_pack) ;
				protected:
                    DirectIOCDataWrapperServerChannelHandler *cdatawrapper_handler;
                    
					DirectIOCDataWrapperServerChannel(std::string alias);
					
					int consumeDataPack(DirectIODataPack *dataPack, DirectIOSynchronousAnswerPtr synchronous_answer);
				public:
                    void setHandler(DirectIOCDataWrapperServerChannelHandler *handler);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOCDataWrapperServerChannel__) */
