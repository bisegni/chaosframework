//
//  DirectIOServerDataPack.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 02/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DirectIOServerDataPack__
#define __CHAOSFramework__DirectIOServerDataPack__

#include <stdint.h>
namespace chaos {
	namespace common {
		namespace direct_io {
            
            //! Contains the data received by the server and permit the reply
            /*!
             The DirectIOServerDataPack is an bastract class that contain the data received 
             by the server and channel referement for the replay information.
             After received the data, the object of this class class (defined by protocoll specific implementation)
             is forwarded to the handlert to be consumed and, if necessary, an answer can be delivered. The rquest/answer ping pong
             need to be regulateted by protocol of the channel.
             */
            class DirectIOServerDataPack {
                
                virtual void getDataPtr() = 0;
                
                virtual uint32_t getDataSize() = 0;
                
                virtual uint32_t sendAnswer(void *data_ptr, uint32_t data_len) = 0;
            };
            
        }
    }
}
#endif /* defined(__CHAOSFramework__DirectIOServerDataPack__) */
