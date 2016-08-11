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

PropertyGroup::PropertyGroup() {}

PropertyGroup::PropertyGroup(const std::string& _name):
name(_name){}

PropertyGroup::PropertyGroup(const PropertyGroup& src):
name(src.name),
map_properties(src.map_properties){}

void PropertyGroup::addProperty(const std::string& property_name,
                                const std::string& property_description,
                                const DataType::DataType property_type) {
    //add property
    map_properties.insert(MapPropertiesPair(property_name, PropertyDescription(property_name,
                                                                               property_description,
                                                                               property_type)));
}

const CDataVariant& PropertyGroup::getPropertyValue(const std::string& property_name) {
    if(map_properties.count(property_name) == 0) return default_null_value;
    return map_properties[property_name].getPropertyValue();
}

PropertyDescription& PropertyGroup::getProperty(const std::string& property_name) {
    return map_properties[property_name];
}

void PropertyGroup::setPropertyValue(const std::string& property_name,
                                     const chaos::common::data::CDataVariant& new_value) {
    if(map_properties.count(property_name) == 0) return;
    map_properties[property_name].updatePropertyValue(new_value);
}

const std::string& PropertyGroup::getGroupName() const {
    return name;
}

void PropertyGroup::copyPropertiesFromGroup(const PropertyGroup& src_group,
                                            bool copy_value) {
    for (MapPropertiesConstIterator it = src_group.map_properties.begin(),
         end = src_group.map_properties.end();
         it != end;
         it++) {
        //!check if variable is present
        if(map_properties.count(it->first) == 0 ) {
            //we can add property
            map_properties.insert(*it);
        }
        
        if(copy_value) {
            //update values for current insert propety
            map_properties[it->first].updatePropertyValue(it->second.getPropertyValue());
        }
    }
}

const MapProperties PropertyGroup::getAllProperties() const {
    return map_properties;
}

void PropertyGroup::resetProperiesValues() {
    for (MapPropertiesIterator it = map_properties.begin(),
         end = map_properties.end();
         it != end;
         it++) {
        it->second.updatePropertyValue(CDataVariant());
    }
}

PropertyGroup& PropertyGroup::operator()() {
    resetProperiesValues();
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         const bool property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         const int32_t property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         const int64_t property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         const double property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         const std::string& property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}

PropertyGroup& PropertyGroup::operator()(const std::string& property_name,
                                         chaos::common::data::CDataBuffer *property_value) {
    setPropertyValue(property_name,
                     CDataVariant(property_value));
    return *this;
}
