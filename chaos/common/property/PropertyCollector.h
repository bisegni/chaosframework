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

#ifndef __CHAOSFramework_D9ACDBF9_0C20_44F6_A9EB_7103CDDA87BA_PropertyCollector_h
#define __CHAOSFramework_D9ACDBF9_0C20_44F6_A9EB_7103CDDA87BA_PropertyCollector_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/property/PropertyGroup.h>
namespace chaos {
    namespace common {
        namespace property {
            class PropertyGroup;
            //define a map of property group
            typedef ChaosSharedPtr<PropertyGroup> PropertyGroupShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<PropertyGroup>, PropertyGroupMap);
            
            //!host one or more group of porperty
            class PropertyCollector {
                mutable PropertyGroupMap map_property;
                PropertyValueChangeFunction     value_change_f;
                PropertyValueUpdatedFunction    value_updated_f;
                
                bool changeHandler(const std::string& group_name,
                                   const std::string& property_name,
                                   const chaos::common::data::CDataVariant& property_value);
                
                void updateHandler(const std::string& group_name,
                                   const std::string& property_name,
                                   const chaos::common::data::CDataVariant& old_value,
                                   const chaos::common::data::CDataVariant& new_value);
            public:
                PropertyCollector();
                ~PropertyCollector();
                
                //add a new group
                PropertyGroup& addGroup(const std::string& group_name);
                
                PropertyGroupShrdPtr getGroup(const std::string& group_name) const;
                
                const bool hasGroup(const std::string& group_name) const;
                
                //add a new variable to a group
                void addGroupProperty(const std::string& group_name,
                                      const std::string& property_name,
                                      const std::string& property_description,
                                      const chaos::DataType::DataType property_type,
                                      const uint32_t flag = 0);
                
                void setPropertyValueChangeFunction(const PropertyValueChangeFunction& new_value_change_f);
                
                void setPropertyValueUpdatedFunction(const PropertyValueUpdatedFunction& new_value_updated_f);
                
                void getGroupNames(ChaosStringVector& names);
                
                void applyValue(const PropertyGroupVector& pg_vec) const;
                
                void fillDescription(const std::string& ser_key,
                                     chaos::common::data::CDataWrapper& serialization_pack);
            };
        }
    }
}

#endif /* __CHAOSFramework_D9ACDBF9_0C20_44F6_A9EB_7103CDDA87BA_PropertyCollector_h */
