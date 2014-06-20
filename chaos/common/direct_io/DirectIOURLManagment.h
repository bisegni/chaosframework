/*
 *	DirectIOURLManagment.h
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

#ifndef __CHAOSFramework__DirectIOURLManagment__
#define __CHAOSFramework__DirectIOURLManagment__

#include <string>
#include <vector>
#include <inttypes.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			
			/*!
			 Class for DirectIO URL managment
			 */
			class DirectIOURLManagment {
			public:
				//! given a standard DirectIO URL (host:p_port:s_port:endpoint), the base server description and endpoint are returned
				bool decodeServerDescriptionWithEndpoint(const std::string& server_description_endpoint,
														 std::string& server_description,
														 uint16_t& endpoint);
				
				//! given a standard DirectIO URL (host:p_port:s_port:endpoint), two single url (host:port) are returned
				bool decoupleServerDescription(const std::string& server_desc,
											   std::string& priority_desc,
											   std::string& service_desc);
				
				//! given a standard DirectIO URL (host:p_port:s_port:endpoint), two single url (host:port) are returned
				bool decoupleServerDescription(const std::string& server_desc,
											   std::vector<std::string>& servers_desc);
				
				//! static check method for the enteri DirectIO URL (host:p_port:s_port:endpoint)
				static bool checkURL(const std::string& url);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOURLManagment__) */
