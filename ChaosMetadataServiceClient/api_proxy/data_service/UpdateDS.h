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

#ifndef __CHAOSFramework__UpdateDS__
#define __CHAOSFramework__UpdateDS__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace data_service {

                class UpdateDS:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(UpdateDS)
                protected:
                    API_PROXY_CD_DECLARATION(UpdateDS)
                public:

                    /*!
                     Update an existing dataservice
                     \param ds_unique_id, the unique id of the unit server
                     \param direct_io_address, the DirectIO address
                     \param direct_io_endpoint, the endpoint within the DirectIO
                     */
                    ApiProxyResult execute(const std::string& ds_unique_id,
                                           const std::string& direct_io_address,
                                           int32_t direct_io_endpoint);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UpdateDS__) */
