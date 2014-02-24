//
//  DirectIODeviceChannelGlobal.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIODeviceChannelGlobal_h
#define CHAOSFramework_DirectIODeviceChannelGlobal_h

#include <string>
#include <stdint.h>
#include <arpa/inet.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				namespace opcode {
                    
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
				
                
                //! Name space for grupping the varius headers for every DeviceChannelOpcode
                namespace opcode_headers {
                    
                    //! Heder for the DeviceChannelOpcodePutOutput[WithCache] opcodes
                    typedef struct DirectIODeviceChannelHeaderPutOpcode {
                        //! The 32bit hash value for the device that we need to insert
                        uint32_t device_hash;
                    } DirectIODeviceChannelHeaderData, *DirectIODeviceChannelHeaderDataPtr;
                    
                    //! Heder for the DeviceChannelOpcodeGetOutputFromCache opcode
                    typedef	union DirectIODeviceChannelHeaderGetOpcode {
                        //raw data representation of the header
                        char raw_data[12];
                        struct header {
                                //! The 32bit hash value for the device that we need to get
                            uint32_t	device_hash;
                                //! The endpoint where the channel is published
                            uint16_t	endpoint;
                                //! The port value for the device that we need to get
                            uint16_t	port;
                                //! The 32bit representation for the ip where send the answer
                            uint64_t	address;
                        } field;
                    } DirectIODeviceChannelHeaderGetOpcode, *DirectIODeviceChannelHeaderGetOpcodePtr;
                }
			}
		}
	}
}

#endif
