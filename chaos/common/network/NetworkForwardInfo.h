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
    typedef void (*NetworkErrorHandler)(int errorCode, int tag);
    
    /*!
     Structure used to contain information for
     message forward
     */
    typedef struct {
            //!Define the information ip:port and node to reach a remote chaos node
        CNodeNetworkAddress nodeNetworkInfo;
            //! the message data
        CDataWrapper *rpcMessage;
            //! the error handler
        NetworkErrorHandler handler;
            //!
        int tagIdentifier;
            //! forward tag
        int tagNumber;
    } NetworkForwardInfo;
}

#endif
