/*
 *	StatusFlagCatalogSDWrapper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 18/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StatusFlagCatalogSDWrapper_h
#define __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StatusFlagCatalogSDWrapper_h

#include <chaos/common/data/TemplatedDataSDWrapper.h>

#include <chaos/common/status_manager/StatusFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace status_manager {
            
            //!serialization wrapepr for status flag catalog
            CHAOS_DEFINE_TEMPLATED_SDWRAPPER_CLASS(StatusFlagCatalog) {
            public:
                StatusFlagCatalogSDWrapper();
                
                StatusFlagCatalogSDWrapper(const StatusFlagCatalog& copy_source);
                
                StatusFlagCatalogSDWrapper(chaos::common::data::CDataWrapper *serialized_data);
                
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);
            };
        }
    }
}

#endif /* __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StatusFlagCatalogSDWrapper_h */
