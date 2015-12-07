/*
 *	RestoreSnapshot.h
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

#ifndef __MetadataServiceClient__RestoreSnapshot_h
#define __MetadataServiceClient__RestoreSnapshot_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                //! Reset all metadata service backend database
                class RestoreSnapshot:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(RestoreSnapshot)
                protected:
                    API_PROXY_CD_DECLARATION(RestoreSnapshot)
                public:
                    
                    ApiProxyResult execute(const std::string& snapshot_name);
                };
            }
        }
    }
}

#endif /* RestoreSnapshot_hpp */
