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
						DeviceChannelOpcodeGetLastOutput            = 2,	/**< request the output dataset*/
						DeviceChannelOpcodePutInput					= 4,	/**< send the input dataset */
						DeviceChannelOpcodePutNewReceivedCommand	= 8	/**< send over the channel the received command */
					} DeviceChannelOpcode;
					
					/*!
                     \enum DeviceChannelOpcode
                     Opcode used by the DirectIO device channel
					 */
					typedef enum PerformanceChannelOpcode {
						PerformanceChannelOpcodeReqRoundTrip			= 1,	/**< forwarda a start of a roundtrip test */
						PerformanceChannelOpcodeRespRoundTrip			= 2,	/**< perform an answer to the roundtrip test */
					} PerformanceChannelOpcode;
				}
				
                
                //! Name space for grupping the varius headers for every DeviceChannelOpcode
                namespace opcode_headers {
#define	GET_PUT_OPCODE_FIXED_PART_LEN	6
					//!macro used to get pointer to the start of the key data
#define GET_PUT_OPCODE_KEY_PTR(h) (void*)((char*)h+6)
					
                    //! Heder for the DeviceChannelOpcodePutOutput[WithCache] opcodes
                    typedef struct DirectIODeviceChannelHeaderPutOpcode {
							//! The 32bit hash value for the device that we need to insert
                        uint32_t device_hash;
							//! The 8 bit tag field
						uint8_t tag;
                            //! The 8 bit key length field
                        uint8_t key_len;
							//the pointer to key data
						void*   key_data;
                    } DirectIODeviceChannelHeaderData, *DirectIODeviceChannelHeaderDataPtr;
					
					
                    //! Header for the DeviceChannelOpcodeGetOutputFromCache opcode
					/*!
					 this is the header for request the last output channel dataset
					 found on shared dataproxy cache. The key of the item to search
					 must be sent in the data part of the message
					 */
                    typedef	union DirectIODeviceChannelHeaderGetOpcode {
                        //raw data representation of the header
                        char raw_data[18];
                        struct header {
							//! The endpoint where the channel is published
                            uint16_t	endpoint;
							//! The priority port value for the device that we need to get
                            uint16_t	p_port;
							//! The priority port value for the device that we need to get
                            uint16_t	s_port;
                                //! The 32bit hash value for the device that we need to get
                            uint32_t	device_hash;
							//! The 32bit hash value for the unique identification of the answer ip string ip::s_port:p_port for fast retryve on other side
                            uint32_t	answer_server_hash;
                                //! The 32bit representation for the ip where send the answer
                            uint64_t	address;
                        } field;
                    } DirectIODeviceChannelHeaderGetOpcode, *DirectIODeviceChannelHeaderGetOpcodePtr;
					
					
					//! Header for the DeviceChannelOpcodeGetOutputFromCache opcode
					/*!
					 this is the header for request the last output channel dataset
					 found on shared dataproxy cache. The key of the item to search
					 must be sent in the data part of the message
					 */
                    typedef	union DirectIOPerformanceChannelHeaderOpcodeRoundTrip {
						//raw data representation of the header
                        char raw_data[16];
                        struct header {
							//! The 64bit value for the timestamp get from the client part at the start of the roundtrip test
                            uint64_t	start_rt_ts;
							//! The 64bit value for the timestamp get from the server layer part at the start of the roundtrip test
                            uint64_t	receiver_rt_ts;
                        } field;
					} DirectIOPerformanceChannelHeaderOpcodeRoundTrip, *DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr;
                }
			}
		}
	}
}

#endif
