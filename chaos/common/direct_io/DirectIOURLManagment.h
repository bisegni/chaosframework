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
