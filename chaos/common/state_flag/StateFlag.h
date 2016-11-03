/*
 *	state_flag_types.h
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

#ifndef __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_state_flag_types_h
#define __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_state_flag_types_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/AbstractListenerContainer.h>

#include <boost/thread.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
namespace chaos {
    namespace common {
        namespace state_flag {
            
            //!forward decalration
            class StateFlagCatalog;
            
            //! define the level of severity of a status flag
            /*!
             Status flag can notify variation on several behaviour of
             node. variation of a state can be identified in normal situazion as
             for example; power supply is on/off or local/remote. But the can also
             identify fault situation that can be critical or warning
             */
            typedef enum StateFlagServerity {
                StateFlagServerityRegular,
                StateFlagServerityWarning,
                StateFlagServerityHigh,
                StateFlagServeritySevere,
                StateFlagServerityUndefuned
            }StateFlagServerity;
            
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
                StateFlagServerity severity;
                
            public:
                //!keep track of how many times the current level has been detected
                unsigned int occurence;
                StateLevel();
                StateLevel(const int8_t value,
                           const std::string& _tag,
                           const std::string& _description,
                           StateFlagServerity _severity = StateFlagServerityRegular);
                StateLevel(const StateLevel& src);
                
                bool operator< (const StateLevel &right);
                
                StateLevel& operator=(StateLevel const &rhs);
                
                const std::string& getTag() const;
                const std::string& getDescription() const;
                const int8_t getValue() const;
                const StateFlagServerity getSeverity() const;
                
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
            class StateFlag;
            
            //! status flag listener
            class StateFlagListener:
            public chaos::common::utility::AbstractListener {
                friend class StateFlag;
                const std::string listener_uuid;
            protected:
                StateFlagListener();
                virtual ~StateFlagListener();
                //!signal the change of the current selected level severity
                virtual void stateFlagUpdated(const std::string& flag_uuid,
                                              const std::string& flag_name,
                                              const std::string& level_name,
                                              const StateFlagServerity current_level_severity) = 0;
                
            public:
                const std::string& getStateFlagListenerUUID();
            };
            
            //!forward declaration
            class StateFlagCatalog;
            //!forward declaration
            class StateFlagSDWrapper;
            //!forward decalration
            class StateFlagCatalogSDWrapper;
            
            //! Status Flag description
            class StateFlag:
            public chaos::common::utility::AbstractListenerContainer {
                friend class StateFlagCatalog;
                friend class StateFlagSDWrapper;
                friend class StateFlagCatalogSDWrapper;
                //! kep track of the current level
                int8_t current_level;
                boost::shared_mutex mutex_current_level;
                
                //! mantains the mapping from level and the state description of that level
                mutable StateLevelContainer set_levels;
                
            protected:
                std::string flag_uuid;
                //! the name that identify the flag
                std::string name;
                //! the compelte description of the flag
                std::string description;
                
                void fireToListener(unsigned int fire_code,
                                    chaos::common::utility::AbstractListener *listener_to_fire);
            public:
                StateFlag();
                StateFlag(const std::string& _name,
                          const std::string& _description);
                StateFlag(const StateFlag& src);
                
                StateFlag& operator=(StateFlag const &rhs);
                
                const std::string& getName() const;
                const std::string& getDescription() const;
                const std::string& getFlagUUID() const;
                
                //! add a new level with level state
                bool addLevel(const StateLevel& level_state);
                
                //! add new level from a source map
                bool addLevelsFromSet(const StateLevelContainer& src_set_levels);
                
                //!set the current level
                void setCurrentLevel(int8_t _current_level);
                
                //return the current level of the flag
                int8_t getCurrentLevel() const;
                
                const StateLevel& getCurrentStateLevel() const;
            };
            
            
            //! identify a flag that can be expressed as On/off, 0/1, true/false etc
            class StateFlagBoolState:
            public StateFlag {
                bool addLevel(const StateLevel& level_state);
            public:
                StateFlagBoolState(const std::string& _name,
                                   const std::string& _description,
                                   const StateLevel& off_state_level,
                                   const StateLevel& on_state_level);
                StateFlagBoolState(const StateFlagBoolState& src);
                
                void setState(bool state);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__1AED93A_A280_4ED9_837D_E3A21159FBE1_state_flag_types_h */
