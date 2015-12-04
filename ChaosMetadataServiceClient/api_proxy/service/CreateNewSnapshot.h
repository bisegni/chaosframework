/*
 *	CreateNewSnapshot.h
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

#ifndef __MetadataServiceClient__CreateNewSnapshot_h
#define __MetadataServiceClient__CreateNewSnapshot_h

#include <chaos/common/chaos_types.h>

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, SnapshotNodeList)
                
                //! Reset all metadata service backend database
                class CreateNewSnapshot:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CreateNewSnapshot)
                protected:
                    API_PROXY_CD_DECLARATION(CreateNewSnapshot)
                public:
                    ApiProxyResult execute(const std::string& snapshot_name,
                                           const SnapshotNodeList& node_list);
                };
            }
        }
    }
}

#endif /* CreateNewSnapshot_h */
