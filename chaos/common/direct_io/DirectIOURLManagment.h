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

namespace chaos {
	namespace common {
		namespace direct_io {
			
			class DirectIOURLManagment {
			public:
				
				bool decodeServerDescriptionWithEndpoint(const std::string& server_description_endpoint,
														 std::string& server_description,
														 uint16_t& endpoint);
				
				bool decoupleServerDescription(const std::string& server_desc,
											   std::string& priority_desc,
											   std::string& service_desc);
				
				bool decoupleServerDescription(const std::string& server_desc,
											   std::vector<std::string>& servers_desc);
				
				static bool checkURL(const std::string& url);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOURLManagment__) */
