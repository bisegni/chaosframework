/*
 *	StateFlagCatalog.h
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

#ifndef __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StateFlagCatalog_h
#define __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StateFlagCatalog_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>
#include <chaos/common/state_flag/StateFlag.h>
#include <chaos/common/utility/LockableObject.h>

#include <boost/thread.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace chaos {
    namespace common {
        namespace state_flag {
            
            struct StateFlagElement {
                
                typedef boost::shared_ptr<StateFlagElement> StateFlagElementPtr;
                
                const unsigned int seq_id;
                const std::string flag_name;
                boost::shared_ptr<StateFlag> status_flag;
                
                StateFlagElement(unsigned int _seq_id,
                                 std::string _flag_name,
                                 boost::shared_ptr<StateFlag> _status_flag):
                seq_id(_seq_id),
                flag_name(_flag_name),
                status_flag(_status_flag){}
                bool operator<(const StateFlagElement& src)const{return seq_id<src.seq_id;}
                
                struct less {
                    bool operator()(const StateFlagElementPtr& h1, const StateFlagElementPtr& h2) {
                        return h1->seq_id < h2->seq_id;
                    }
                };
                
                struct extract_key {
                    typedef std::string result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StateFlagElementPtr &p) const
                    {
                        return p->flag_name;
                    }
                };
                
                struct extract_ordered_id {
                    typedef unsigned int result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StateFlagElementPtr &p) const
                    {
                        return p->seq_id;
                    }
                };
                
                struct extract_flag_uuid {
                    typedef std::string result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StateFlagElementPtr &p) const
                    {
                        return p->status_flag->getFlagUUID();
                    }
                };
            };
            
            
            //tag
            struct mitag_ordered{};
            struct mitag_name{};
            struct mitag_flag_uuid{};
            
            //multi-index set
            typedef boost::multi_index_container<
            StateFlagElement::StateFlagElementPtr,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::tag<mitag_ordered>,  StateFlagElement::extract_ordered_id>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<mitag_name>,  StateFlagElement::extract_key>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<mitag_flag_uuid>,  StateFlagElement::extract_flag_uuid>
            >
            > StateFlagElementContainer;
            
            //!priority index and iterator
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_ordered>::type                      StateFlagElementContainerOrderedIndex;
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_ordered>::type::iterator            StateFlagElementContainerOrderedIndexIterator;
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_ordered>::type::reverse_iterator    StateFlagElementContainerOrderedIndexReverseIterator;
            
            //!name index and iterator
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_name>::type                         StateFlagElementContainerNameIndex;
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_name>::type::iterator               StateFlagElementContainerNameIterator;
            
            //uuid
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_flag_uuid>::type                     StateFlagElementContainerFlaUUIDIndex;
            typedef boost::multi_index::index<StateFlagElementContainer, mitag_flag_uuid>::type::iterator           StateFlagElementContainerFlaUUIDIndexItarator;
            
            //!define the mat that will keep track of status field taht the current state repsect the severity for key
            CHAOS_DEFINE_MAP_FOR_TYPE(StateFlagServerity, boost::dynamic_bitset<uint8_t>, MapSeverityBitfield);
            //!define a vector for status flag
            CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<StateFlag>, VectorStateFlag);
            
            //!forward decalration
            class StateFlagCatalogSDWrapper;
            
            //! define a set of status flag with usefull operation on it
            class StateFlagCatalog:
            public StateFlagListener {
                friend class StateFlagCatalogSDWrapper;
                boost::shared_ptr<StateFlag> empty_flag;
                std::string catalog_name;
                //define the catalog where is assigned an unique id to a flag
                mutable chaos::common::utility::LockableObject<StateFlagElementContainer>  catalog_container;
                //mutable boost::shared_mutex mutex_catalog;
                //keep track of changed severity for everi flags
                /*!
                 the map key is the severity, so for each severity
                 a bitfield map with the ordered if of the flag
                 with flags has his current state on severity
                 indicated by the key
                 */
                MapSeverityBitfield map_severity_bf_flag;
                
                //!inherited by @StateFlagCatalog
                void stateFlagUpdated(const FlagDescription     flag_description,
                                      const std::string&        level_name,
                                      const StateFlagServerity  current_level_severity);
                
                void addMemberToSeverityMap(boost::shared_ptr<StateFlag> new_status_flag);
            public:
                StateFlagCatalog();
                StateFlagCatalog(const std::string& _catalog_name);
                StateFlagCatalog(const StateFlagCatalog& _catalog);
                virtual ~StateFlagCatalog();
                
                //!add a new status flag
                void addFlag(const boost::shared_ptr<StateFlag>& flag);
                
                //! set state for flag usign his name
                void setFlagState(const std::string& flag_name, int8_t new_state);
                
                //! set state for flag usign ordered insertion id
                void setFlagState(const unsigned int flag_ordered_id, int8_t new_state);
                
                //!set the state for all alarm
                void setAllFlagState(int8_t new_state);
                
                //!append an intere status flag map into owned one
                void appendCatalog(const StateFlagCatalog& src);
                
                //! return a status flag by name
                boost::shared_ptr<StateFlag>& getFlagByName(const std::string& flag_name);
                
                //!return the flag using the ordered id
                boost::shared_ptr<StateFlag>& getFlagByOrderedID(const unsigned int flag_ordered_id);
                
                //!return all flag that are in a determinated severity
                void getFlagsForSeverity(StateFlagServerity severity,
                                         VectorStateFlag& found_flag);
                
                //!get data buffer describing all flag with own level
                std::auto_ptr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                //!set the falg value base on array description
                void setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer>& raw_level);
                
                //!return the number of status flag in the catalog
                const size_t size() const;

                //!return the max state level of the catalog
                 const uint8_t max() const;
                //!return the name fo the catalog
                const std::string& getName() const;
                
                StateFlagCatalog& operator=(StateFlagCatalog const &rhs);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StateFlagCatalog_h */
