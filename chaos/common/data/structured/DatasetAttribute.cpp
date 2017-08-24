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

#include <chaos/common/data/structured/DatasetAttribute.h>

using namespace chaos::common::data::structured;

#pragma mark DatasetAttribute
DatasetAttribute::DatasetAttribute():
name(),
description(),
direction(chaos::DataType::Bidirectional),
type(chaos::DataType::TYPE_UNDEFINED),
binary_subtype_list(),
max_size(0),
binary_cardinality(0){}

DatasetAttribute::DatasetAttribute(const std::string& attr_name,
                                   const std::string& attr_description,
                                   const chaos::DataType::DataType& attr_type):
name(attr_name),
description(attr_description),
direction(chaos::DataType::Bidirectional),
type(attr_type),
binary_subtype_list(),
max_size(0),
binary_cardinality(0){
    
}

DatasetAttribute::DatasetAttribute(const DatasetAttribute& copy_src):
name(copy_src.name),
description(copy_src.description),
direction(copy_src.direction),
type(copy_src.type),
binary_subtype_list(copy_src.binary_subtype_list),
min_value(copy_src.min_value),
max_value(copy_src.max_value),
default_value(copy_src.default_value),
max_size(copy_src.max_size),
binary_cardinality(copy_src.binary_cardinality){}

DatasetAttribute& DatasetAttribute::operator=(DatasetAttribute const &rhs) {
    name = rhs.name;
    description = rhs.description;
    direction = rhs.direction;
    type = rhs.type;
    binary_subtype_list = rhs.binary_subtype_list;
    binary_cardinality = rhs.binary_cardinality;
    return *this;
};
