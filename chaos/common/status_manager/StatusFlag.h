/*
 *	status_manager_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h
#define __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h

#include <chaos/common//chaos_constants.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace status_manager {
            
            //! define the level of severity of a status flag
            /*!
             Status flag can notify variation on several behaviour of
             node. variation of a state can be identified in normal situazion as
             for example; power supply is on/off or local/remote. But the can also
             identify fault situation that can be critical or warning
             */
            typedef enum StatusFlagServerity{
                StatusFlagServerityOperationl,
                StatusFlagServerityWarning,
                StatusFlagServerityCritical
            }StatusFlagServerity;
            
            
            //! define a single level with the tag and a description
            struct LevelState {
                const std::string description;
                const StatusFlagServerity severity;
                //!keep track of how many times the current level has been detected
                unsigned int occurence;
                
                LevelState();
                LevelState(const std::string& _description,
                           StatusFlagServerity _severity = StatusFlagServerityOperationl);
                LevelState(const LevelState& src);
            };
            
            CHAOS_DEFINE_MAP_FOR_TYPE(int8_t, LevelState, MapFlagLevelState);
            
            //forward declaration
            class StatusFlag;
            
            class StatusFlagListener {
                friend class StatusFlag;
                
                const std::string listener_uuid;
            protected:
                StatusFlagListener();
                virtual ~StatusFlagListener();
                
                virtual void statusFlagUpdated(const std::string flag_uuid) = 0;
                
            public:
                const std::string& getStatusFlagListenerUUID();
            };

            
            //! Status Flag description
            class StatusFlag {
                const std::string flag_uuid;
                //! the name taht identify the flag
                const std::string name;
                //! the compelte description of the flag
                const std::string description;
                //! kep track of the current level
                int8_t current_level;

                //! mantains the mapping from level and the state description of that level
                MapFlagLevelState map_level_tag;
                
                
                StatusFlagListener *listener;
                boost::shared_mutex mutex_listener;
            public:
                StatusFlag(const std::string& _name,
                           const std::string& _description);
                StatusFlag(const StatusFlag& src);
                //! add a new level with level state
                bool addLevel(int8_t level, const LevelState& level_state);
                //!set the current level
                void setCurrentLevel(int8_t _current_level);
                
                int8_t getCurrentLevel() const;

                const LevelState& getCurrentLevelState();
                
                const std::string& getFlagUUID();
                
                void setListener(StatusFlagListener *new_listener);
            };

            
            //! identify a flag that can be expressed as On/off, 0/1, true/false etc
            class StatusFlagBoolState:
            public StatusFlag {
                bool addLevel(int8_t level, const LevelState& level_state);
            public:
                StatusFlagBoolState(const std::string& _name,
                                     const std::string& _description);
                StatusFlagBoolState(const StatusFlagBoolState& src);

                void setState(bool state);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h */
