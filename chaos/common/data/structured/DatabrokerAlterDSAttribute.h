/*
 *	DatabrokerAlterDSAttribute.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 30/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__916B7D8_6FEA_430D_AD84_3A954F031D45_DatabrokerAlterDSAttribute_h
#define __CHAOSFramework__916B7D8_6FEA_430D_AD84_3A954F031D45_DatabrokerAlterDSAttribute_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/property/PropertyGroup.h>

#include <boost/variant.hpp>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                
                enum AlterDatasetAttributeActionType {
                    AlterDatasetAttributeActionTypeSetValue,
                    AlterDatasetAttributeActionTypeAddTrigger,
                    AlterDatasetAttributeActionTypeRemoveTrigger,
                    AlterDatasetAttributeActionTypeConfigureTrigger
                };
                
                //!compose the action for set value on dataset
                struct AlterDatasetAttributeSetValue {
                    std::string dataset_name;
                    std::string attribute_name;
                    chaos::common::data::CDataVariant attribute_value;
                    
                    AlterDatasetAttributeSetValue();
                    AlterDatasetAttributeSetValue(const AlterDatasetAttributeSetValue& src);
                };
                
                //! define serialization wrapper for @AlterDatasetAttributeSetValue
                CHAOS_OPEN_SDWRAPPER(AlterDatasetAttributeSetValue)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    
                    //remove all attribute
                    Subclass::dataWrapped().dataset_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                    chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                                                                    "");
                    Subclass::dataWrapped().attribute_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                      chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                                                                      "");
                    Subclass::dataWrapped().attribute_value = CDW_GET_VALUE_WITH_DEFAULT(serialized_data,
                                                                                         chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_VALUE,
                                                                                         getVariantValue,
                                                                                         chaos::common::data::CDataVariant());
                }
                
                std::auto_ptr<CDataWrapper> serialize() {
                    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                           Subclass::dataWrapped().dataset_name);
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                           Subclass::dataWrapped().attribute_name);
                    result->addVariantValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_VALUE,
                                            Subclass::dataWrapped().attribute_value);
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                //!actin description for add trigger
                struct AlterDatasetAttributeAddTrigger {
                    std::string dataset_name;
                    std::string attribute_name;
                    std::string event_name;
                    std::string trigger_name;
                    chaos::common::property::PropertyGroup trigger_properties;
                    
                    AlterDatasetAttributeAddTrigger();
                    AlterDatasetAttributeAddTrigger(const AlterDatasetAttributeAddTrigger& src);
                };
                
                //!actin description for add trigger
                struct AlterDatasetAttributeRemoveTrigger {
                    std::string dataset_name;
                    std::string attribute_name;
                    std::string event_name;
                    std::string trigger_name_uid;
                    
                    AlterDatasetAttributeRemoveTrigger();
                    AlterDatasetAttributeRemoveTrigger(const AlterDatasetAttributeRemoveTrigger& src);
                };
                
                struct AlterDatasetAttributeConfigureTrigger {
                    std::string dataset_name;
                    std::string attribute_name;
                    std::string event_name;
                    std::string trigger_name_uid;
                    chaos::common::property::PropertyGroup trigger_properties;
                    
                    AlterDatasetAttributeConfigureTrigger();
                    AlterDatasetAttributeConfigureTrigger(const AlterDatasetAttributeConfigureTrigger& src);
                };
                
                
                //collect the dataset attribute action descriptions
                struct AlterDatasetAttributeAction {
                    //type of the action
                    AlterDatasetAttributeActionType action_type;
                    
                    //action descriptions
                    boost::variant<AlterDatasetAttributeSetValue,
                    AlterDatasetAttributeAddTrigger,
                    AlterDatasetAttributeRemoveTrigger,
                    AlterDatasetAttributeConfigureTrigger> action_description;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__916B7D8_6FEA_430D_AD84_3A954F031D45_DatabrokerAlterDSAttribute_h */
