/*
 *	GetSetFullUnitServer.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__GetSetFullUnitServer__
#define __CHAOSFramework__GetSetFullUnitServer__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

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
                     \param usdesc full key/value description
                     \param reset  if true remove all unitserver and cu before initialize
                     */
                    ApiProxyResult execute(const std::string& unit_server_uid,
                                           const chaos::common::data::CDataWrapper& usdesc, int reset);


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
