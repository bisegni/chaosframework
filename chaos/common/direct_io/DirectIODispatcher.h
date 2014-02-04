/*
 *	AbstractDirectIODispatcher.h
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
#ifndef __CHAOSFramework__AbstractDirectIODispatcher__
#define __CHAOSFramework__AbstractDirectIODispatcher__

#include <chaos/common/direct_io/DirectIOHandler.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/utility/StartableService.h>
#include <boost/thread.hpp>
#include <boost/function.hpp>

#define MAX_END_POINT_NUMBER 256

namespace chaos {
	namespace common {
		namespace direct_io {
			

			//boost::function2<void, void*, uint32_t> delegate = priority_service?
           // boost::bind(&DirectIOHandler::serviceDataReceived, handler_impl, _1, _2):
           // boost::bind(&DirectIOHandler::priorityDataReceived, handler_impl, _1, _2);
			//! Default dispatcher for the direct io system
			class DirectIODispatcher : public common::direct_io::DirectIOHandler, public chaos::utility::StartableService {
				struct EndpointFastDelegation {
					DirectIOServerEndpoint *endpoint;
					boost::function<void(DirectIOServerDataPack *)> priority_delegate;
					boost::function<void(DirectIOServerDataPack *)> service_delegate;
				};

				
				//primary_hashtable = (item**)calloc(hashsize(hashpower), sizeof(item**));
				EndpointFastDelegation * * endpoint_hash;
			public:
				// Initialize instance
				void init(void *init_data) throw(chaos::CException);
				
				// Start the implementation
				void start() throw(chaos::CException);
				
				// Stop the implementation
				void stop() throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
				
				
				DirectIOServerEndpoint *getNewEndpoint();
				
				// Event for a new data received
				void priorityDataReceived(DirectIOServerDataPack *);
                
                // Event for a new data received
				void serviceDataReceived(DirectIOServerDataPack *);

			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractDirectIODispatcher__) */
