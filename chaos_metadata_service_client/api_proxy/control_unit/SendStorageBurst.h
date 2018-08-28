/*
 * Copyright 2012, 03/07/2018 INFN
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

#ifndef __CHAOSFramework__SendStorageBurst_h
#define __CHAOSFramework__SendStorageBurst_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/data/structured/Dataset.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {

                //! Start a stoarge burst on a determinated constor unit
                /*!
                 The stoarge burst a funciton taht permit to momentary enable the istoricization of
                 consotrlnit output dataset on determinate rule
                 */
                class SendStorageBurst:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SendStorageBurst)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(SendStorageBurst)
                public:
                    /*!
                     \param cu_uid the uid of the control unit
                     \param dataset_burst is the information of burst
                     */
                    ApiProxyResult execute(const std::string& cu_uid,
                                           chaos::common::data::structured::DatasetBurst& dataset_burst);
                    /*!
                     \param cu_uid the set of uid of one or more control unit
                     \param dataset_burst is the information of burst
                     */
                    ApiProxyResult execute(const ChaosStringSet& cu_set,
                                           chaos::common::data::structured::DatasetBurst& dataset_burst);
                };
            }
        }
    }
}

#endif /* Delete_h */
