/*
 *	DirectIOTypes.h
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
#ifndef CHAOSFramework_DirectIOTypes_h
#define CHAOSFramework_DirectIOTypes_h

#include <string>
#include <chaos/common/network/CNodeNetworkAddress.h>
namespace chaos {
	namespace common {
		//! Namespace that enclose all direct io infrastructure.
		/*!
			DirectIO infrastructure is an unidirectional double channel that permit to exange data between node. Each
			channel connet two node client -> server. A client can connect to more server to send the same pachet
			to every one of these. On the other side each server can accept connection form more client and receive 
			data form everyone. At this layer data noto managed so waht is done with data is not discussed. Here
			is specified how connection two node together.
		 */
		namespace direct_io {
			
			//! Structure to identify the target server of the client connection
			typedef struct DirectIOConnection {
				std::string										connection_name;
				chaos::common::network::CNodeNetworkAddress		server_address;
			} DirectIOCOnnection;
			
            //! Namespace that enclose all type of data flow rule
			namespace DirectIOConnectionModes {
				typedef enum DirectIOConnectionModes{
					DirectIOPoll,
					DirectIOPush
				} DirectIOConnectionModes;
			}
			
            //! Namespace that enclose all type of spreadest of the data forwarding
			namespace DirectIOConnectionSpreadType {
				typedef enum DirectIOConnectionSpreadType{
					DirectIONoSetting,
					DirectIOFailOver,
					DirectIORoundRobin
				} DirectIOConnectionSpreadType;
			}
            
            //! Namespace that enclose all type of the channel
            namespace DirectIOChannelType {
				typedef enum DirectIOChannelType {
					DirectIODataChannel,
					DirectIOServiceChannel
				} DirectIOChannelType;
			}
		}
	}
}

#endif
