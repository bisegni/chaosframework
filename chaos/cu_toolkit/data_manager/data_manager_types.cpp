/*
 *	data_manager_types.cpp
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


#include <chaos/cu_toolkit/data_manager/data_manager_types.h>

using namespace chaos::cu::data_manager;


#pragma mark DatasetElement

DatasetElement::DatasetElement(unsigned int _seq_id,
                               DatasetPtr _dataset):
seq_id(_seq_id),
dataset(_dataset){}

bool DatasetElement::less::operator()(const DatasetElementPtr& h1, const DatasetElementPtr& h2) {
    return h1->seq_id < h2->seq_id;
}

const DatasetElement::extract_key::result_type&
DatasetElement::extract_key::operator()(const DatasetElementPtr &p) const {
    return p->dataset->name;
}

const DatasetElement::extract_type::result_type&
DatasetElement::extract_type::operator()(const DatasetElementPtr &p) const {
    return p->dataset->type;
}

const DatasetElement::extract_ordered_id::result_type&
DatasetElement::extract_ordered_id::operator()(const DatasetElementPtr &p) const {
    return p->seq_id;
}

void DatasetElement::getChangedAttributeIndex(ChangedDatasetAttributeIndexVector& changed_index) {
    return dataset_value_cache.getChangedIndex(changed_index);
}
