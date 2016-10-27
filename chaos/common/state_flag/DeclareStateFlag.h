/*
 *	DeclareStateFlag.h
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

#ifndef __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStateFlag_h
#define __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStateFlag_h

#include <chaos/common/state_flag/StateFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace state_flag {
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(StateLevel, LevelStateVector);
            
            //!helper class for the management of status flag
            /*!
             Expose simplifyed api for creating satus flag
             */
            class DeclareStateFlag:
            public StateFlagCatalog {
            public:
                DeclareStateFlag(const std::string& _catalog_name);
                ~DeclareStateFlag();
                
                void addBoolFlag(const std::string flag_name,
                                 const std::string flag_description,
                                 const StateLevel& off_level,
                                 const StateLevel& on_level);
                void addLevelFlagWithStates(const std::string flag_name,
                                            const std::string flag_description,
                                            const StateLevelContainer& map_code_level_state);
                void addFlag(const std::string flag_name,
                             const std::string flag_description);
                void addStateToFlag(const std::string flag_name,
                                    const StateLevel& new_state);
                void setFlagState(const std::string flag_name,
                                  const int8_t set_state);
            };
            
        }
    }
}

#endif /* __CHAOSFramework_CAE388D2_7D9C_40F5_A3CC_0467A5638A49_DeclareStateFlag_h */
