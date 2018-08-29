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
                chaos::common::data::CDWUniquePtr message;
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
				
                void setMessage(chaos::common::data::CDWUniquePtr _message) {
                    message = MOVE(_message);
                }
                
				chaos::common::data::CDWUniquePtr detachMessage() {
					return MOVE(message);
				}
                
                bool hasMessage() {
                    return message.get() != NULL;
                }
			} NetworkForwardInfo;
            
            typedef ChaosUniquePtr<NetworkForwardInfo> NFIUniquePtr;
            typedef ChaosSharedptr<NetworkForwardInfo> NFISharedPtr;
		}
	}
}

#endif
