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

#ifndef __CHAOSFramework__HTTPWANInterfaceStringResponse__
#define __CHAOSFramework__HTTPWANInterfaceStringResponse__
#include <sstream>

#include "../AbstractWANInterfaceResponse.h"
namespace chaos{
	namespace wan_proxy {
		namespace wan_interface {
            namespace http {
                
                class HTTPWANInterfaceStringResponse:
                public std::stringstream,
                public AbstractWANInterfaceResponse {
                    std::string buffer;
                public:
                    HTTPWANInterfaceStringResponse();
					HTTPWANInterfaceStringResponse(const std::string& content_type);
                    ~HTTPWANInterfaceStringResponse();
                    /**
                     * Gets the response body
                     *
                     * @return string the response body
                     */
                    const char * getBody(uint32_t& body_len);
                    
                    void writeRawHTTPBody(const char *body_ptr, uint32_t body_len);
                };
            }
		}
	}
}

#endif /* defined(__CHAOSFramework__HTTPWANInterfaceStringResponse__) */
