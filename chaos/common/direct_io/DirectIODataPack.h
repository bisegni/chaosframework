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

namespace chaos {
    namespace common {
        namespace direct_io {
            
            //! DirectIO data pack structure. It is write in little endian
            struct DirectIODataPack {
                //! define the length of pack
                uint32_t    dio_pack_len;
                union {
                    uint32_t    dispatcher_raw_data;
                    struct dispatcher_data {
						//! destination routing address
						uint16_t	route_addr;
                        //! channel index
                        uint16_t	channel_idx: 8;
                        //! channel tag
                        uint16_t    channel_tag: 8;
                    }       fields;
                }           header;
                
                //! channel data
                void        *data;
            };
        }
    }
}

#endif
