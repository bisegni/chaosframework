/*
 *	RpcSyncServer.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__RpcSyncServer__
#define __CHAOSFramework__RpcSyncServer__


#include <string>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/rpc/RpcServerHandler.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/UrlAccessibleService.h>

namespace chaos {
	namespace common {
		namespace network {
			class NetworkBroker;
		}
		namespace sync_rpc {

			/*
			 Abstract class for standard adapter method for permit, to CommandManager
			 the correct initialization for the adapter instance
			 */
			class RpcSyncServer:
			public chaos::common::utility::StartableService,
			public chaos::common::utility::UrlAccessibleService,
			public chaos::common::utility::NamedService {
				friend class chaos::common::network::NetworkBroker;
			protected:
				int service_port;
				std::string service_url;
				
				RpcServerHandler *command_handler;
				
				/*
				 init the rpc adapter
				 */
				void init(void*) throw(CException);
				
				/*
				 start the rpc adapter
				 */
				void start() throw(CException);

				/*
				 start the rpc adapter
				 */
				void stop() throw(CException);
				
				/*
				 deinit the rpc adapter
				 */
				void deinit() throw(CException);
				
			public:
				RpcSyncServer(const std::string& alias);
				virtual ~RpcSyncServer();
				
				/*!
				 Return the published port
				 */
				int getPublishedPort();
				
				//! inherited method
				const std::string& getUrl();
				
				/*
				 set the command dispatcher associated to the instance of rpc adapter
				 */
				void setCommandDispatcher(RpcServerHandler *new_command_handler);
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__RpcSyncServer__) */
