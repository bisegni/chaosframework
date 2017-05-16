/*
 *	NetworkForwardInfo.h
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

#ifndef CHAOSFramework_NetworkForwardInfo_h
#define CHAOSFramework_NetworkForwardInfo_h
#include <stdint.h>
#include <string>
#include <memory>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/network/CNodeNetworkAddress.h>

namespace chaos {
	namespace common {
		namespace network {
			/*!
			 Structure used to contain information for
			 message forward
			 */
			typedef struct NetworkForwardInfo {
                bool is_request;
                bool is_synchronous_request;
				//!Define the information ip:port used to reach a remote chaos network broker
				std::string destinationAddr;
				//! the message data
                std::auto_ptr<chaos::common::data::CDataWrapper> message;
				//! tag returned in all handler call used by emitter
				int64_t tag;
				//! the ndoe id of the sender channel
                std::string sender_node_id;
                //! the sender request count
                uint32_t sender_request_id;
                
				NetworkForwardInfo(bool _is_request,
                                   bool _is_synchronous_request = false):
                is_request(_is_request),
                is_synchronous_request(_is_synchronous_request),
                destinationAddr(),
                message(),
                tag(0),
                sender_node_id(),
                sender_request_id(0){}

                ~NetworkForwardInfo(){}
				
                void setMessage(chaos::common::data::CDataWrapper *_message) {
                    message.reset(_message);
                }
                
				chaos::common::data::CDataWrapper *detachMessage() {
					return message.release();
				}
                
                bool hasMessage() {
                    return message.get() != NULL;
                }
			} NetworkForwardInfo;
		}
	}
}

#endif
