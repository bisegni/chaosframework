/*
 *	DatasetAttribute.cpp
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

#include <chaos/common/data/structured/DatasetAttribute.h>

using namespace chaos::common::data::structured;

#pragma mark DatasetAttribute
DatasetAttribute::DatasetAttribute():
name(),
description(),
direction(chaos::DataType::Bidirectional),
type(chaos::DataType::TYPE_UNDEFINED),
binary_subtype_list(),
binary_cardinality(0){}

DatasetAttribute::DatasetAttribute(const std::string& attr_name,
                                   const std::string& attr_description,
                                   const chaos::DataType::DataType& attr_type):
name(attr_name),
description(attr_description),
direction(chaos::DataType::Bidirectional),
type(attr_type),
binary_subtype_list(),
binary_cardinality(0){
    
}

DatasetAttribute::DatasetAttribute(const DatasetAttribute& copy_src):
name(copy_src.name),
description(copy_src.description),
direction(copy_src.direction),
type(copy_src.type),
binary_subtype_list(copy_src.binary_subtype_list),
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
