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
#include "HTTPWANInterfaceStringResponse.h"

using namespace chaos::wan_proxy::wan_interface::http;

HTTPWANInterfaceStringResponse::HTTPWANInterfaceStringResponse():
std::stringstream(std::ios::in |
				  std::ios::out){
	
}
HTTPWANInterfaceStringResponse::HTTPWANInterfaceStringResponse(const std::string& content_type):
AbstractWANInterfaceResponse(content_type),
std::stringstream(std::ios::in |
				  std::ios::out){
	
}

HTTPWANInterfaceStringResponse::~HTTPWANInterfaceStringResponse() {
	
}

//std::ios::in | std::ios::out
const char * HTTPWANInterfaceStringResponse::getBody(uint32_t& body_len) {
	const char * result = NULL;
	if(!buffer.size()) {
		buffer = this->str();
	}
	result = buffer.c_str();
	body_len = (uint32_t)buffer.size();
	return result;
}