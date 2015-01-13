/*
 *	HTTPWANInterfaceStringResponse.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh <year> INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__HTTPWANInterfaceStringResponse__
#define __CHAOSFramework__HTTPWANInterfaceStringResponse__
#include <sstream>

#include "HTTPWANInterfaceResponse.h"
namespace chaos{
	namespace wan_proxy {
		namespace wan_interface {
			
			class HTTPWANInterfaceStringResponse:
			public std::stringstream,
			public HTTPWANInterfaceResponse {
				std::string buffer;
			public:
				HTTPWANInterfaceStringResponse();
				~HTTPWANInterfaceStringResponse();
				/**
				 * Gets the response body
				 *
				 * @return string the response body
				 */
				const char * getHTTPBody(uint32_t& body_len);
				
				void writeRawHTTPBody(const char *body_ptr, uint32_t body_len);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__HTTPWANInterfaceStringResponse__) */
