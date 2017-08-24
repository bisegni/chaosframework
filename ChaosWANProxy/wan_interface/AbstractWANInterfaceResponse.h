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

#ifndef __CHAOSFramework__AbstractWANInterfaceResponse__
#define __CHAOSFramework__AbstractWANInterfaceResponse__

#include <string>
#include <map>
#include <stdint.h>
namespace chaos{
	namespace wan_proxy {
		namespace wan_interface {
			
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_FORBIDDEN 403
#define HTTP_SERVER_ERROR 500
			
			class AbstractWANInterfaceResponse {
			public:
				AbstractWANInterfaceResponse();
				AbstractWANInterfaceResponse(const std::string& content_type);
				virtual ~AbstractWANInterfaceResponse();
				
				/**
				 * Get the data of the response, this will contain headers and
				 * body
				 *
				 * @return string the response data
				 */
				virtual std::map<std::string, std::string>& getHeader();
				
				virtual void addHeaderKeyValue(const std::string& key, const std::string& value);
				
				/**
				 * Gets the response body
				 *
				 * @return string the response body
				 */
				virtual const char * getBody(uint32_t& body_len)=0;
				
				/**
				 * Sets the response code
				 */
				void setCode(int code);
				
				int getCode();
				
			protected:
				int code;
				std::map<std::string, std::string> headers;
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__AbstractWANInterfaceResponse__) */
