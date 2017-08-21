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

#ifndef __CHAOSFramework__GetSetFullUnitServer__
#define __CHAOSFramework__GetSetFullUnitServer__

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace unit_server {
                
                class GetSetFullUnitServer:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetSetFullUnitServer)
                protected:
                    API_PROXY_CD_DECLARATION(GetSetFullUnitServer)
                public:
                    
                    /*!
                     Initialize an unit server
                     \param unit_server_uid the uid of the unitserver to initialize
                     \param reset  if true remove all unitserver and cu before initialize
                     \param usdesc full key/value description, if null just create the node
                     */
                    ApiProxyResult execute(const std::string& unit_server_uid, int reset,
                                           const chaos::common::data::CDataWrapper* usdesc=NULL);


                    /*!
                       Get the full unitserver
                       \param unit_server_uid the uid of the unitserver to initialize

                     */
                      ApiProxyResult execute(const std::string& unit_server_uid);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetSetFullUnitServer__) */
