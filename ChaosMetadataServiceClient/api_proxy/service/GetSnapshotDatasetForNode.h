/*
 *	GetSnapshotDatasetForNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__BC1FE42_4591_4D85_AEA4_439B4E66336E_GetSnapshotDatasetForNode_h
#define __CHAOSFramework__BC1FE42_4591_4D85_AEA4_439B4E66336E_GetSnapshotDatasetForNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                //! Reset all metadata service backend database
                class GetSnapshotDatasetForNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetSnapshotDatasetForNode)
                protected:
                    API_PROXY_CD_DECLARATION(GetSnapshotDatasetForNode)
                public:
                    
                    ApiProxyResult execute(const std::string& snapshot_name,
                                           const std::string& node_uid);
                    
                    static void getAsMap(chaos::common::data::CDataWrapper& api_result,
                                         chaos::common::data::VectorStrCDWShrdPtr& map_dataset);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__BC1FE42_4591_4D85_AEA4_439B4E66336E_GetSnapshotDatasetForNode_h */
