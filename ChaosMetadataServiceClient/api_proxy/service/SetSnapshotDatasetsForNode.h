/*
 *	SetSnapshotDatasetsForNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 26/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__5B48DE9_CCDA_4A6C_942B_988EE60EB694_SetSnapshotDatasetsForNode_h
#define __CHAOSFramework__5B48DE9_CCDA_4A6C_942B_988EE60EB694_SetSnapshotDatasetsForNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                typedef boost::shared_ptr<chaos::common::data::CDataWrapper> DatasetValueShrdPtr;
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(DatasetValueShrdPtr, VectorDatasetValue);
                
                //! Reset all metadata service backend database
                class SetSnapshotDatasetsForNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SetSnapshotDatasetsForNode)
                protected:
                    API_PROXY_CD_DECLARATION(SetSnapshotDatasetsForNode)
                public:
                    
                    ApiProxyResult execute(const std::string& snapshot_name,
                                           const std::string& node_uid,
                                           const std::string& dataset_key,
                                           const chaos::common::data::CDataWrapper& dataset_value);
                    
                    ApiProxyResult execute(const std::string& snapshot_name,
                                           const std::string& node_uid,
                                           const VectorDatasetValue& datasets_value_vec);
                    
                    static DatasetValueShrdPtr createDatasetValue(const std::string& dataset_key,
                                                                  const DatasetValueShrdPtr& dataset_value);
                    
                    static DatasetValueShrdPtr createDatasetValue(const std::string& dataset_key,
                                                                  const chaos::common::data::CDataWrapper& dataset_value);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__5B48DE9_CCDA_4A6C_942B_988EE60EB694_SetSnapshotDatasetsForNode_h */
