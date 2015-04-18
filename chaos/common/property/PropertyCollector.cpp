//
//  PropertyCollector.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/04/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <chaos/common/property/PropertyCollector.h>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/function_output_iterator.hpp>

using namespace chaos::common::property;


struct map_property_extractor
{
    std::vector<std::string>& v;
    map_property_extractor(std::vector<std::string>& _v)
    : v(_v)
    {}
    
    void operator()(AbstractProperty *  property) const
    {
        v.push_back(property->getName());
    }
};

//! add a new property to a section
void PropertyCollector::addPropertyToSection(const std::string& section_name,
                                             AbstractProperty *property_for_section) throw (chaos::CException) {
    if(map_property_sections.count(section_name) == 0) {
        map_property_sections.insert(make_pair(section_name, PropertyMap()));
    }
    if(map_property_sections[section_name].count(property_for_section->getName()) > 0) {
        throw chaos::CException(-1, "Property already registerd", __PRETTY_FUNCTION__);
    }
    map_property_sections[section_name].insert(make_pair(property_for_section->getName(), property_for_section));
}

void PropertyCollector::setSectionPropertyStrValue(const std::string& section,
                                                   const std::string& property_name,
                                                   const std::string& property_value)  throw (chaos::CException) {
    if((map_property_sections.count(section) > 0) &&
       (map_property_sections[section].count(property_name) > 0)){
        map_property_sections[section][property_name]->setStrValue(property_value);
    }
}

std::string PropertyCollector::getSectionPropertyStrValue(const std::string& section,
                                                          const std::string& property_name) {
    if((map_property_sections.count(section) > 0) &&
       (map_property_sections[section].count(property_name) > 0)){
        return map_property_sections[section][property_name]->getStrValue();
    } else {
        return std::string();
    }
}

void PropertyCollector::getAllSection(std::vector<std::string>& sections){
    boost::copy(map_property_sections | boost::adaptors::map_keys, std::back_inserter(sections));
}

void PropertyCollector::getAllPropertyForSection(const std::string& section,
                                                 std::vector<std::string>& properties){
    if(map_property_sections.count(section)==0) return;
    
    boost::copy(map_property_sections[section] | boost::adaptors::map_values,
                boost::make_function_output_iterator(map_property_extractor(properties)));
    
}

const AbstractProperty *PropertyCollector::getAbstractProperty(const std::string& section,
                                                               const std::string& property) {
    if(map_property_sections.count(section)==0) return NULL;
    if(map_property_sections[section].count(property)==0) return NULL;
    return map_property_sections[section][property];
}
