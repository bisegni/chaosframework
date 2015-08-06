/*
 *	GetBestEndpoints.h
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

#ifndef __CHAOSFramework__GetBestEndpoints__
#define __CHAOSFramework__GetBestEndpoints__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace data_service {
                
                class GetBestEndpoints:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetBestEndpoints)
                protected:
                    API_PROXY_CD_DECLARATION(GetBestEndpoints)
                public:
                    
                    /*!
                     Return the best avaialbe data services endpoint
                     \param number_of_result is the number of enpoint to return the default is 3
                     */
                    ApiProxyResult execute(int number_of_result = 3);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetBestEndpoints__) */
