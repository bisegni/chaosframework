/*
 *	PropertyGroup.h
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

#ifndef __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h
#define __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>
#include <chaos/common/property/PropertyDescription.h>

namespace chaos {
    namespace common {
        namespace property {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PropertyDescription, MapProperties);
            
            class PropertyGroup {
                chaos::common::data::CDataVariant default_null_value;
            public:
                std::string name;
                MapProperties map_properties;
                PropertyGroup();
                PropertyGroup(const std::string& _name);
                
                void addProperty(const std::string& property_name,
                                 const std::string& property_description,
                                 const DataType::DataType property_type);
                
                void setPropertyValue(const std::string& property_name,
                                      const chaos::common::data::CDataVariant& new_value);
                
                const chaos::common::data::CDataVariant& getPropertyValue(const std::string& property_name);
                
                const std::string& getGroupName() const;
                
                void copyPropertiesFromGroup(const PropertyGroup& src_group);
                
                const MapProperties getAllProperties() const;
                
                void resetProperiesValues();
                
                PropertyGroup& operator()();
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const bool property_value);
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const int32_t property_value);
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const int64_t property_value);
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const double property_value);
                
                PropertyGroup& operator()(const std::string& property_name,
                                          const std::string& property_value);
                
                PropertyGroup& operator()(const std::string& property_name,
                                          chaos::common::data::CDataBuffer *property_value);
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
                    std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> prop_array(serialized_data->getVectorValue("property_g_plist"));
                    
                    for(unsigned int idx = 0;
                        idx < prop_array->size();
                        idx++) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> prop(prop_array->getCDataWrapperElementAtIndex(idx));
                        property_wrapper.deserialize(prop.get());
                        
                        //insert new porperty
                        Subclass::dataWrapped().map_properties.insert(MapPropertiesPair(property_wrapper.dataWrapped().getName(),
                                                                                        property_wrapper.dataWrapped()));
                    }
                    
                }
            }
            
            std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue("property_g_name", Subclass::dataWrapped().name);
                if(Subclass::dataWrapped().map_properties.size()) {
                    
                    PropertyDescription pd;
                    PropertyDescriptionSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyDescription, pd));
                    
                    for(MapPropertiesIterator it = Subclass::dataWrapped().map_properties.begin(),
                        end = Subclass::dataWrapped().map_properties.end();
                        it != end;
                        it++) {
                        property_ref_wrapper.dataWrapped() = it->second;
                        data_serialized->appendCDataWrapperToArray(*property_ref_wrapper.serialize());
                    }
                    
                    //finalize array
                    data_serialized->finalizeArrayForKey("property_g_plist");
                }
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
        }
    }
}

#endif /* __CHAOSFramework__8F92517_5908_4153_8403_2A2EE5E3B18A_PropertyGroup_h */
