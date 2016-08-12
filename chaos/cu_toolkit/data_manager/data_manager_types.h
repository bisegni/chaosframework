/*
 *	data_manager_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 26/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_B3386161_D92D_4CDB_A1F5_3ED869DF6624_data_manager_types_h
#define __CHAOSFramework_B3386161_D92D_4CDB_A1F5_3ED869DF6624_data_manager_types_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/structured/Dataset.h>
#include <chaos/common/data/cache/AttributeCache.h>


#include <boost/shared_ptr.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace chaos {
    namespace cu {
        namespace data_manager {
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(common::data::cache::VariableIndexType, ChangedDatasetAttributeIndexVector)
            
            //!define dataset pointer type
            typedef boost::shared_ptr<chaos::common::data::structured::Dataset> DatasetPtr;
            
            //! define the contaner for the dataset within the boost multi index set
            struct DatasetElement {
                friend struct extract_key;
                
                typedef boost::shared_ptr<DatasetElement> DatasetElementPtr;
                //!keep track of ordering id
                unsigned int seq_id;
                
                //dataset pointer
                DatasetPtr dataset;
                
                //dataset value cache
                chaos::common::data::cache::AttributeCache dataset_value_cache;
                
                //!constructor
                DatasetElement(unsigned int _seq_id,
                               DatasetPtr _dataset);
                
                //!pretmi to fetch the changed attribute index
                void getChangedAttributeIndex(ChangedDatasetAttributeIndexVector& changed_index);
                
                //!key accessors for multindix infrastructure
                struct less {
                    bool operator()(const DatasetElementPtr& h1, const DatasetElementPtr& h2);
                };
                
                struct extract_key {
                    typedef std::string result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const DatasetElementPtr &p) const;
                };
                
                struct extract_type {
                    typedef chaos::DataType::DatasetType result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const DatasetElementPtr &p) const;
                };
                
                struct extract_ordered_id {
                    typedef unsigned int result_type;
                    // modify_key() requires return type to be non-const
                    const result_type &operator()(const DatasetElementPtr &p) const;
                };
            };
            
            //tag
            struct DatasetElementTagName{};
            struct DatasetElementTagTypeName{};
            struct DatasetElementTagOrderedId{};
            
            //multi-index set
            typedef boost::multi_index_container<
            DatasetElement::DatasetElementPtr,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::tag<DatasetElementTagOrderedId>,  DatasetElement::extract_ordered_id>,
            boost::multi_index::hashed_unique<boost::multi_index::tag<DatasetElementTagName>,  DatasetElement::extract_key>,
            boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<DatasetElementTagTypeName>,
            boost::multi_index::composite_key<
            DatasetElement::DatasetElementPtr,
            DatasetElement::extract_type,
            DatasetElement::extract_key
            >
            >
            >
            > DatasetElementContainer;
            
            //!priority index and iterator
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagOrderedId>::type                      DECOrderedIndex;
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagOrderedId>::type::iterator            DECOrderedIndexIterator;
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagOrderedId>::type::reverse_iterator    DECOrderedIndexReverseIterator;
            
            //!name index and iterator
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagName>::type                           DECNameIndex;
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagName>::type::iterator                 DECNameIndexIterator;
            
            //!name/type index and iterator
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagTypeName>::type                       DECTypeNameIndex;
            typedef boost::multi_index::index<DatasetElementContainer, DatasetElementTagTypeName>::type::iterator             DECTypeNameIndexIterator;
        }
    }
}

#endif /* __CHAOSFramework_B3386161_D92D_4CDB_A1F5_3ED869DF6624_data_manager_types_h */
