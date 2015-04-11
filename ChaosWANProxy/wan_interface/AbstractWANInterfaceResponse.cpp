/*
 *	AbstractWANInterfaceResponse.cpp
 *	!CHAOS
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
#include "AbstractWANInterfaceResponse.h"

using namespace chaos::wan_proxy::wan_interface;

AbstractWANInterfaceResponse::AbstractWANInterfaceResponse():
code(HTTP_OK) {
}

AbstractWANInterfaceResponse::AbstractWANInterfaceResponse(const std::string& content_type):
code(HTTP_OK) {
		headers.insert(make_pair("Content-Type", content_type));
}

AbstractWANInterfaceResponse::~AbstractWANInterfaceResponse() {
}


std::map<std::string, std::string>& AbstractWANInterfaceResponse::getHeader() {
	return headers;
}

void AbstractWANInterfaceResponse::addHeaderKeyValue(const std::string& key,
													 const std::string& value) {
	headers.insert(make_pair(key, value));
}

void AbstractWANInterfaceResponse::setCode(int _code) {
	code = _code;
}

int AbstractWANInterfaceResponse::getCode() {
	return code;
}