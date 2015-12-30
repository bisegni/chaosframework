/*
 *	AbstractDirectIODispatcher.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__AbstractDirectIODispatcher__
#define __CHAOSFramework__AbstractDirectIODispatcher__

#include <chaos/common/direct_io/DirectIOHandler.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/utility/StartableService.h>

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/lockfree/queue.hpp>

namespace chaos {
	namespace common {
		namespace direct_io {
            class DirectIOServer;
			//boost::function2<void, void*, uint32_t> delegate = priority_service?
           // boost::bind(&DirectIOHandler::serviceDataReceived, handler_impl, _1, _2):
           // boost::bind(&DirectIOHandler::priorityDataReceived, handler_impl, _1, _2);
			//! Default dispatcher for the direct io system
			class DirectIODispatcher:
			public common::direct_io::DirectIOHandler,
			public utility::StartableService {
				friend class DirectIOServer;
				//! struct for fast delegation
				struct EndpointFastDelegation {
					bool enable;
					DirectIOServerEndpoint *endpoint;
				};

				
				//! available endpoint slotc
				EndpointFastDelegation * * endpoint_slot_array;
				
				//!available index queue
				boost::lockfree::queue<unsigned int> available_endpoint_slot;
                
                DirectIOServerPublicInterface *server_public_interface;
			public:
				DirectIODispatcher();
				~DirectIODispatcher();
				// Initialize instance
				void init(void *init_data) throw(chaos::CException);
				
				// Start the implementation
				void start() throw(chaos::CException);
				
				// Stop the implementation
				void stop() throw(chaos::CException);
				
				// Deinit the implementation
				virtual void deinit() throw(chaos::CException);
				
				//! Allocate a new endpoint
				virtual DirectIOServerEndpoint *getNewEndpoint();
				
				//! Relase the endpoint
				void releaseEndpoint(DirectIOServerEndpoint *endpoint_to_release);
				
				// Event for a new data received
				virtual int priorityDataReceived(DirectIODataPack *data_pack,
                                                 DirectIODataPack *synchronous_answer,
                                                 DirectIODeallocationHandler **answer_header_deallocation_handler,
                                                 DirectIODeallocationHandler **answer_data_deallocation_handler);
                
                // Event for a new data received
				virtual int serviceDataReceived(DirectIODataPack *data_pack,
                                                DirectIODataPack *synchronous_answer,
                                                DirectIODeallocationHandler **answer_header_deallocation_handler,
                                                DirectIODeallocationHandler **answer_data_deallocation_handler);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractDirectIODispatcher__) */
