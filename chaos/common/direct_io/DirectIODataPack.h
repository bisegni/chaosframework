/*
 *	DirectIODataPack.h
 *	!CHAOS
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
#ifndef CHAOSFramework_DirectIODataPack_h
#define CHAOSFramework_DirectIODataPack_h

/*
 +-------+-------+-------+-------+
 | route |  part | ch_id | ch_tag| 32
 +-------+-------+-------+-------+
 |                               |
 :         header data           : n-64
 |                               |
 +-------+-------+-------+-------+
 |                               |
 :         channel data          : n-64
 |                               |
 +-------+-------+-------+-------+
 */
#include <chaos/common/utility/endianess.h>
namespace chaos {
    namespace common {
        namespace direct_io {
			
#define DIRECT_IO_HEADER_SIZE					16
#define DIRECT_IO_DISPATCHER_HEADER_SIZE		8

#define DIRECT_IO_GET_DISPATCHER_DATA(d)		chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint64_t>(*((uint64_t*)d));
#define DIRECT_IO_GET_CHANNEL_HEADER_SIZE(d)    chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint32_t>(*((uint32_t*)((char*)d+8)));
#define DIRECT_IO_GET_CHANNEL_DATA_SIZE(d)		chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint32_t>(*((uint32_t*)((char*)d+12)));
			
#define DIRECT_IO_SET_DISPATCHER_DATA(d)		chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint64_t>(d);
#define DIRECT_IO_SET_CHANNEL_HEADER_SIZE(d)    chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint32_t>(d);
#define DIRECT_IO_SET_CHANNEL_DATA_SIZE(d)		chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint32_t>(d);
			
#define DIRECT_IO_CHANNEL_PART_EMPTY			0
#define DIRECT_IO_CHANNEL_PART_HEADER_ONLY		1
#define DIRECT_IO_CHANNEL_PART_DATA_ONLY		2
#define DIRECT_IO_CHANNEL_PART_HEADER_DATA		3
			

#define DIRECT_IO_SET_CHANNEL_HEADER(pack, h_ptr, h_size)\
pack->header.dispatcher_header.fields.channel_part = pack->header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_HEADER_ONLY;\
pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(h_size);\
pack->channel_header_data = h_ptr;

#define DIRECT_IO_SET_CHANNEL_DATA(pack, d_ptr, d_size)\
pack->header.dispatcher_header.fields.channel_part = pack->header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_DATA_ONLY;\
pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(d_size);\
pack->channel_data = d_ptr;
			
			
			typedef struct DirectIOSynchronousAnswer {
				void *answer_data;
				uint32_t answer_size;
			} DirectIOSynchronousAnswer, *DirectIOSynchronousAnswerPtr;

			
            //! DirectIO data pack structure. It is wrote in little endian
			/*!
			 This represent the data pack sent over direct io infrastructure
			 It is composed by an header that transport:
			 1) dispatch infromation, used to send data to the specified endpoint on the
				remote server.
			 2) the channel header size determinate the lenght of the channel specified header.
				Every channel can specify a custom header for custom purphose
			 3) data size is th elenght of the channel data.
			 */
            typedef struct DirectIODataPack {
                //! define the length of pack
                struct DirectIODataPackDispatchHeader {
					union {
						//!header raw data
						uint64_t    raw_data;
						
						//!field for semplify the dispatch header configuration
						struct dispatcher_header {
							//! destination routing address
							uint16_t	route_addr;
							//! check when a request need a synchronous answer
							uint16_t	synchronous_answer:1;
							//! unused padding data
							uint16_t	unused:15;
							//! channel index
							uint16_t	channel_idx: 8;
							//! channel tag
							uint16_t    channel_part: 8;
							//! channel tag
							uint16_t    channel_opcode: 8;
							//! channel tag
							uint16_t    channel_specified_tag: 8;
						} fields;
					} dispatcher_header;
					
					//! channel header size (if present > 0)
					uint32_t    channel_header_size;
					//! channel data  (if present > 0)
					uint32_t	channel_data_size;
				} header;
				//!ptr to channel header data
                void        *channel_header_data;
				//!ptr to channel data
				void        *channel_data;
            } DirectIODataPack;
        }
    }
}

#endif
