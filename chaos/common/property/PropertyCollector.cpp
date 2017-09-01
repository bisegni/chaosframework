/*
 * Copyright 2012, 01/09/2017 INFN
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
#include <chaos/common/property/PropertyCollector.h>

using namespace chaos;
using namespace chaos::common::property;

PropertyCollector::PropertyCollector() {}

PropertyCollector::~PropertyCollector() {}

void PropertyCollector::addGroup(const std::string& group_name) {
    if(map_property.count(group_name)) return;
    map_property.insert(PropertyGroupMapPair(group_name, PropertyGroupShrdPtr(new PropertyGroup(group_name))));
}

void PropertyCollector::addGroupProperty(const std::string& group_name,
                                         const std::string& property_name,
                                         const std::string& property_description,
                                         const DataType::DataType property_type,
                                         const uint32_t flag) {
    if(map_property.count(group_name)) return;
    map_property[group_name]->addProperty(property_name,
                                          property_description,
                                          property_type,
                                          flag);
}

void PropertyCollector::setPropertyValueChangeFunction(const std::string& group_name,
                                                       const PropertyValueChangeFunction& value_change_f) {
    if(map_property.count(group_name)) return;
    map_property[group_name]->setPropertyValueChangeFunction(value_change_f);
}

void PropertyCollector::setPropertyValueUpdatedFunction(const std::string& group_name,
                                                        const PropertyValueUpdatedFunction& value_updated_f) {
    if(map_property.count(group_name)) return;
    map_property[group_name]->setPropertyValueUpdatedFunction(value_updated_f);
}

void PropertyCollector::getGroupNames(ChaosStringVector& names) {
    for(PropertyGroupMapIterator it = map_property.begin(),
        end = map_property.end();
        it != end;
        it++) {
        names.push_back(it->first);
    }
}
