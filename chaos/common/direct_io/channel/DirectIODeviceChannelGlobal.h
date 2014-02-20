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
				
				namespace DeviceChannelOpcode {

					/*!
						\enum DeviceChannelOpcode
						Opcode used by the DirectIO device channel
					 */
					typedef enum DeviceChannelOpcode {
						DeviceChannelOpcodePutOutput				= 1,	/**< send the output dataset */
						DeviceChannelOpcodePutOutputWithCache		= 2,	/**< send the output dataset with cache specification */
						DeviceChannelOpcodeGetOutputFromCache		= 4,	/**< request the output dataset from the cache */
						DeviceChannelOpcodePutInput					= 8,	/**< send the input dataset */
						DeviceChannelOpcodePutInputWithCache		= 16,	/**< send the input dataset with cache specification */
						DeviceChannelOpcodePutNewReceivedCommand	= 32	/**< send over the channel the received command */
					} DeviceChannelOpcode;
				}
				typedef struct DirectIODeviceChannelHeaderData {
					uint32_t device_hash;
				} DirectIODeviceChannelHeaderData, *DirectIODeviceChannelHeaderDataPtr;
				
			}
		}
	}
}

#endif
