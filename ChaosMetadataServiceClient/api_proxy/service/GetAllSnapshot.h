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
                
                typedef ChaosSharedPtr<SnapshotInformation> SnapshotInformationPtr;
                CHAOS_DEFINE_VECTOR_FOR_TYPE(SnapshotInformationPtr, SnapshotInfoList);
                
                class GetAllSnapshotHelper {
                    SnapshotInfoList snapshot_list;
                public:
                    GetAllSnapshotHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetAllSnapshotHelper();
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
                    
                    static ChaosUniquePtr<GetAllSnapshotHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}


#endif /* GetAllSnapshot_hpp */
