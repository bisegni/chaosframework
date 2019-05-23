/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef CHAOSFramework_DirectIODataPack_h
#define CHAOSFramework_DirectIODataPack_h
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/Buffer.hpp>
#include <chaos/common/utility/endianess.h>
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

namespace chaos {
    namespace common {
        namespace direct_io {
			
#define DIRECT_IO_HEADER_SIZE					sizeof(chaos::common::direct_io::DirectIODataPackDispatchHeader_t)
#define DIRECT_IO_DISPATCHER_HEADER_SIZE		10

#define DIRECT_IO_GET_DISPATCHER_DATA(d)		chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint64_t>(*((uint64_t*)d));
#define DIRECT_IO_GET_CHANNEL_HEADER_SIZE(d)    chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint32_t>(d);
#define DIRECT_IO_GET_CHANNEL_DATA_SIZE(d)		chaos::common::utility::byte_swap<chaos::common::utility::little_endian, chaos::common::utility::host_endian, uint32_t>(d);
			
#define DIRECT_IO_SET_DISPATCHER_DATA(d)		chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint64_t>(d);
#define DIRECT_IO_SET_CHANNEL_HEADER_SIZE(d)    chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint32_t>(d);
#define DIRECT_IO_SET_CHANNEL_DATA_SIZE(d)		chaos::common::utility::byte_swap<chaos::common::utility::host_endian, chaos::common::utility::little_endian, uint32_t>(d);
			
#define DIRECT_IO_CHANNEL_PART_EMPTY			0
#define DIRECT_IO_CHANNEL_PART_HEADER_ONLY		1
#define DIRECT_IO_CHANNEL_PART_DATA_ONLY		2
#define DIRECT_IO_CHANNEL_PART_HEADER_DATA		3
			
#define DIRECT_IO_DATAPACK_DISPATCH_HEADER_FROM_ENDIAN(x)\
x->header.dispatcher_header.endianes.field_1 = FROM_LITTLE_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_1);\
x->header.dispatcher_header.endianes.field_2 = FROM_LITTLE_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_2);\
x->header.dispatcher_header.endianes.field_3 = FROM_LITTLE_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_3);\
x->header.dispatcher_header.endianes.field_4 = FROM_LITTLE_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_4);\
x->header.dispatcher_header.endianes.field_5 = FROM_LITTLE_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_5);

#define DIRECT_IO_DATAPACK_DISPATCH_HEADER_TO_ENDIAN(x)\
x->header.dispatcher_header.endianes.field_1 = TO_LITTEL_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_1);\
x->header.dispatcher_header.endianes.field_2 = TO_LITTEL_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_2);\
x->header.dispatcher_header.endianes.field_3 = TO_LITTEL_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_3);\
x->header.dispatcher_header.endianes.field_4 = TO_LITTEL_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_4);\
x->header.dispatcher_header.endianes.field_5 = TO_LITTEL_ENDNS_NUM(uint16_t, x->header.dispatcher_header.endianes.field_5);

#define DIRECT_IO_SET_CHANNEL_HEADER(pack, h_ptr, h_size)\
if(h_ptr && h_size){pack->header.dispatcher_header.fields.channel_part = pack->header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_HEADER_ONLY;\
pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(h_size);\
pack->channel_header_data = h_ptr;}

#define DIRECT_IO_SET_CHANNEL_DATA(pack, d_ptr, d_size)\
if(d_ptr && d_size){pack->header.dispatcher_header.fields.channel_part = pack->header.dispatcher_header.fields.channel_part+DIRECT_IO_CHANNEL_PART_DATA_ONLY;\
pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(d_size);\
pack->channel_data = d_ptr;}\

			
            //! define the length of pack
            typedef struct DirectIODataPackDispatchHeader {
                union {
                    //!header raw data
                    char    raw_data[10];
                    
                    //!field for semplify the dispatch header configuration
                    struct dispatcher_header {
                        //! destination routing address
                        uint16_t    route_addr;
                        //! api error
                        int16_t    err;
                        //! channel index
                        uint16_t    channel_idx: 8;
                        //! channel tag
                        uint16_t    channel_part: 8;
                        //! channel tag
                        uint16_t    channel_opcode: 8;
                        //! check when a request need a synchronous answer
                        uint16_t    synchronous_answer:1;
                        //! cpadding
                        uint16_t    unused: 7;
                        //!used for coutn the current message from client
                        uint16_t    counter;
                    } fields;
                    
                    //!convenient struct for endianes conversion
                    struct endian_header {
                        uint16_t    field_1;
                        int16_t     field_2;
                        uint16_t    field_3;
                        uint16_t    field_4;
                        uint16_t    field_5;
                    } endianes;
                } dispatcher_header;
                
                //! channel header size (if present > 0)
                uint32_t    channel_header_size;
                //! channel data  (if present > 0)
                uint32_t    channel_data_size;
            } DirectIODataPackDispatchHeader_t;
            
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
            struct DirectIODataPack {
                DirectIODataPackDispatchHeader_t header;
				//!ptr to channel header data
                chaos::common::data::BufferSPtr channel_header_data;
				//!ptr to channel data
                chaos::common::data::BufferSPtr channel_data;
                
                DirectIODataPack(){
                    memset(header.dispatcher_header.raw_data, 0, DIRECT_IO_DISPATCHER_HEADER_SIZE);
                    header.channel_header_size = 0;
                    header.channel_data_size = 0;
                }
            };
            
            //! defin esmart pointer for DirectIODataPack
//            typedef ChaosUniquePtr<DirectIODataPack> DirectIODataPackSPtr;
            typedef ChaosSharedPtr<DirectIODataPack> DirectIODataPackSPtr;
            
            //is a single element contained within channel data memory
            /*!
                api that need ot return more single element fo data need to fille channel_data memory with
                element of this type. in this way implementation layer of directio can stream single element
                over network.
             */
            typedef struct DirectIODataPart {
                //! the data included in this part
                ChaosUniquePtr<char> part_data;
                
                //! the lenght of the data
                uint16_t part_len;
            } DirectIODataPart_t;
        }
    }
}

#endif
