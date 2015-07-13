/*
 *	ManageCUType.h
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

#ifndef __CHAOSFramework__ManageCUType__
#define __CHAOSFramework__ManageCUType__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

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
