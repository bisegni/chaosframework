/*
 *	HTTPRpcSyncServer.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__HTTPRpcSyncServer__
#define __CHAOSFramework__HTTPRpcSyncServer__

#include <chaos/common/sync_rpc/RpcSyncServer.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <mongoose/Server.h>
#include <mongoose/WebController.h>

namespace chaos {
	namespace common {
        namespace data {
            class CDataWrapper;
        }
        
		namespace sync_rpc {
			
			/*
			 Class that implement the Chaos RPC server using HTTP
			 */
			REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(HTTPRpcSyncServer, RpcSyncServer),
            public Mongoose::WebController {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(HTTPRpcSyncServer)
				
				HTTPRpcSyncServer(const string& alias);
				~HTTPRpcSyncServer();
				
                Mongoose::Server http_server;
                
                Mongoose::Response *process(Mongoose::Request &request);
                
                bool handles(string method, string url);
			protected:
				//inherited method
				void init(void*) throw(CException);
				
				//inherited method
				void start() throw(CException);
				
				//inherited method
				void stop() throw(CException);
				
				//inherited method
				void deinit() throw(CException);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__HTTPRpcSyncServer__) */
