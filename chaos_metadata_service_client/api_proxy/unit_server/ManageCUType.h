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

#ifndef __CHAOSFramework__ManageCUType__
#define __CHAOSFramework__ManageCUType__

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace unit_server {
                
                class ManageCUType:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(ManageCUType)
                protected:
                    API_PROXY_CD_DECLARATION(ManageCUType)
                public:
                    
                    /*!
                     Manage the control unit type for an unit server
                     \param control_unit_type the unique id of the unit server where perform the operation
                     \param control_unit_type is the control unit type object of the oepration
                     \param operation   0- will perform the add operation
                                        1- will perform the remove operation
                     */
                    ApiProxyResult execute(const std::string& unit_server_uid,
                                           const std::string& control_unit_type,
                                           uint32_t operation);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ManageCUType__) */
