/*
 *	AssociationControlUnit.h
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

#ifndef __CHAOSFramework__AssociationControlUnitByDS__
#define __CHAOSFramework__AssociationControlUnitByDS__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <vector>
#include <string>
namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace data_service {

                class AssociationControlUnit:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(AssociationControlUnit)
                protected:
                    API_PROXY_CD_DECLARATION(AssociationControlUnit)

                public:

                    /*!
                     Delete a data service
                     \param ds_unique_id, the unique id of the unit server
                     */
                    ApiProxyResult execute(const std::string& ds_unique_id,
                                           std::vector<std::string> control_unit_to_associate,
                                           bool associate);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AssociationControlUnitByDS__) */
