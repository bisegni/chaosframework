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
