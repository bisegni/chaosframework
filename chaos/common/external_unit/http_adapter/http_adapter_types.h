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
#include <chaos/common/data/CDataBuffer.h>

namespace chaos{
    namespace common {
        namespace external_unit {
            namespace http_adapter {
                typedef enum {
                    WorkRequestTypeUnspecified,
                    WorkRequestTypeWSFrame
                } WorkRequestType;
                
                struct HTTPServerAdapterSetting {
                    uint32_t thread_number;
                    std::string publishing_port;
                };
                
#define OPT_HTTP_PUBLISHING_PORT "/chaos/common/external_unit/http/publishing/port"
            }
        }
    }
}

#endif /* __CHAOSFramework_B00648AB_D8BE_4C86_950A_A5588E3850E5_http_adapter_types_h */
