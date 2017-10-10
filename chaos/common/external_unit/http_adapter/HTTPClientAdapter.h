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

#ifndef chaos_common_external_unit_http_adapter_HTTPClientAdapter_h
#define chaos_common_external_unit_http_adapter_HTTPClientAdapter_h

#include <chaos/common/external_unit/http_adapter/HTTPBaseAdapter.h>

namespace chaos {
    namespace common {
        namespace external_unit {
            namespace http_adapter {
                
                class HTTPClientAdapter:
                public HTTPBaseAdapter {
                protected:
                    void processBufferElement(WorkRequest *request, ElementManagingPolicy& policy) throw(CException);
                    int sendDataToConnection(const std::string& connection_identifier,
                                             const chaos::common::data::CDBufferUniquePtr data,
                                             const EUCMessageOpcode opcode);
                    int closeConnection(const std::string& connection_identifier);
                public:
                    HTTPClientAdapter();
                    ~HTTPClientAdapter();
                    void init(void *init_data) throw (chaos::CException);
                    void deinit() throw (chaos::CException);
                };
            }
        }
    }
}

#endif /* chaos_common_external_unit_http_adapter_HTTPClientAdapter_h */
