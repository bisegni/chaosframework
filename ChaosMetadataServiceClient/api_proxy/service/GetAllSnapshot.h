/*
 *	GetAllSnapshot.h
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

#ifndef __MetadataServiceClient__GetAllSnapshot_h
#define __MetadataServiceClient__GetAllSnapshot_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                struct SnapshotInformation {
                    std::string name;
                    uint64_t    timestamp;
                    uint32_t    work_concurency;
                };
                
                typedef boost::shared_ptr<SnapshotInformation> SnapshotInformationPtr;
                CHAOS_DEFINE_VECTOR_FOR_TYPE(SnapshotInformationPtr, SnapshotInfoList);
                
                class GetAllSnapshotHelper:
                public ApiResultHelper {
                    SnapshotInfoList snapshot_list;
                public:
                    GetAllSnapshotHelper(ApiProxyResult _api_result);
                    ~GetAllSnapshotHelper();
                    
                    int update();
                    size_t getSnapshotListSize();
                    const SnapshotInfoList& getSnapshotInfoList();
                };
                
                //! Reset all metadata service backend database
                class GetAllSnapshot:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetAllSnapshot)
                protected:
                    API_PROXY_CD_DECLARATION(GetAllSnapshot)
                public:
                    
                    ApiProxyResult execute(const std::string& query_filter = std::string());
                    
                    static std::auto_ptr<GetAllSnapshotHelper> getHelper(ApiProxyResult _api_result);
                };
            }
        }
    }
}


#endif /* GetAllSnapshot_hpp */
