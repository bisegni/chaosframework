/*
 * Copyright 2012, 10/10/2017 INFN
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
#include <chaos/common/external_unit/http_adapter/HTTPClientAdapter.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::external_unit::http_adapter;

HTTPClientAdapter::HTTPClientAdapter() {
    
}

HTTPClientAdapter::~HTTPClientAdapter() {
    
}

void HTTPClientAdapter::init(void *init_data) throw (chaos::CException) {
    
}

void HTTPClientAdapter::deinit() throw (chaos::CException) {
    
}

void HTTPClientAdapter::processBufferElement(WorkRequest *request, ElementManagingPolicy& policy) throw(CException) {
    
}

int HTTPClientAdapter::sendDataToConnection(const std::string& connection_identifier,
                         const chaos::common::data::CDBufferUniquePtr data,
                                            const EUCMessageOpcode opcode) {
    return 0;
}

int HTTPClientAdapter::closeConnection(const std::string& connection_identifier) {
    return 0;
}
