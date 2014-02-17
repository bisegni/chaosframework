/*
 *	DirectIODataPack.h
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
#ifndef CHAOSFramework_DirectIODataPack_h
#define CHAOSFramework_DirectIODataPack_h

/*
 
 +-------+-------+-------+-------+
 |             length = n        | 32
 +-------+-------+-------+-------+
 |	  route     + ch_id + ch_tag| 32
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
			
#define DIRECT_IO_HEADER_SIZE					12
#define DIRECT_IO_DISPATCHER_HEADER_SIZE		4

#define DIRECT_IO_GET_DISPATCHER_DATA(d)		byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)d));
#define DIRECT_IO_GET_CHANNEL_HEADER_SIZE(d)    byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)((char*)d+4)));
#define DIRECT_IO_GET_CHANNEL_DATA_SIZE(d)		byte_swap<little_endian, host_endian, uint32_t>(*((uint32_t*)((char*)d+8)));
			
#define DIRECT_IO_SET_DISPATCHER_DATA(d)		byte_swap<host_endian, little_endian, uint32_t>(d);
#define DIRECT_IO_SET_CHANNEL_HEADER_SIZE(d)    byte_swap<host_endian, little_endian, uint32_t>(d);
#define DIRECT_IO_SET_CHANNEL_DATA_SIZE(d)		byte_swap<host_endian, little_endian, uint32_t>(d);
			
#define DIRECT_IO_CHANNEL_PART_EMPTY			0
#define DIRECT_IO_CHANNEL_PART_HEADER_ONLY		1
#define DIRECT_IO_CHANNEL_PART_DATA_ONLY		2
#define DIRECT_IO_CHANNEL_PART_HEADER_DATA		3
			

#define DIRECT_IO_SET_CHANNEL_HEADER(pack_ptr, h_ptr, h_size)\
pack_ptr.header.dispatcher_header.fields.channel_part = pack_ptr.header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_HEADER_ONLY;\
pack_ptr.header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(h_size);\
pack_ptr.channel_header_data = h_ptr;

#define DIRECT_IO_SET_CHANNEL_DATA(pack_ptr, d_ptr, d_size)\
pack_ptr.header.dispatcher_header.fields.channel_part = pack_ptr.header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_DATA_ONLY;\
pack_ptr.header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(d_size);\
pack_ptr.channel_data = d_ptr;
			
            //! DirectIO data pack structure. It is write in little endian
            typedef struct DirectIODataPack {
                //! define the length of pack
                struct header {
					union {
						//!header raw data
						uint32_t    raw_data;
						struct dispatcher_header {
							//! destination routing address
							uint16_t	route_addr;
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
