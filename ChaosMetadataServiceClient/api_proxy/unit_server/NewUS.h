/*
 *	NewUS.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__NewUS__
#define __CHAOSFramework__NewUS__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace unit_server {

                class NewUS:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(NewUS)
                protected:
                    API_PROXY_CD_DECLARATION(NewUS)
                public:

                    /*!
                     Return the full description of the unit server
                     \param new_unit_server_uid the unique id of th enew unit server
                     \param desc description string optional
                     */
                    ApiProxyResult execute(const std::string& new_unit_server_uid,const std::string desc="",const chaos::common::data::CDataWrapper*custom=NULL);
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__NewUS__) */
