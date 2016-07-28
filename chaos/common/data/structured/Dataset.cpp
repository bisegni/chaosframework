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
