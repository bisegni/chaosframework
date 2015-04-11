/*
 *	HTTPWANInterfaceRequest.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__HTTPWANInterfaceRequest__
#define __CHAOSFramework__HTTPWANInterfaceRequest__

#include <mongoose.h>

namespace chaos{
	namespace wan_proxy {
		namespace wan_interface {
			namespace http {
				class HTTPWANInterface;
				
				class HTTPWANInterfaceRequest {
					friend class HTTPWANInterface;;
					
					struct mg_connection *connection;
				protected:
					HTTPWANInterfaceRequest(struct mg_connection *_connection);
					~HTTPWANInterfaceRequest();
				public:
					const char * getHeaderKeyValue(const char * header_key);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__HTTPWANInterfaceRequest__) */
