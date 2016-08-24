/*
 *	PropertyDescription.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/property/PropertyDescription.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::common::data::cache;

//! default constructor
PropertyDescription::PropertyDescription():
user_ptr(NULL),
name(),
description(),
type(DataType::TYPE_UNDEFINED),
flag(0){}

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

const std::string& PropertyDescription::getName() const {
    return name;
}

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

void PropertyDescription::setUserPtr(void *_user_ptr) {
    user_ptr = _user_ptr;
}

void *PropertyDescription::getUserPtr() {
    return user_ptr;
}
