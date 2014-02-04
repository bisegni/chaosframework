/*
 *	DirectIOServerDataPack.h
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
