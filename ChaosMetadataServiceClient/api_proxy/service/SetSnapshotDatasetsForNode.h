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

#ifndef __CHAOSFramework__5B48DE9_CCDA_4A6C_942B_988EE60EB694_SetSnapshotDatasetsForNode_h
#define __CHAOSFramework__5B48DE9_CCDA_4A6C_942B_988EE60EB694_SetSnapshotDatasetsForNode_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                typedef ChaosSharedPtr<chaos::common::data::CDataWrapper> DatasetValueShrdPtr;
                
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
