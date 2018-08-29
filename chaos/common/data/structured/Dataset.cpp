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
    if(this != &rhs) {
        name = rhs.name;
        dataset_key = rhs.dataset_key;
        type = rhs.type;
        attribute_set = rhs.attribute_set;
    }
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

DatasetAttributePtr Dataset::getAttributebyOrderedID(const unsigned int ordered_id) {
    DECOrderedIndex& ordered_index = attribute_set.get<DAETagOrderedId>();
    DECOrderedIndexIterator it = ordered_index.find(ordered_id);
    if(it == ordered_index.end()) return DatasetAttributePtr();
    return (*it)->dataset_attribute;
}

const unsigned int Dataset::getAttributeSize() const {
    return (unsigned int)attribute_set.size();
}

bool Dataset::hasAttribute(const std::string& attr_name) const {
    DECNameIndex& name_index = attribute_set.get<DAETagName>();
    return (name_index.find(attr_name) != name_index.end());
}


const std::string& Dataset::getDatasetName() const {
    return name;
}

const chaos::DataType::DatasetType& Dataset::getDatasetType() const {
    return type;
}

void Dataset::setDatasetKey(const std::string& ds_key) {
    dataset_key = ds_key;
}

const std::string& Dataset::getDatasetKey() const {
    return dataset_key;
}
#pragma mark DatasetBurst
DatasetBurst::DatasetBurst():
tag(),
value(),
type(chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeUndefined){}

DatasetBurst::DatasetBurst(const DatasetBurst& copy_src):
tag(copy_src.tag),
value(copy_src.value),
type(copy_src.type){}

DatasetBurst& DatasetBurst::operator=(DatasetBurst const &rhs) {
    if(this != &rhs) {
        tag = rhs.tag;
        value = rhs.value;
        type = rhs.type;
    }
    return *this;
}
