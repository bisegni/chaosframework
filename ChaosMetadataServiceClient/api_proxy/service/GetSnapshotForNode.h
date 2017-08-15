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

#ifndef __CHAOSFramework_BA15D1C2_31C9_47E4_BEED_93B9DAD674CA_GetSnapshotForNode_h
#define __CHAOSFramework_BA15D1C2_31C9_47E4_BEED_93B9DAD674CA_GetSnapshotForNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                class GetSnapshotForNodeHelper {
                    ChaosStringVector shapshot_list;
                public:
                    GetSnapshotForNodeHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetSnapshotForNodeHelper();
                    size_t getSnapshotListSize();
                    const ChaosStringVector& getSnapshotList();
                };
                
                //! Reset all metadata service backend database
                class GetSnapshotForNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetSnapshotForNode)
                protected:
                    API_PROXY_CD_DECLARATION(GetSnapshotForNode)
                public:
                    
                    ApiProxyResult execute(const std::string& snapshot_name);
                    
                    static ChaosUniquePtr<GetSnapshotForNodeHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_BA15D1C2_31C9_47E4_BEED_93B9DAD674CA_GetSnapshotForNode_h */
