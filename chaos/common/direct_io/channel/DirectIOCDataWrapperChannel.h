//
//  DirectIOCDataWrapperChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DirectIOCDataWrapperChannel__
#define __CHAOSFramework__DirectIOCDataWrapperChannel__

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualServerChannel.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				class DirectIOCDataWrapperChannel : public DirectIOVirtualClientChannel, public DirectIOVirtualServerChannel, public chaos::common::direct_io::DirectIOEndpointHandler {
				public:
					typedef struct DirectIOCDataWrapperChannelHandler {
						virtual void consumeCDataWrapper(uint8_t channel_tag, chaos::common::data::CDataWrapper *data_wrapper) = 0;
					} DirectIOCDataWrapperChannelHandler;
					
					DirectIOCDataWrapperChannelHandler *cdatawrapper_handler;
					
					uint32_t pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t tag, chaos_data::SerializationBuffer *data_pack) ;
				protected:
					DirectIOCDataWrapperChannel(DirectIOCDataWrapperChannelHandler *delegate);
					
					void consumeDataPack(DirectIODataPack *dataPack);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOCDataWrapperChannel__) */
