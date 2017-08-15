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

#ifndef __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h
#define __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>

#include <chaos/common/property/PropertyGroup.h>
#include <chaos/common/utility/ObjectInstancer.h>

namespace chaos {
    namespace common {
        namespace trigger {
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::CDataVariant, CDataVariantVector)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::data::CDataVariant, MapKeyCDataVariant)
                
                typedef enum ConsumerResult {
                    //!value has been accepted
                    ConsumerResultOK,
                    ConsumerResultWarinig,
                    ConsumerResultCritical
                } ConsumerResult;
        }
    }
}

#endif /* __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h */
