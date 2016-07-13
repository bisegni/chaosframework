/*
 *	DeclareStatusFlag.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStatusFlag_h
#define __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStatusFlag_h

#include <chaos/common/status_manager/StatusFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace status_manager {
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(StateLevel, LevelStateVector);
            
            //!helper class for the management of status flag
            /*!
             Expose simplifyed api for creating satus flag
             */
            class DeclareStatusFlag:
            public StatusFlagCatalog {
            public:
                DeclareStatusFlag(const std::string& _catalog_name);
                ~DeclareStatusFlag();
                
                void addBoolFlag(const std::string flag_name,
                                 const std::string flag_description);
                void addLevelFlag(const std::string flag_name,
                                 const std::string flag_description,
                                  const MapFlagStateLevel& map_code_level_state);
            };
            
        }
    }
}

#endif /* __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStatusFlag_h */
