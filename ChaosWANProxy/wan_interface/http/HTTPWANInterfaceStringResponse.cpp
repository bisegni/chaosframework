/*
 *	HTTPWANInterfaceStringResponse.cpp
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