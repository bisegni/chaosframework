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

#ifndef __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h
#define __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                class GetVariable:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetVariable)
                protected:
                    API_PROXY_CD_DECLARATION(GetVariable)
                public:
                    
                    ApiProxyResult execute(const std::string& variable_name);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_DD2153F3_FDAC_449B_9E83_4033729A18F4_GetVariable_h */
