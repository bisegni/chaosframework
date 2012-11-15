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

namespace chaos {
        //! Pointer function for error handler
    typedef void (*NetworkErrorHandler)(const char *  emitterIdentifier, int64_t tag, ErrorCode::ErrorCode errorCode);
 
        //! Pointer function for finisced operation handler
    typedef void (*NetworkFinischedHandler)(const char *  emitterIdentifier, int64_t tag);
    
    /*!
     Structure used to contain information for
     message forward
     \param destinationAddr server:port addres of remote endpoint
     \param message the message coded into key/value semantics
     \param errorOpHandler
     \param endOpHandler
     \param emitterIdentifier
     \param tag
     */
    typedef struct {
            //!Define the information ip:port and node to reach a remote chaos node
        string destinationAddr;
            //! the message data
        CDataWrapper *message;
            //! the error handler
        NetworkErrorHandler errorOpHandler;
            //! the handlet to informa the finisched operation
        NetworkFinischedHandler endOpHandler;
            //! the information for the emitter to be recognized itself
        const char *emitterIdentifier;
            //! forward tag for the miteer to recognize the kind of his operation
        int64_t tag;
    } NetworkForwardInfo;
}

#endif
