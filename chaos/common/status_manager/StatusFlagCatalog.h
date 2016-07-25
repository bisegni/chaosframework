/*
 *	StatusFlagCatalog.h
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

#ifndef __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h
#define __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>
#include <chaos/common/status_manager/StatusFlag.h>

#include <boost/thread.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace chaos {
    namespace common {
        namespace status_manager {
            
            struct StatusFlagElement {
                
                typedef boost::shared_ptr<StatusFlagElement> StatusFlagElementPtr;
                
                const unsigned int seq_id;
                const std::string flag_name;
                boost::shared_ptr<StatusFlag> status_flag;
                
                StatusFlagElement(unsigned int _seq_id,
                                  std::string _flag_name,
                                  boost::shared_ptr<StatusFlag> _status_flag):
                seq_id(_seq_id),
                flag_name(_flag_name),
                status_flag(_status_flag){}
                bool operator<(const StatusFlagElement& src)const{return seq_id<src.seq_id;}
                
                struct less {
                    bool operator()(const StatusFlagElementPtr& h1, const StatusFlagElementPtr& h2) {
                        return h1->seq_id < h2->seq_id;
                    }
                };
                
                struct extract_key {
                    typedef std::string result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StatusFlagElementPtr &p) const
                    {
                        return p->flag_name;
                    }
                };
                
                struct extract_ordered_id {
                    typedef unsigned int result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StatusFlagElementPtr &p) const
                    {
                        return p->seq_id;
                    }
                };
                
                struct extract_flag_uuid {
                    typedef std::string result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const StatusFlagElementPtr &p) const
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
            StatusFlagElement::StatusFlagElementPtr,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::tag<mitag_ordered>,  StatusFlagElement::extract_ordered_id>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<mitag_name>,  StatusFlagElement::extract_key>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<mitag_flag_uuid>,  StatusFlagElement::extract_flag_uuid>
            >
            > StatusFlagElementContainer;
            
            //!priority index and iterator
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_ordered>::type                      StatusFlagElementContainerOrderedIndex;
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_ordered>::type::iterator            StatusFlagElementContainerOrderedIndexIterator;
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_ordered>::type::reverse_iterator    StatusFlagElementContainerOrderedIndexReverseIterator;
            
            //!name index and iterator
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_name>::type                         StatusFlagElementContainerNameIndex;
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_name>::type::iterator               StatusFlagElementContainerNameIterator;
            
            //uuid
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_flag_uuid>::type                     StatusFlagElementContainerFlaUUIDIndex;
            typedef boost::multi_index::index<StatusFlagElementContainer, mitag_flag_uuid>::type::iterator           StatusFlagElementContainerFlaUUIDIndexItarator;
            
            //!define the mat that will keep track of status field taht the current state repsect the severity for key
            CHAOS_DEFINE_MAP_FOR_TYPE(StatusFlagServerity, boost::dynamic_bitset<uint8_t>, MapSeverityBitfield);
            //!define a vector for status flag
            CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<StatusFlag>, VectorStatusFlag);
            
            //!forward decalration
            class StatusFlagCatalogSDWrapper;
            
            //! define a set of status flag with usefull operation on it
            class StatusFlagCatalog:
            public StatusFlagListener {
                friend class StatusFlagCatalogSDWrapper;
                std::string catalog_name;
                //define the catalog where is assigned an unique id to a flag
                StatusFlagElementContainer  catalog_container;
                mutable boost::shared_mutex mutex_catalog;
                //keep track of changed severity for everi flags
                /*!
                 the map key is the severity, so for each severity
                 a bitfield map with the ordered if of the flag
                 with flags has his current state on severity
                 indicated by the key
                 */
                MapSeverityBitfield map_severity_bf_flag;
                
                //!inherited by @StatusFlagCatalog
                void statusFlagUpdated(const std::string& flag_uuid,
                                       const StatusFlagServerity current_level_severity);
                
                void addMemberToSeverityMap(boost::shared_ptr<StatusFlag> new_status_flag);
            public:
                StatusFlagCatalog();
                StatusFlagCatalog(const std::string& _catalog_name);
                virtual ~StatusFlagCatalog();
                
                //!add a new status flag
                void addFlag(boost::shared_ptr<StatusFlag> flag);
                
                //!append an intere status flag map into owned one
                void appendCatalog(const StatusFlagCatalog& src);
                
                //! return a status flag by name
                boost::shared_ptr<StatusFlag> getFlagByName(const std::string& flag_name);
                
                //!return the flag using the ordered id
                boost::shared_ptr<StatusFlag> getFlagByOrderedID(const unsigned int flag_ordered_id);
                
                //!return all flag that are in a determinated severity
                void getFlagsForSeverity(StatusFlagServerity severity,
                                         VectorStatusFlag& found_flag);
                
                //!get data buffer describing all flag with own level
                std::auto_ptr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                //!set the falg value base on array description
                void setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer> raw_level);
                
                //!return the number of status flag in the catalog
                const size_t size() const;
                //!return the name fo the catalog
                const std::string& getName() const;
                
                StatusFlagCatalog& operator=(StatusFlagCatalog const &rhs);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h */
