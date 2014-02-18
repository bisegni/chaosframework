//
//  DirectIODeviceChannelGlobal.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIODeviceChannelGlobal_h
#define CHAOSFramework_DirectIODeviceChannelGlobal_h

#include <stdint.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
#define DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_OUTPUT_CHANNEL				1
#define DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_OUTPUT_CHANNEL_WITH_LIVE	2
#define DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_INPUT_CHANNEL				3
#define DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_INPUT_CHANNEL_WITH_LIVE		4
#define DIRECT_IO_DEVICE_CHANNEL_OPCODE_PUT_NEW_RECEIVED_COMMAND		5
				
				typedef struct DirectIODeviceChannelHeaderData {
					uint32_t device_hash;
				} DirectIODeviceChannelHeaderData, *DirectIODeviceChannelHeaderDataPtr;
				
			}
		}
	}
}

#endif
