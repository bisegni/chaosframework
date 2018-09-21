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

#ifndef __CHAOSFramework__DirectIOPerformanceSession__
#define __CHAOSFramework__DirectIOPerformanceSession__

#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIOPerformanceClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIOPerformanceServerChannel.h>

#include <boost/lockfree/queue.hpp>

namespace chaos {
	namespace common{
		namespace message {
			class PerformanceNodeChannel;
		}
		namespace network{
			//forward declaration
			class PerformanceManagment;
		}
		namespace direct_io {
			
			//forward declaration
			class DirectIOClient;
			class DirectIOServerEndpoint;
			
			typedef boost::lockfree::queue< channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr, boost::lockfree::fixed_sized<false> > RttResultQueue;
			
			template<typename T>
			class ResultFetcher {
				boost::lockfree::queue< T, boost::lockfree::fixed_sized<false> > *result_queue_ptr;
			public:
				ResultFetcher(boost::lockfree::queue< T, boost::lockfree::fixed_sized<false> > *_result_queue_ptr):result_queue_ptr(_result_queue_ptr) {}
				
				bool getNext(T& result) {
					return result_queue_ptr->pop(result);
				}
			};
			
			typedef ResultFetcher<channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr> RttResultFetcher;
			
			//! Direct IO performance loop connection
			/*!
			 this class permit to realize a direct io loop to make some bench test between two node
			 using direct io system.
			 */
			class DirectIOPerformanceSession :
			protected utility::InizializableService,
			protected  channel::DirectIOPerformanceServerChannel::DirectIOPerformanceServerChannelHandler {
				friend class chaos::common::network::PerformanceManagment;
				friend class chaos::common::message::PerformanceNodeChannel;
				
				std::string server_description;
				
				DirectIOClientConnection	*client_connection;
				DirectIOServerEndpoint		*server_endpoint;
				
				channel::DirectIOPerformanceClientChannel *client_channel;
				channel::DirectIOPerformanceServerChannel *server_channel;

				//chaos::WaitSemaphore rtt_request_wait_sema;
				RttResultQueue rtt_queue;
				//channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_answer;
			protected:
				void handleReqRoundTripRequest(channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request);
				void handleRespRoundTripRequest(channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request);
				
			public:
				DirectIOPerformanceSession(DirectIOClientConnection *_client_connection, DirectIOServerEndpoint *_server_endpoint);
				~DirectIOPerformanceSession();
				void setConnectionHandler(DirectIOClientConnectionEventHandler *event_handler);
				// Initialize instance
				void init(void *init_data);
				
				// Deinit the implementation
				void deinit();
				
				int64_t sendRttTest(uint32_t ms_timeout = 1000);
				
				RttResultFetcher *getRttResultQueue();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOPerformanceSession__) */
