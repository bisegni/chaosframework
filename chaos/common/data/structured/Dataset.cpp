/*
 *	Dataset.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/structured/Dataset.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;

#pragma mark DatasetAttributeElement
DatasetAttributeElement::DatasetAttributeElement(unsigned int _seq_id,
                                                 DatasetAttributePtr _dataset_attribute):
seq_id(_seq_id),
dataset_attribute(_dataset_attribute){}

bool DatasetAttributeElement::less::operator()(const DatasetAttributeElementPtr& h1, const DatasetAttributeElementPtr& h2) {
    return h1->seq_id < h2->seq_id;
}

const DatasetAttributeElement::extract_key::result_type&
DatasetAttributeElement::extract_key::operator()(const DatasetAttributeElementPtr &p) const {
    return p->dataset_attribute->name;
}

const DatasetAttributeElement::extract_type::result_type&
DatasetAttributeElement::extract_type::operator()(const DatasetAttributeElementPtr &p) const {
    return p->dataset_attribute->type;
}

const DatasetAttributeElement::extract_ordered_id::result_type&
DatasetAttributeElement::extract_ordered_id::operator()(const DatasetAttributeElementPtr &p) const {
    return p->seq_id;
}

#pragma mark Dataset
Dataset::Dataset(const std::string& _name,
                 const chaos::DataType::DatasetType& _type):
name(_name),
type(_type){}

Dataset::Dataset():
name(),
dataset_key(),
type(chaos::DataType::DatasetTypeUndefined){}

Dataset::Dataset(const Dataset& copy_src):
name(copy_src.name),
dataset_key(copy_src.dataset_key),
type(copy_src.type){}

Dataset& Dataset::operator=(Dataset const &rhs) {
    name = rhs.name;
    dataset_key = rhs.dataset_key;
    type = rhs.type;
    attribute_set = rhs.attribute_set;
    return *this;
}

int Dataset::addAttribute(DatasetAttributePtr new_attribute) {
    DatasetAttributeElement::DatasetAttributeElementPtr ele_ptr(new DatasetAttributeElement((unsigned int)attribute_set.size(),
                                                                                            new_attribute));
    DECNameIndex& name_index = attribute_set.get<DAETagName>();
    DECNameIndexIterator it = name_index.find(new_attribute->name);
    if(it != name_index.end()) return -1;
    attribute_set.insert(ele_ptr);
    return 0;
}

DatasetAttributePtr Dataset::getAttributebyName(const std::string& attr_name) {
    DECNameIndex& name_index = attribute_set.get<DAETagName>();
    DECNameIndexIterator it = name_index.find(attr_name);
    if(it == name_index.end()) return DatasetAttributePtr();
    return (*it)->dataset_attribute;
}

DatasetAttributePtr Dataset::getAttributebyOrderedIDe(const unsigned int ordered_id) {
    DECOrderedIndex& ordered_index = attribute_set.get<DAETagOrderedId>();
    DECOrderedIndexIterator it = ordered_index.find(ordered_id);
    if(it == ordered_index.end()) return DatasetAttributePtr();
    return (*it)->dataset_attribute;
}

#pragma mark DatasetSDWrapper
CHAOS_DEFINE_SD_WRAPPER_CONSTRUCTOR(Dataset)

void DatasetSDWrapper::deserialize(chaos::common::data::CDataWrapper *serialized_data) {
    //remove all attribute
    dataWrapped().attribute_set.clear();
    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME, "");
    dataWrapped().type = static_cast<chaos::DataType::DatasetType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TYPE, chaos::DataType::DatasetTypeUndefined));
    dataWrapped().dataset_key = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_SEARCH_KEY, "");
    if(serialized_data->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST) &&
       serialized_data->isVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST)) {
        DatasetAttributeSDWrapper attribute_wrapper;
        std::auto_ptr<CMultiTypeDataArrayWrapper> attr_vec(serialized_data->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_LIST));
        for(int idx = 0;
            idx < attr_vec->size();
            idx++) {
            std::auto_ptr<CDataWrapper> attr_ser(attr_vec->getCDataWrapperElementAtIndex(idx));
            attribute_wrapper.deserialize(attr_ser.get());
            dataWrapped().attribute_set.insert(DatasetAttributeElement::DatasetAttributeElementPtr(new DatasetAttributeElement((unsigned int)dataWrapped().attribute_set.size(),
                                                                                                                               DatasetAttributePtr(new DatasetAttribute(attribute_wrapper())))));
        }
    }
}

std::auto_ptr<CDataWrapper> DatasetSDWrapper::serialize() {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    DatasetAttributeSDWrapper attribute_wrapper;
    DECOrderedIndex& ordered_index = dataWrapped().attribute_set.get<DAETagOrderedId>();
    for(DECOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++) {
        //set the wrapper as the
        attribute_wrapper() = *(*it)->dataset_attribute;
    }
    return result;
}
