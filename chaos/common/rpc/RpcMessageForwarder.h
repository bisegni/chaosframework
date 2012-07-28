/*
 *	RPCMessageForwarderInterface.h
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

#ifndef CHAOSFramework_RPCMessageForwarderInterface_h
#define CHAOSFramework_RPCMessageForwarderInterface_h

#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

namespace chaos {
        //! Handle the rpc message forwarding
    /*!
     Abstract class that indetify the implementation as an rpc message forwarder
     */
    class RPCMessageForwarderInterface {
    public:
        /*!
         Submit a pack, all the inromation for forwarding it are already into CDataWrapper
         \param MessageNetworkDestination the information for the message forward to network node
         \param onThisThread if true the message is forwarded in the same thread of the caller
         */
        virtual bool submitRpcPack(CNodeNetworkAddress*, bool onThisThread=false) throw(CException) = 0;
    };
}

#endif
