/*
 *	PropertyGroup.cpp
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

#include <chaos/common/property/PropertyGroup.h>

using namespace chaos::common::data;
using namespace chaos::common::property;

PorpertyGroup::PorpertyGroup() {}

PorpertyGroup::PorpertyGroup(const std::string& _name):
name(_name){}

void PorpertyGroup::addProperty(const std::string& property_name,
                                const std::string& property_description,
                                const DataType::DataType property_type) {
    //add property
    maps_properties.insert(MapPropertiesPair(property_name, PropertyDescription(property_name,
                                                                                property_description,
                                                                                property_type)));
}

const CDataVariant& PorpertyGroup::getPropertyValue(const std::string& property_name) {
    return maps_properties[property_name].getPropertyValue();
}

const std::string& PorpertyGroup::getGroupName() const {
    return name;
}
