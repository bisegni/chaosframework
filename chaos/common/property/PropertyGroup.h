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

#ifndef __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h
#define __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>
#include <chaos/common/property/PropertyDescription.h>

namespace chaos {
    namespace common {
        namespace property {
            
            //!define the type of funciton called when property need to be updated
            /*!
             the prototype retun a boolan that need to be returned as false if the set of the
             value has not been successfull.
             */
            typedef ChaosFunction<bool(const std::string&,//!group_name
            const std::string&,//!property name
            const chaos::common::data::CDataVariant&//!property value
            )> PropertyValueChangeFunction;
            
            //!function called when new value has been acceppted
            typedef ChaosFunction<void(const std::string&,//!group name
            const std::string&,//!property name
            const chaos::common::data::CDataVariant&, //!old value
            const chaos::common::data::CDataVariant& //!new value
            )> PropertyValueUpdatedFunction;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PropertyDescription, MapProperties);
            
            class PropertyGroup {
                chaos::common::data::CDataVariant default_null_value;
                
                PropertyValueChangeFunction value_change_function;
                PropertyValueUpdatedFunction value_updated_function;
            public:
                std::string name;
                mutable MapProperties map_properties;
                PropertyGroup();
                PropertyGroup(const std::string& _name);
                PropertyGroup(const PropertyGroup& src);
                
                bool addProperty(const std::string& property_name,
                                 const std::string& property_description,
                                 const DataType::DataType property_type,
                                 const uint32_t flag = 0,
                                 const chaos::common::data::CDataVariant& property_default_value = chaos::common::data::CDataVariant());
                
                bool addProperty(const std::string& property_name,
                                 const chaos::common::data::CDataVariant& property_default_value = chaos::common::data::CDataVariant());
                
                void setPropertyValue(const std::string& property_name,
                                      const chaos::common::data::CDataVariant& new_value) const;
                
                void setPropertyValueChangeFunction(const PropertyValueChangeFunction& value_change_f);
                
                void setPropertyValueUpdatedFunction(const PropertyValueUpdatedFunction& value_updated_f);
                
                PropertyDescription& getProperty(const std::string& property_name);
                
                const chaos::common::data::CDataVariant& getPropertyValue(const std::string& property_name) const;
                
                const std::string& getGroupName() const;
                
                void copyPropertiesFromGroup(const PropertyGroup& src_group,
                                             bool copy_value = false);
                
                void updatePropertiesValueFromSourceGroup(const PropertyGroup& src_group) const;
                
                const MapProperties getAllProperties() const;
                
                const bool hasProperty(const std::string& property_name) const;
                
                void resetProperiesValues();
                
                PropertyGroup& operator()();
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const chaos::common::data::CDataVariant& property_value);
            };
            
            CHAOS_OPEN_SDWRAPPER(PropertyGroup)
            void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                if(serialized_data == NULL) return;
                
                //clear map
                Subclass::dataWrapped().map_properties.clear();
                
                Subclass::dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "property_g_name", "");
                if(serialized_data->hasKey("property_g_plist") &&
                   serialized_data->isVectorValue("property_g_plist")){
                    //we have properties
                    PropertyDescriptionSDWrapper property_wrapper;
                    chaos::common::data::CMultiTypeDataArrayWrapperSPtr prop_array = serialized_data->getVectorValue("property_g_plist");
                    
                    for(unsigned int idx = 0;
                        idx < prop_array->size();
                        idx++) {
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> prop(prop_array->getCDataWrapperElementAtIndex(idx));
                        property_wrapper.deserialize(prop.get());
                        Subclass::dataWrapped().map_properties.insert(MapPropertiesPair(property_wrapper.dataWrapped().getName(),
                                                                                        property_wrapper.dataWrapped()));
                    }
                    
                }
            }
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue("property_g_name", Subclass::dataWrapped().name);
                if(Subclass::dataWrapped().map_properties.size()) {
                    for(MapPropertiesIterator it = Subclass::dataWrapped().map_properties.begin(),
                        end = Subclass::dataWrapped().map_properties.end();
                        it != end;
                        it++) {
                        PropertyDescriptionSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyDescription, it->second));
                        data_serialized->appendCDataWrapperToArray(*property_ref_wrapper.serialize());
                    }
                    
                    //finalize array
                    data_serialized->finalizeArrayForKey("property_g_plist");
                }
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
            
            //!define a vector of property group
            CHAOS_DEFINE_VECTOR_FOR_TYPE(PropertyGroup, PropertyGroupVector);
            
            //define the serialization of property vector
            CHAOS_OPEN_SDWRAPPER(PropertyGroupVector)
            std::string serialization_key;
            void deserialize(chaos::common::data::CDataWrapper *serialized_data){
                if(serialized_data == NULL) return;
                Subclass::dataWrapped().clear();
                PropertyGroupSDWrapper pg_sdw;
                const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                if(serialized_data->hasKey(ser_key) &&
                   serialized_data->isVectorValue(ser_key)) {
                    chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue(ser_key);
                    for(int idx = 0;
                        idx < serialized_array->size();
                        idx++) {
                        if(serialized_array->isCDataWrapperElementAtIndex(idx) == false) continue;
                        chaos::common::data::CDWUniquePtr ser_group(serialized_array->getCDataWrapperElementAtIndex(idx));
                        pg_sdw.deserialize(ser_group.get());
                        Subclass::dataWrapped().push_back(pg_sdw());
                    }
                }
            }
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                for(PropertyGroupVectorIterator it = Subclass::dataWrapped().begin(),
                    end = Subclass::dataWrapped().end();
                    it != end;
                    it++) {
                    PropertyGroupSDWrapper pg_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroup, *it));
                    result->appendCDataWrapperToArray(*pg_sdw.serialize());
                }
                result->finalizeArrayForKey(ser_key);
                return result;
            }
            CHAOS_CLOSE_SDWRAPPER();
        }
    }
}

#endif /* __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h */
