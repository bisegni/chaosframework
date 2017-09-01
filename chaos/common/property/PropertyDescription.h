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

#ifndef __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h
#define __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

#include <boost/function.hpp>

namespace chaos {
    namespace common {
        namespace property {
            class PropertyDescriptionSDWrapper;
            
            //!define a porperty taht is rapresentated by haos base value
            class PropertyDescription {
                friend class PropertyDescriptionSDWrapper;
                //!current property value
                chaos::common::data::CDataVariant  property_value;
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
                                    const uint32_t _flag = 0,
                                    const chaos::common::data::CDataVariant& property_default_value = chaos::common::data::CDataVariant());
                
                
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
                    Subclass::dataWrapped().property_value = serialized_data->getVariantValue("property_value");
                }
            }
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                CDW_CHECK_AND_SET(Subclass::dataWrapped().name.size(), data_serialized, addStringValue, "property_name", Subclass::dataWrapped().name);
                CDW_CHECK_AND_SET(Subclass::dataWrapped().description.size(), data_serialized, addStringValue, "property_description", Subclass::dataWrapped().description);
                CDW_CHECK_AND_SET(Subclass::dataWrapped().type != DataType::TYPE_UNDEFINED, data_serialized, addInt32Value, "property_type", Subclass::dataWrapped().type);
                CDW_CHECK_AND_SET(Subclass::dataWrapped().flag != 0, data_serialized, addInt32Value, "property_flag", Subclass::dataWrapped().type);
                CDW_CHECK_AND_SET(Subclass::dataWrapped().property_value.isValid(), data_serialized, addStringValue, "property_value", Subclass::dataWrapped().property_value);
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
            
        }
    }
}

#endif /* __CHAOSFramework_A779EE47_822D_461E_B702_849E049D5CD7_PropertyDescription_h */
