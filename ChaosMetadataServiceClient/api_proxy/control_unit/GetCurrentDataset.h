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

#ifndef __CHAOSFramework__GetCurrentDataset__
#define __CHAOSFramework__GetCurrentDataset__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {

                    //! return the current control unit dataset
                class GetCurrentDataset:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetCurrentDataset)
                protected:
                        //! default constructor
                    API_PROXY_CD_DECLARATION(GetCurrentDataset)
                public:
                        //! perform the start and stop operation for a control unit
                    /*!
                     \param cu_unique_id control unit id for wich we need the dataset
                     */
                    ApiProxyResult execute(const std::string& cu_unique_id);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetCurrentDataset__) */
