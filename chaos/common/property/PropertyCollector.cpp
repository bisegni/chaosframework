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
#include <chaos/common/chaos_constants.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::property;

PropertyCollector::PropertyCollector() {}

PropertyCollector::~PropertyCollector() {}

PropertyGroup& PropertyCollector::addGroup(const std::string& group_name) {
    if(map_property.count(group_name)) return *map_property[group_name];
    PropertyGroupShrdPtr pg_shrd_ptr(new PropertyGroup(group_name));
    pg_shrd_ptr->setPropertyValueChangeFunction(ChaosBind(&PropertyCollector::changeHandler, this, ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3)));
    pg_shrd_ptr->setPropertyValueUpdatedFunction(ChaosBind(&PropertyCollector::updateHandler, this, ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3), ChaosBindPlaceholder(_4)));
    map_property.insert(PropertyGroupMapPair(group_name, pg_shrd_ptr));
    return *map_property[group_name];
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

PropertyGroupShrdPtr PropertyCollector::getGroup(const std::string& group_name) const {
    if(map_property.count(group_name)) return PropertyGroupShrdPtr();
    return map_property[group_name];
}

const bool PropertyCollector::hasGroup(const std::string& group_name) const {
    return map_property.count(group_name);
}

void PropertyCollector::setPropertyValueChangeFunction(const PropertyValueChangeFunction& new_value_change_f) {
    value_change_f = new_value_change_f;
}

void PropertyCollector::setPropertyValueUpdatedFunction( const PropertyValueUpdatedFunction& new_value_updated_f) {
    value_updated_f = new_value_updated_f;
}

void PropertyCollector::getGroupNames(ChaosStringVector& names) {
    for(PropertyGroupMapIterator it = map_property.begin(),
        end = map_property.end();
        it != end;
        it++) {
        names.push_back(it->first);
    }
}

bool PropertyCollector::changeHandler(const std::string& group_name,
                                      const std::string& property_name,
                                      const chaos::common::data::CDataVariant& property_value) {
    return value_change_f(group_name,
                          property_name,
                          property_value);
}

void PropertyCollector::updateHandler(const std::string& group_name,
                                      const std::string& property_name,
                                      const chaos::common::data::CDataVariant& old_value,
                                      const chaos::common::data::CDataVariant& new_value) {
    value_updated_f(group_name,
                    property_name,
                    old_value,
                    new_value);
}

void PropertyCollector::applyValue(const PropertyGroupVector& pg_vec) const {
    for(PropertyGroupVectorConstIterator it = pg_vec.begin(),
        end = pg_vec.end();
        it != end;
        it++) {
        const std::string& pg_name = it->getGroupName();
        if(map_property.count(pg_name) == 0) continue;
        map_property[pg_name]->updatePropertiesValueFromSourceGroup(*it);
    }
}

void PropertyCollector::fillDescription(const std::string& ser_key,
                                        CDataWrapper& serialization_pack) {
    PropertyGroupVectorSDWrapper pgv_sdw;
    pgv_sdw.serialization_key = ser_key;
    for(PropertyGroupMapIterator it = map_property.begin(),
        end = map_property.end();
        it != end;
        it++) {
        pgv_sdw().push_back(*it->second);
    }
    pgv_sdw.serialize()->copyAllTo(serialization_pack);
}
