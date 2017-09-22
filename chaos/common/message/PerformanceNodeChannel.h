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
#ifndef __CHAOSFramework__PerformanceNodeChannel__
#define __CHAOSFramework__PerformanceNodeChannel__
#include <string>
#include <chaos/common/message/NodeMessageChannel.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/direct_io/DirectIOPerformanceSession.h>
namespace chaos_direct_io = chaos::common::direct_io;

namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOClient;
		}
		namespace message {
			//! Message Channel specialize for metadataserver comunication
			/*!
			 This class represent a message chanel for comunication with a device
			 */
			class PerformanceNodeChannel:
            public MessageChannel {
				friend class chaos::common::network::NetworkBroker;
                
				ChaosUniquePtr<CNetworkAddress> node_network_address;
				chaos_direct_io::DirectIOClient *client_instance;
				chaos_direct_io::DirectIOPerformanceSession *local_performance_session;
			protected:
				//! base constructor
				/*!
				 The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
				 ip:port:cu_node_address(instance):deviceID
				 */
				PerformanceNodeChannel(NetworkBroker *msg_broker,
                                       CNetworkAddress *_node_network_address,
                                       chaos_direct_io::DirectIOClient *_client_instance,
                                       MessageRequestDomainSHRDPtr _new_message_request_domain = MessageRequestDomainSHRDPtr(new MessageRequestDomain()));
				~PerformanceNodeChannel();
			public:
				//Get the performance session for a chaos node
				/*!
				 Get the performance object that permit to check the performance between the requester and the
				 end point node identified by CNodeNetworkAddress
				 */
				int getPerformanceSession(chaos_direct_io::DirectIOPerformanceSession **performance_session_handler, uint32_t ms_timeout = 1000);
				
				//! release the performance session
				int releasePerformanceSession(chaos_direct_io::DirectIOPerformanceSession *performance_session, uint32_t ms_timeout = 1000);

			};
		}
	}
}

#endif /* defined(__CHAOSFramework__PerformanceNodeChannel__) */
