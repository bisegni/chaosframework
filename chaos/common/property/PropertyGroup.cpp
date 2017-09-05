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

#include <chaos/common/property/PropertyGroup.h>

using namespace chaos::common::data;
using namespace chaos::common::property;

PropertyGroup::PropertyGroup() {}

PropertyGroup::PropertyGroup(const std::string& _name):
name(_name){}

PropertyGroup::PropertyGroup(const PropertyGroup& src):
name(src.name),
map_properties(src.map_properties),
value_change_function(src.value_change_function),
value_updated_function(src.value_updated_function){}

bool PropertyGroup::addProperty(const std::string& property_name,
                                const std::string& property_description,
                                const DataType::DataType property_type,
                                const uint32_t flag,
                                const CDataVariant& property_default_value) {
    //add property
    if(map_properties.count(property_name) != 0) return false;
    map_properties.insert(MapPropertiesPair(property_name, PropertyDescription(property_name,
                                                                               property_description,
                                                                               property_type,
                                                                               flag,
                                                                               property_default_value)));
    return true;
}

bool PropertyGroup::addProperty(const std::string& property_name,
                                const CDataVariant& property_default_value) {
    //add property
    if(map_properties.count(property_name) != 0) return false;
    map_properties.insert(MapPropertiesPair(property_name, PropertyDescription(property_name,
                                                                               "",
                                                                               DataType::TYPE_UNDEFINED,
                                                                               0,
                                                                               property_default_value)));
    return true;
}

const CDataVariant& PropertyGroup::getPropertyValue(const std::string& property_name) const {
    if(map_properties.count(property_name) == 0) return default_null_value;
    return map_properties[property_name].getPropertyValue();
}

PropertyDescription& PropertyGroup::getProperty(const std::string& property_name) {
    return map_properties[property_name];
}

void PropertyGroup::setPropertyValue(const std::string& property_name,
                                     const chaos::common::data::CDataVariant& new_value) const {
    if(map_properties.count(property_name) == 0) return;
    if(value_change_function) {
        //!check if the value is accepted
        if(value_change_function(name,
                                 property_name,
                                 new_value) == false) return;
    }
    CDataVariant old = map_properties[property_name].getPropertyValue();
    map_properties[property_name].updatePropertyValue(new_value);
    
    //inform the ganged function fo the changed
    if(value_updated_function){value_updated_function(name,
                                                      property_name,
                                                      old,
                                                      map_properties[property_name].getPropertyValue());}
}

void PropertyGroup::setPropertyValueChangeFunction(const PropertyValueChangeFunction& value_change_f) {
    value_change_function = value_change_f;
}

void PropertyGroup::setPropertyValueUpdatedFunction(const PropertyValueUpdatedFunction& value_updated_f) {
    value_updated_function = value_updated_f;
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

void PropertyGroup::updatePropertiesValueFromSourceGroup(const PropertyGroup& src_group) const {
    for (MapPropertiesConstIterator it = src_group.map_properties.begin(),
         end = src_group.map_properties.end();
         it != end;
         it++) {
        //!check if variable is present
        if(map_properties.count(it->first)) {
            //update values for current insert propety
            setPropertyValue(it->first, it->second.getPropertyValue());
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
                                         const CDataVariant& property_value) {
    setPropertyValue(property_name,
                     property_value);
    return *this;
}
