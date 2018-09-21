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

#ifndef CHAOSFramework_RPCMessageForwarderInterface_h
#define CHAOSFramework_RPCMessageForwarderInterface_h
#include <stdint.h>
#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/NetworkForwardInfo.h>

namespace chaos {
	/*!
	 Abstract class that indetify the implementation as an rpc message forwarder
	 */
	class RpcMessageForwarder {
		
	public:
        RpcMessageForwarder(){}
        virtual ~RpcMessageForwarder(){}
		/*!
		 Submit a pack, all the inromation for forwarding it are already into CDataWrapper
		 \param forward_info the information for the message forward to network node
		 \param on_this_thread if true the message is forwarded in the same thread of the caller
		 */
        virtual bool submitMessage(chaos::common::network::NFISharedPtr forward_info,
                                   bool on_this_thread=false)=0;
	};
}

#endif
