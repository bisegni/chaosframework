/*
 *	trigger_system_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h
#define __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>
namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::CDataVariant, CDataVariantVector)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::data::CDataVariant, MapKeyCDataVariant)
            }
        }
    }
}

#endif /* __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h */
