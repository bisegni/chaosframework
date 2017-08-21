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

#ifndef __CHAOSFramework__60474DE_BEBE_4042_B28F_32C131E273B0_StartUploadSession_h
#define __CHAOSFramework__60474DE_BEBE_4042_B28F_32C131E273B0_StartUploadSession_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace deploy {
                
                //!
                class StartUploadSession:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(StartUploadSession)
                protected:
                    API_PROXY_CD_DECLARATION(StartUploadSession)
                public:
                    //!
                    /*!
                     \param
                     */
                    ApiProxyResult execute(const std::string& agent_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__60474DE_BEBE_4042_B28F_32C131E273B0_StartUploadSession_h */
