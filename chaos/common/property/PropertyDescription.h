/*
 *	PropertyDescription.h
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

#ifndef __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h
#define __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace property {
            
            class PropertyDescriptionSDWrapper;
            
            //!define a porperty taht is rapresentated by haos base value
            class PropertyDescription {
                friend class PropertyDescriptionSDWrapper;
                chaos::common::data::CDataVariant  property_values;
            public:
                std::string name;
                std::string description;
                DataType::DataType type;
                uint32_t flag;
                //! default constructor
                PropertyDescription();
                
                //! default constructor with the alias of the command
                PropertyDescription(const std::string& _name,
                                    const std::string& _description,
                                    const DataType::DataType& _type = DataType::TYPE_UNDEFINED,
                                    const uint32_t _flag = 0);
                
                
                ~PropertyDescription();
                
                PropertyDescription& operator=(PropertyDescription const &rhs);
                
                //! return the alias of the command
                const std::string& getName() const;
                
                //! return the alias of the command
                const std::string& getDescription() const;
                
                const DataType::DataType getType() const;
                
                const uint32_t getFlags() const;
                
                void updatePropertyValue(const chaos::common::data::CDataVariant& new_property_value);
                
                const chaos::common::data::CDataVariant& getPropertyValue() const;
            };
            
            
            //!helper for create or read the script description
            CHAOS_OPEN_SDWRAPPER(PropertyDescription)
            void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                if(serialized_data == NULL) return;
                Subclass::dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "property_name", "");
                Subclass::dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "property_description", "");
                Subclass::dataWrapped().type = (DataType::DataType)CDW_GET_INT32_WITH_DEFAULT(serialized_data, "property_type", 0);
                Subclass::dataWrapped().flag = (uint32_t)CDW_GET_INT32_WITH_DEFAULT(serialized_data, "property_flag", 0);
                if(serialized_data->hasKey("property_value")){
                    Subclass::dataWrapped().property_values = serialized_data->getVariantValue("property_value");
                }
            }
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue("property_name", Subclass::dataWrapped().name);
                data_serialized->addStringValue("property_description", Subclass::dataWrapped().description);
                data_serialized->addInt32Value("property_type", Subclass::dataWrapped().type);
                data_serialized->addInt32Value("property_flag", Subclass::dataWrapped().flag);
                data_serialized->addVariantValue("property_value", Subclass::dataWrapped().property_values);
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
            
        }
    }
}

#endif /* __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h */
