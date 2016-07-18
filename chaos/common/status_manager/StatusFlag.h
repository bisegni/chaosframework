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
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
namespace chaos {
    namespace common {
        namespace status_manager {
            
            //!forward decalration
            class SatusFlagCatalog;
            
            //! define the level of severity of a status flag
            /*!
             Status flag can notify variation on several behaviour of
             node. variation of a state can be identified in normal situazion as
             for example; power supply is on/off or local/remote. But the can also
             identify fault situation that can be critical or warning
             */
            typedef enum StatusFlagServerity{
                StatusFlagServerityRegular,
                StatusFlagServerityWarning,
                StatusFlagServerityCritical,
                StatusFlagServerityUndefuned
            }StatusFlagServerity;
            
            class StateLevelSDVWrapper;
            //! define a single level with the tag and a description
            class StateLevel {
                friend class StateLevelSDWrapper;
                //! level value
                int8_t        value;
                //! short name
                std::string   tag;
                //! full description
                std::string   description;
                StatusFlagServerity severity;

            public:
                //!keep track of how many times the current level has been detected
                unsigned int occurence;
                StateLevel();
                StateLevel(const int8_t value,
                           const std::string& _tag,
                           const std::string& _description,
                           StatusFlagServerity _severity = StatusFlagServerityRegular);
                StateLevel(const StateLevel& src);
                
                bool operator< (const StateLevel &right);
                
                StateLevel& operator=(StateLevel const &rhs);
                
                const std::string& getTag() const;
                const std::string& getDescription() const;
                const int8_t getValue() const;
                const StatusFlagServerity getSeverity() const;
                
                struct ExtractValue {
                    typedef int8_t result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StateLevel &p) const {
                        return p.value;
                    }
                };

            };
            
            
            struct ordered_index_tag{};
            
            //multi-index set
            typedef boost::multi_index_container<
            StateLevel,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::tag<ordered_index_tag>, StateLevel::ExtractValue>
            >
            > StateLevelContainer;
            
            //!priority index and iterator
            typedef boost::multi_index::index<StateLevelContainer, ordered_index_tag>  StatusLevelContainerIndex;
            typedef StatusLevelContainerIndex::type                                    StatusLevelContainerOrderedIndex;
            typedef StatusLevelContainerIndex::type::iterator                          StatusLevelContainerOrderedIndexIterator;
            
            typedef StatusLevelContainerIndex::type::const_iterator                          StatusLevelContainerOrderedIndexConstIterator;
            
            struct StateLevelContainerIncrementCounter{
                StateLevelContainerIncrementCounter(){}
                
                void operator()(StateLevel& sl) {
                    sl.occurence++;
                }
            };
            //forward declaration
            class StatusFlag;
            
            //! status flag listener
            class StatusFlagListener {
                friend class StatusFlag;
                
                const std::string listener_uuid;
            protected:
                StatusFlagListener();
                virtual ~StatusFlagListener();
                //!signal the change of the current selected level severity
                virtual void statusFlagUpdated(const std::string& flag_uuid,
                                               const StatusFlagServerity current_level_severity) = 0;
                
            public:
                const std::string& getStatusFlagListenerUUID();
            };
            
            
            CHAOS_DEFINE_SET_FOR_TYPE(StatusFlagListener*, SetListner);
            
            //!forward declaration
            class StatusFlagSDWrapper;
            
            //! Status Flag description
            class StatusFlag {
                friend class StatusFlagSDWrapper;
                //! kep track of the current level
                int8_t current_level;
                boost::shared_mutex mutex_current_level;
                
                //! mantains the mapping from level and the state description of that level
                StateLevelContainer set_levels;
                
                SetListner listener;
                boost::shared_mutex mutex_listener;
                
            protected:
                std::string flag_uuid;
                //! the name taht identify the flag
                std::string name;
                //! the compelte description of the flag
                std::string description;
                
            public:
                StatusFlag();
                StatusFlag(const std::string& _name,
                           const std::string& _description);
                StatusFlag(const StatusFlag& src);
                
                StatusFlag& operator=(StatusFlag const &rhs);
                
                const std::string& getName();
                const std::string& getDescription();
                const std::string& getFlagUUID();
                
                //! add a new level with level state
                bool addLevel(const StateLevel& level_state);
                
                //! add new level from a source map
                bool addLevelsFromSet(const StateLevelContainer& src_set_levels);
                
                //!set the current level
                void setCurrentLevel(int8_t _current_level);
                
                //return the current level of the flag
                int8_t getCurrentLevel() const;
                
                const StateLevel& getCurrentStateLevel();
                
                
                void addListener(StatusFlagListener *new_listener);
                void removeListener(StatusFlagListener *erase_listener);
                void fireToListener();
            };
            
            
            //! identify a flag that can be expressed as On/off, 0/1, true/false etc
            class StatusFlagBoolState:
            public StatusFlag {
                bool addLevel(const StateLevel& level_state);
            public:
                StatusFlagBoolState(const std::string& _name,
                                    const std::string& _description,
                                    const StateLevel& off_state_level,
                                    const StateLevel& on_state_level);
                StatusFlagBoolState(const StatusFlagBoolState& src);
                
                void setState(bool state);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_status_manager_types_h */
