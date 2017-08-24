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

#ifndef __CHAOSFramework_B00648AB_D8BE_4C86_950A_A5588E3850E5_http_adapter_types_h
#define __CHAOSFramework_B00648AB_D8BE_4C86_950A_A5588E3850E5_http_adapter_types_h

#include <string>
#include <vector>
#include <chaos/cu_toolkit/additional_lib/mongoose.h>
#include <chaos/common/data/CDataBuffer.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace http_adapter {
                typedef enum {
                    WorkRequestTypeUnspecified,
                    WorkRequestTypeHttpRequest,
                    WorkRequestTypeWSHandshakeRequest,
                    WorkRequestTypeWSFrame,
                    WorkRequestTypeWSCloseEvent
                } WorkRequestType;
                // This info is passed to the worker thread
                struct WorkRequest {
                    WorkRequestType r_type;
                    mg_connection *nc;
                    std::string uri;
                    //!serialization type issuead on the beginning of the http request
                    std::string s_type;
                    ChaosUniquePtr<chaos::common::data::CDataBuffer> buffer;
                    
                    WorkRequest():
                    r_type(WorkRequestTypeUnspecified),
                    nc(NULL),
                    uri(),
                    s_type(),
                    buffer(){}
                    
                    WorkRequest(const char *ptr,
                                uint32_t size):
                    r_type(WorkRequestTypeUnspecified),
                    nc(NULL),
                    uri(),
                    s_type(),
                    buffer(new chaos::common::data::CDataBuffer(ptr, size, true)){}
                };
                
                
                struct HttpAdapterSetting {
                    uint32_t thread_number;
                    std::string publishing_port;
                };
                
#define CU_EG_HTTP_PUBLISHING_PORT "/cu/eg/http/publishing/port"
            }
        }
    }
}

#endif /* __CHAOSFramework_B00648AB_D8BE_4C86_950A_A5588E3850E5_http_adapter_types_h */
