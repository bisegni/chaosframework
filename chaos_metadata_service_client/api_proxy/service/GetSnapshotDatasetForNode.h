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

#ifndef __CHAOSFramework__BC1FE42_4591_4D85_AEA4_439B4E66336E_GetSnapshotDatasetForNode_h
#define __CHAOSFramework__BC1FE42_4591_4D85_AEA4_439B4E66336E_GetSnapshotDatasetForNode_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

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
