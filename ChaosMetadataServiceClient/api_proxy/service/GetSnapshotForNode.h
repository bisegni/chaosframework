/*
 *	GetSnapshotForNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/09/2016 INFN, National Institute of Nuclear Physics
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
                    
                    static std::auto_ptr<GetSnapshotForNodeHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_BA15D1C2_31C9_47E4_BEED_93B9DAD674CA_GetSnapshotForNode_h */
