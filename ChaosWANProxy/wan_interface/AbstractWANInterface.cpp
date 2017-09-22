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
#include "AbstractWANInterface.h"

using namespace chaos::wan_proxy::wan_interface;

AbstractWANInterface::AbstractWANInterface(const std::string& alias):
NamedService(alias),
service_port(0),
handler(NULL){}

AbstractWANInterface::~AbstractWANInterface() {}

// inherited method
void AbstractWANInterface::init(void *init_data) throw(chaos::CException) {
    std::string param_str = (char*)init_data;
    if(!json_reader.parse(param_str, wan_interface_parameter)) {
        throw chaos::CException(-1, "Error reading json parameter", __PRETTY_FUNCTION__);
    }
}

// inherited method
void AbstractWANInterface::deinit() throw(chaos::CException) {
	
}

// inherited method
void AbstractWANInterface::start() throw(chaos::CException) {
	
}

// inherited method
void AbstractWANInterface::stop() throw(chaos::CException) {
	
}

//inherited method
const std::string& AbstractWANInterface::getUrl() {
	return service_url;
}

Json::Value& AbstractWANInterface::getParameter() {
	return wan_interface_parameter;
}

void AbstractWANInterface::setHandler(BaseWANInterfacelHandler *_handler) {
	handler = _handler;
}