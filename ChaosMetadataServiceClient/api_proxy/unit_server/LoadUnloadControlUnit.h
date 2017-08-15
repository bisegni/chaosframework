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

#ifndef __CHAOSFramework__LoadUnloadControlUnit__
#define __CHAOSFramework__LoadUnloadControlUnit__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace unit_server {

                class LoadUnloadControlUnit:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LoadUnloadControlUnit)
                protected:
                    API_PROXY_CD_DECLARATION(LoadUnloadControlUnit)
                public:

                    /*!
                     Launch the load or unload operation within the metadata service background.
                     \param cu_uid the control unit unique id target of the operation
                     \param load_unload if true the control unit is load false for unload operation
                     */
                    ApiProxyResult execute(const std::string& cu_instance_uid,
                                           const bool load_unload);
                };

            }
        }
    }
}


#endif /* defined(__CHAOSFramework__LoadUnloadControlUnit__) */
