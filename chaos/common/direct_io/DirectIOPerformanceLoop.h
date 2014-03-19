/*
 *	DirectIOPerformanceLoop.h
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

#ifndef __CHAOSFramework__DirectIOPerformanceLoop__
#define __CHAOSFramework__DirectIOPerformanceLoop__

#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIOPerformanceClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIOPerformanceServerChannel.h>
namespace chaos {
	namespace common{
		namespace network{
			//forward declaration
			class PerformanceManagment;
		}
		namespace direct_io {
			
			//forward declaration
			class DirectIOClient;
			class DirectIOServerEndpoint;
			
			//! Direct IO performance loop connection
			/*!
			 this class permit to realize a direct io loop to make some bench test between two node
			 using direct io system.
			 */
			class DirectIOPerformanceLoop :
			public chaos::utility::InizializableService,
			protected  channel::DirectIOPerformanceServerChannel::DirectIOPerformanceServerChannelHandler {
				friend class chaos::common::network::PerformanceManagment;
				
				std::string server_description;
				
				DirectIOClientConnection	*client_connection;
				DirectIOServerEndpoint		*server_endpoint;
				
				channel::DirectIOPerformanceClientChannel *client_channel;
				channel::DirectIOPerformanceServerChannel *server_channel;

				chaos::WaitSemaphore rtt_request_wait_sema;
				channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_answer;
			protected:
				void handleRoundTripRequest(channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request);
				
			public:
				DirectIOPerformanceLoop(DirectIOClientConnection *_client_connection, DirectIOServerEndpoint *_server_endpoint);
				~DirectIOPerformanceLoop();
				void setConnectionHandler(DirectIOClientConnectionEventHandler *event_handler);
				// Initialize instance
				void init(void *init_data) throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
				
				channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rttTest(uint32_t ms_timeout = 1000);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOPerformanceLoop__) */
