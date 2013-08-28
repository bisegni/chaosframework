/*
 *	NetworkForwardInfo
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

#ifndef CHAOSFramework_NetworkForwardInfo_h
#define CHAOSFramework_NetworkForwardInfo_h
#include <stdint.h>
#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/network/CNodeNetworkAddress.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
	
        //! Pointer function for error handler
    typedef void (*NetworkErrorHandler)(const char *  emitterIdentifier, int64_t tag, ErrorCode::ErrorCode errorCode);
 
        //! Pointer function for finisced operation handler
    typedef void (*NetworkFinischedHandler)(const char *  emitterIdentifier, int64_t tag);
    
    /*!
     Structure used to contain information for
     message forward
     */
    typedef struct {
            //!Define the information ip:port used to reach a remote chaos network broker
        std::string destinationAddr;
            //! the message data
        chaos_data::CDataWrapper *message;
            //! the error handler
        NetworkErrorHandler errorOpHandler;
            //! the handlet to informa the finisched operation
        NetworkFinischedHandler endOpHandler;
            //! the information for the emitter returned in all hadnler call
        const char *emitterIdentifier;
            //! tag returned in all hadnler call used by emitter
        int64_t tag;
    } NetworkForwardInfo;
}

#endif
