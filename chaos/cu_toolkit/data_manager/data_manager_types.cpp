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
    return p->dataset->getDatasetName();
}

const DatasetElement::extract_type::result_type&
DatasetElement::extract_type::operator()(const DatasetElementPtr &p) const {
    return p->dataset->getDatasetType();
}

const DatasetElement::extract_ordered_id::result_type&
DatasetElement::extract_ordered_id::operator()(const DatasetElementPtr &p) const {
    return p->seq_id;
}

void DatasetElement::getChangedAttributeIndex(ChangedDatasetAttributeIndexVector& changed_index) {
    return dataset_value_cache.getChangedIndex(changed_index);
}
