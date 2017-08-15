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

#include <chaos/common/property/PropertyDescription.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::common::data::cache;

//! default constructor
PropertyDescription::PropertyDescription(){
    
}

//! default constructor with the alias of the command
PropertyDescription::PropertyDescription(const std::string& _name,
                                         const std::string& _description,
                                         const DataType::DataType& _type,
                                         const uint32_t _flag):
name(_name),
description(_description),
type(_type),
flag(_flag){}


PropertyDescription::~PropertyDescription() {}

PropertyDescription& PropertyDescription::operator=(PropertyDescription const &rhs) {
    name=rhs.name;
    description=rhs.description;
    type=rhs.type;
    flag=rhs.flag;
    property_values=rhs.property_values;
    return *this;
}

//! return the alias of the command
const std::string& PropertyDescription::getName() const {
    return name;
}

//! return the alias of the command
const std::string& PropertyDescription::getDescription() const {
    return description;
}
const DataType::DataType PropertyDescription::getType() const {
    return type;
}
const uint32_t PropertyDescription::getFlags() const {
    return flag;
}

void PropertyDescription::updatePropertyValue(const CDataVariant& new_property_value) {
    property_values = new_property_value;
}

const chaos::common::data::CDataVariant& PropertyDescription::getPropertyValue() const {
    return property_values;
}
