//
//  DirectIOHeader.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 30/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIOHeader_h
#define CHAOSFramework_DirectIOHeader_h

/*
 
+-------+-------+-------+-------+
|          DIO_PACK_LEN         | 32
+-------+-------+-------+-------+
| ch_id +      ch_tag           | 8+24
+-------+-------+-------+-------+
|                               |
:         channel data          :
|                               |
+-------+-------+-------+-------+
 */

#define DIO_PACK_LEN_OFFSET     0
#define DIO_CHANNEL_IDX_OFFSET  31

namespace chaos {
    namespace common {
        namespace direct_io {
            
            //! DirectIO data pack structure. It is write in little endian
            struct DirectIODataPack {
                //! define the length of pack
                uint32_t    dio_pack_len;
                
                union {
                    uint32_t    ch_data;
                    struct data {
                        //! channel index
                        uint32_t     channel_idx: 8;
                        
                        //! channel tag
                        uint32_t     channel_tag: 24;
                    }       field;
                }           channel_header;
                
                //! channel data
                void        *channel_data;
            };
        }
    }
}

#endif
