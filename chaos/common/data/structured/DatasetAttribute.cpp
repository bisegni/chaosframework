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

chaos::common::data::CDataWrapper& operator<<(chaos::common::data::CDataWrapper& dst,const chaos::common::data::structured::DatasetAttribute&src){
                chaos::common::data::structured::DatasetAttributeSDWrapper sd(src);
                dst.addCSDataValue(src.name,*(sd.serialize().get()));
                return dst;

            }

chaos::common::data::structured::DatasetAttribute&  operator>>(chaos::common::data::structured::DatasetAttribute& dst,const chaos::common::data::CDataWrapper& src ){
    
    chaos::common::data::structured::DatasetAttributeSDWrapper sd;
    sd.deserialize((chaos::common::data::CDataWrapper*)&src);
    dst=sd.dataWrapped();
                
    return dst;
}
#pragma mark DatasetAttribute
DatasetAttribute::DatasetAttribute():
name(),
description(),
direction(chaos::DataType::Bidirectional),
type(chaos::DataType::TYPE_UNDEFINED),
unit("NA"),
increment("0"),
binary_subtype_list(),
max_size(0),
binary_cardinality(0){}

DatasetAttribute::DatasetAttribute(const std::string& attr_name,
                                     const std::string& attr_description,
                                     const chaos::DataType::DataType& attr_type,
                                     const chaos::DataType::DataSetAttributeIOAttribute io,
                                     const std::string& _min,
                                     const std::string& _max,
                                     const std::string& _def,
                                     const std::string& _inc,
                                     const std::string& _unit):name(attr_name),
description(attr_description),
direction(io),
type(attr_type),
binary_subtype_list(),
max_size(0),
min_value(_min), 
max_value(_max),
increment(_inc),
default_value(_def),
unit(_unit),
binary_cardinality(0){
                                         
                                     }
DatasetAttribute::DatasetAttribute(const std::string& attr_name,
                                   const std::string& attr_description,
                                   const chaos::DataType::DataType& attr_type):
name(attr_name),
description(attr_description),
direction(chaos::DataType::Bidirectional),
type(attr_type),
binary_subtype_list(),
max_size(0),
unit("NA"),
increment("0"),
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
unit(copy_src.unit),
increment(copy_src.increment),
default_value(copy_src.default_value),
max_size(copy_src.max_size),
binary_cardinality(copy_src.binary_cardinality){}

DatasetAttribute& DatasetAttribute::operator=(DatasetAttribute const &rhs) {
    if(this != &rhs) {
        name = rhs.name;
        description = rhs.description;
        direction = rhs.direction;
        type = rhs.type;
        binary_subtype_list = rhs.binary_subtype_list;
        binary_cardinality = rhs.binary_cardinality;
        min_value=rhs.min_value;
        max_value=rhs.max_value;
        max_size=rhs.max_size;
        unit=rhs.unit;
        increment=rhs.increment;
    }
    return *this;
};
