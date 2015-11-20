/*
 *	CUCommonUtility.h
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

#ifndef __CHAOSFramework__CUCommonUtility_hpp
#define __CHAOSFramework__CUCommonUtility_hpp

#include "../persistence/persistence.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace common {
            
            class CUCommonUtility {
            public:
                static std::auto_ptr<chaos::common::data::CDataWrapper> prepareRequestPackForLoadControlUnit(const std::string& cu_uid,
                                                                                                             chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da);
                
                static void prepareAutoInitAndStartInAutoLoadControlUnit(const std::string& cu_uid,
                                                                         chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                         chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da,
                                                                         chaos::metadata_service::persistence::data_access::DataServiceDataAccess *ds_da,
                                                                         chaos::common::data::CDataWrapper *auto_load_pack);
                
                static std::auto_ptr<chaos::common::data::CDataWrapper> initDataPack(const std::string& cu_uid,
                                                                                     chaos::metadata_service::persistence::data_access::NodeDataAccess *n_da,
                                                                                     chaos::metadata_service::persistence::data_access::ControlUnitDataAccess *cu_da,
                                                                                     chaos::metadata_service::persistence::data_access::DataServiceDataAccess *ds_da);
                
                static std::auto_ptr<chaos::common::data::CDataWrapper> startDataPack(const std::string& cu_uid);
                
                static std::auto_ptr<chaos::common::data::CDataWrapper> stopDataPack(const std::string& cu_uid);
                
                static std::auto_ptr<chaos::common::data::CDataWrapper> deinitDataPack(const std::string& cu_uid);
                
                static std::auto_ptr<chaos::common::data::CDataWrapper> mergeDatasetAttributeWithSetup(chaos::common::data::CDataWrapper *element_in_dataset,
                                                                                                       chaos::common::data::CDataWrapper *element_in_setup);
            };
            
        }
    }
}

#endif /* CUCommonUtility_hpp */
