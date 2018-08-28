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

#ifndef __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h
#define __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h

#include <chaos/common/chaos_constants.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>
#include <chaos/common/data/structured/DatasetAttribute.h>

#include <boost/shared_ptr.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                
                typedef ChaosSharedPtr<DatasetAttribute> DatasetAttributePtr;
                
                //! define the contaner for the dataset within the boost multi index set
                struct DatasetAttributeElement {
                    
                    typedef ChaosSharedPtr<DatasetAttributeElement> DatasetAttributeElementPtr;
                    
                    //!keep track of ordering id
                    unsigned int seq_id;
                    
                    //dataset pointer
                    DatasetAttributePtr dataset_attribute;
                    
                    DatasetAttributeElement(unsigned int _seq_id,
                                            DatasetAttributePtr _dataset);
                    
                    struct less {
                        bool operator()(const DatasetAttributeElementPtr& h1, const DatasetAttributeElementPtr& h2);
                    };
                    
                    struct extract_key {
                        typedef std::string result_type;
                        // modify_key() requires return type to be non-const
                        const result_type &operator()(const DatasetAttributeElementPtr &p) const;
                    };
                    
                    struct extract_type {
                        typedef chaos::DataType::DataType result_type;
                        // modify_key() requires return type to be non-const
                        const result_type &operator()(const DatasetAttributeElementPtr &p) const;
                    };
                    
                    struct extract_ordered_id {
                        typedef unsigned int result_type;
                        // modify_key() requires return type to be non-const
                        const result_type &operator()(const DatasetAttributeElementPtr &p) const;
                    };
                };
                
                //tag
                struct DAETagName{};
                struct DAETagOrderedId{};
                
                //multi-index set
                typedef boost::multi_index_container<
                DatasetAttributeElement::DatasetAttributeElementPtr,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::tag<DAETagOrderedId>,  DatasetAttributeElement::extract_ordered_id>,
                boost::multi_index::hashed_unique<boost::multi_index::tag<DAETagName>,  DatasetAttributeElement::extract_key>
                >
                > DatasetAttributeElementContainer;
                
                //!priority index and iterator
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagOrderedId>::type                      DECOrderedIndex;
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagOrderedId>::type::iterator            DECOrderedIndexIterator;
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagOrderedId>::type::reverse_iterator    DECOrderedIndexReverseIterator;
                
                //!name index and iterator
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagName>::type                           DECNameIndex;
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagName>::type::const_iterator           DECNameIndexConstIterator;
                typedef boost::multi_index::index<DatasetAttributeElementContainer, DAETagName>::type::iterator                 DECNameIndexIterator;
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(DatasetAttributePtr ,DatasetPtrVector);
                
                class DatasetSDWrapper;
                
                //! The description of a complete dataset with his attribute and property
                class Dataset {
                    friend class DatasetSDWrapper;
                    //is the name of the dataset
                    std::string                         name;
                    //is the type of the dataset
                    chaos::DataType::DatasetType        type;
                    //is the key that need to be used to searh dataset on the shared memory
                    std::string                         dataset_key;
                    //is the ocmplete list of the attribute of the dataset
                    mutable DatasetAttributeElementContainer    attribute_set;
                public:
                    Dataset();
                    Dataset(const std::string& _name,
                            const chaos::DataType::DatasetType& _type);
                    Dataset(const Dataset& copy_src);
                    Dataset& operator=(Dataset const &rhs);
                    
                    int addAttribute(DatasetAttributePtr new_attribute);
                    
                    DatasetAttributePtr getAttributebyName(const std::string& attr_name);
                    
                    DatasetAttributePtr getAttributebyOrderedID(const unsigned int ordered_id);
                    
                    const unsigned int getAttributeSize() const;
                    
                    bool hasAttribute(const std::string& attr_name) const;
                    
                    const std::string& getDatasetName() const;
                    
                    const chaos::DataType::DatasetType& getDatasetType() const;
                    
                    void setDatasetKey(const std::string& ds_key);
                    
                    const std::string& getDatasetKey() const;
                };
                
                //! define serialization wrapper for dataset type
                CHAOS_OPEN_SDWRAPPER(Dataset)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    //remove all attribute
                    Subclass::dataWrapped().attribute_set.clear();
                    Subclass::dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME, "");
                    Subclass::dataWrapped().type = static_cast<chaos::DataType::DatasetType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TYPE, chaos::DataType::DatasetTypeUndefined));
                    Subclass::dataWrapped().dataset_key = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_SEARCH_KEY, "");
                    if(serialized_data->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST) &&
                       serialized_data->isVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST)) {
                        DatasetAttributeSDWrapper attribute_wrapper;
                        CMultiTypeDataArrayWrapperSPtr attr_vec = serialized_data->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST);
                        for(int idx = 0;
                            idx < attr_vec->size();
                            idx++) {
                            ChaosUniquePtr<chaos::common::data::CDataWrapper> attr_ser(attr_vec->getCDataWrapperElementAtIndex(idx));
                            attribute_wrapper.deserialize(attr_ser.get());
                            Subclass::dataWrapped().attribute_set.insert(DatasetAttributeElement::DatasetAttributeElementPtr(new DatasetAttributeElement((unsigned int)Subclass::dataWrapped().attribute_set.size(),
                                                                                                                                                         DatasetAttributePtr(new DatasetAttribute(attribute_wrapper())))));
                        }
                    }
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME, Subclass::dataWrapped().name);
                    
                    DatasetAttribute dataset_attribute_buf;
                    DatasetAttributeSDWrapper attribute_reference_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(DatasetAttribute, dataset_attribute_buf));
                    DatasetAttributeElementContainer& cnt = Subclass::dataWrapped().attribute_set;
                    DECOrderedIndex& ordered_index = cnt.get<DAETagOrderedId>();
                    for(DECOrderedIndexIterator it = ordered_index.begin(),
                        end = ordered_index.end();
                        it != end;
                        it++) {
                        //set the wrapper as the
                        attribute_reference_wrapper() = *(*it)->dataset_attribute;
                    }
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h */
