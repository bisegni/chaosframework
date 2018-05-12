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
                virtual int priorityDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                 chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                
                // Event for a new data received
                virtual int serviceDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractDirectIODispatcher__) */
