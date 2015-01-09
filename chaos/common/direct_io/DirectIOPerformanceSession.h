/*
 *	DirectIOPerformanceSession.h
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
				void init(void *init_data) throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
				
				int64_t sendRttTest(uint32_t ms_timeout = 1000);
				
				RttResultFetcher *getRttResultQueue();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOPerformanceSession__) */
