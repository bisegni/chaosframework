/*
 *	DirectIOServer.h
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

#ifndef __CHAOSFramework__DirectIOServer__
#define __CHAOSFramework__DirectIOServer__

#include <string>
#include <map>

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>

#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/DirectIODispatcher.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			
			//! Direct IO server base class
			/*!
				This class reppresents the base interface for the operation on direct io input channel,
				The server layer will accept the client connection for the data reception. Server can accept connection
				by different client
				dio_client---> data message -->dio_server
			 */
			class DirectIOServer : public chaos::utility::StartableService, public NamedService  {
				//! handler implementation for the server instance
            protected:
				DirectIODispatcher *handler_impl;
			public:
				DirectIOServer(std::string alias);
				
				virtual ~DirectIOServer();
				
				// Initialize instance
				void init(void *init_data) throw(chaos::CException);
				
				// Start the implementation
				void start() throw(chaos::CException);
				
				// Stop the implementation
				void stop() throw(chaos::CException);
				
				// Deinit the implementation
				void deinit() throw(chaos::CException);
				
				//! Send some data to the server
				void setHandler(DirectIODispatcher *_handler_impl);
				
				//! Remove the handler pointer
				void clearHandler();
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__DirectIOServer__) */
