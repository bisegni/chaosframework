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
                    AlterDatasetAttributeActionTypeUndefined,
                    AlterDatasetAttributeActionTypeSetValue,
                    AlterDatasetAttributeActionTypeAddTrigger,
                    AlterDatasetAttributeActionTypeRemoveTrigger,
                    AlterDatasetAttributeActionTypeConfigureTrigger
                };
                
#pragma mark AlterDatasetAttributeSetValue
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
                    if(serialized_data == NULL) return;
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
                
#pragma mark AlterDatasetAttributeAddTrigger
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
                
                //! define serialization wrapper for @AlterDatasetAttributeAddTrigger
                CHAOS_OPEN_SDWRAPPER(AlterDatasetAttributeAddTrigger)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    
                    //remove all attribute
                    Subclass::dataWrapped().dataset_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                    chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                                                                    "");
                    Subclass::dataWrapped().attribute_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                      chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                                                                      "");
                    Subclass::dataWrapped().event_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                  chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                                                                  "");
                    Subclass::dataWrapped().trigger_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                    chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                                                                    "");
                    
                    if(serialized_data->hasKey(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES)) {
                        //we have encoded properties
                        chaos::common::property::PropertyGroupSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(chaos::common::property::PropertyGroup, Subclass::dataWrapped().trigger_properties));
                        std::auto_ptr<CDataWrapper> prop_ser_auto(serialized_data->getCSDataValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES));
                        property_ref_wrapper.deserialize(prop_ser_auto.get());
                    }
                }
                
                std::auto_ptr<CDataWrapper> serialize() {
                    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                           Subclass::dataWrapped().dataset_name);
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                           Subclass::dataWrapped().attribute_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                           Subclass::dataWrapped().event_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                           Subclass::dataWrapped().trigger_name);
                    
                    chaos::common::property::PropertyGroupSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(chaos::common::property::PropertyGroup, Subclass::dataWrapped().trigger_properties));
                    result->addCSDataValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES, *property_ref_wrapper.serialize());
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
#pragma mark AlterDatasetAttributeRemoveTrigger
                //!actin description for add trigger
                struct AlterDatasetAttributeRemoveTrigger {
                    std::string dataset_name;
                    std::string attribute_name;
                    std::string event_name;
                    std::string trigger_name_uid;
                    
                    AlterDatasetAttributeRemoveTrigger();
                    AlterDatasetAttributeRemoveTrigger(const AlterDatasetAttributeRemoveTrigger& src);
                };
                
                //! define serialization wrapper for @AlterDatasetAttributeRemoveTrigger
                CHAOS_OPEN_SDWRAPPER(AlterDatasetAttributeRemoveTrigger)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    
                    //remove all attribute
                    Subclass::dataWrapped().dataset_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                    chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                                                                    "");
                    Subclass::dataWrapped().attribute_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                      chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                                                                      "");
                    Subclass::dataWrapped().event_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                  chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                                                                  "");
                    Subclass::dataWrapped().trigger_name_uid = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                        chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                                                                        "");
                }
                
                std::auto_ptr<CDataWrapper> serialize() {
                    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                           Subclass::dataWrapped().dataset_name);
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                           Subclass::dataWrapped().attribute_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                           Subclass::dataWrapped().event_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                           Subclass::dataWrapped().trigger_name_uid);
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
#pragma mark AlterDatasetAttributeConfigureTrigger
                struct AlterDatasetAttributeConfigureTrigger {
                    std::string dataset_name;
                    std::string attribute_name;
                    std::string event_name;
                    std::string trigger_name_uid;
                    chaos::common::property::PropertyGroup trigger_properties;
                    
                    AlterDatasetAttributeConfigureTrigger();
                    AlterDatasetAttributeConfigureTrigger(const AlterDatasetAttributeConfigureTrigger& src);
                };
                
                //! define serialization wrapper for @AlterDatasetAttributeConfigureTrigger
                CHAOS_OPEN_SDWRAPPER(AlterDatasetAttributeConfigureTrigger)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    
                    //remove all attribute
                    Subclass::dataWrapped().dataset_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                    chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                                                                    "");
                    Subclass::dataWrapped().attribute_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                      chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                                                                      "");
                    Subclass::dataWrapped().event_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                  chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                                                                  "");
                    Subclass::dataWrapped().trigger_name_uid = CDW_GET_SRT_WITH_DEFAULT(serialized_data,
                                                                                        chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                                                                        "");
                    
                    if(serialized_data->hasKey(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES)) {
                        //we have encoded properties
                        chaos::common::property::PropertyGroupSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(chaos::common::property::PropertyGroup, Subclass::dataWrapped().trigger_properties));
                        std::auto_ptr<CDataWrapper> prop_ser_auto(serialized_data->getCSDataValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES));
                        property_ref_wrapper.deserialize(prop_ser_auto.get());
                    }
                }
                
                std::auto_ptr<CDataWrapper> serialize() {
                    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_NAME,
                                           Subclass::dataWrapped().dataset_name);
                    result->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                           Subclass::dataWrapped().attribute_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_EVENT_NAME,
                                           Subclass::dataWrapped().event_name);
                    result->addStringValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_NAME,
                                           Subclass::dataWrapped().trigger_name_uid);
                    
                    chaos::common::property::PropertyGroupSDWrapper property_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(chaos::common::property::PropertyGroup, Subclass::dataWrapped().trigger_properties));
                    result->addCSDataValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_TRIGGER_PROPERTIES, *property_ref_wrapper.serialize());
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
#pragma mark AlterDatasetAttributeAction
                class AlterDatasetAttributeActionSDWrapper;
                //collect the dataset attribute action descriptions
                struct AlterDatasetAttributeAction {
                    friend class AlterDatasetAttributeActionSDWrapper;
                private:
                    //action descriptions
                    boost::variant<AlterDatasetAttributeSetValue,
                    AlterDatasetAttributeAddTrigger,
                    AlterDatasetAttributeRemoveTrigger,
                    AlterDatasetAttributeConfigureTrigger> action_description;
                public:
                    //type of the action
                    AlterDatasetAttributeActionType action_type;
                    
                    AlterDatasetAttributeSetValue&  getSetValueAction();
                    AlterDatasetAttributeAddTrigger& getAddTriggerAction();
                    AlterDatasetAttributeRemoveTrigger& getRemoveTriggerAction();
                    AlterDatasetAttributeConfigureTrigger& getConfigureTriggerAction();
                };
                
                CHAOS_OPEN_SDWRAPPER(AlterDatasetAttributeAction)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    
                    //serialize the action description
                    Subclass::dataWrapped().action_type = (AlterDatasetAttributeActionType)CDW_GET_INT32_WITH_DEFAULT(serialized_data,
                                                                                                                      chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_ACTION,
                                                                                                                      AlterDatasetAttributeActionTypeUndefined);
                    
                    //deserialize the action descirption
                    std::auto_ptr<chaos::common::data::SDWrapper> sd_wrapper;
                    if(serialized_data->hasKey(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_ACTION_SER)) {
                        switch (Subclass::dataWrapped().action_type) {
                            case AlterDatasetAttributeActionTypeSetValue: {
                                sd_wrapper.reset(new AlterDatasetAttributeSetValueSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeSetValue,
                                                                                                                                  Subclass::dataWrapped().getSetValueAction())));
                                break;
                            }
                            case AlterDatasetAttributeActionTypeAddTrigger: {
                                sd_wrapper.reset(new AlterDatasetAttributeAddTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeAddTrigger,
                                                                                                                                    Subclass::dataWrapped().getAddTriggerAction())));
                                break;
                            }
                            case AlterDatasetAttributeActionTypeRemoveTrigger: {
                                sd_wrapper.reset(new AlterDatasetAttributeRemoveTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeRemoveTrigger,
                                                                                                                                       Subclass::dataWrapped().getRemoveTriggerAction())));
                                break;
                            }
                            case AlterDatasetAttributeActionTypeConfigureTrigger: {
                                sd_wrapper.reset(new AlterDatasetAttributeConfigureTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeConfigureTrigger,
                                                                                                                                          Subclass::dataWrapped().getConfigureTriggerAction())));
                                break;
                            }
                            default:
                                break;
                        }
                        sd_wrapper->deserialize(serialized_data->getCSDataValueAsAutoPtr(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_ACTION_SER).get());
                    }
                }
                
                std::auto_ptr<CDataWrapper> serialize() {
                    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addInt32Value(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_ACTION,
                                          Subclass::dataWrapped().action_type);
                    
                    
                    //serialize the action descirption
                    std::auto_ptr<chaos::common::data::SDWrapper> sd_wrapper;
                    switch (Subclass::dataWrapped().action_type) {
                        case AlterDatasetAttributeActionTypeSetValue: {
                            sd_wrapper.reset(new AlterDatasetAttributeSetValueSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeSetValue,
                                                                                                                              Subclass::dataWrapped().getSetValueAction())));
                            break;
                        }
                        case AlterDatasetAttributeActionTypeAddTrigger: {
                            sd_wrapper.reset(new AlterDatasetAttributeAddTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeAddTrigger,
                                                                                                                                Subclass::dataWrapped().getAddTriggerAction())));
                            break;
                        }
                        case AlterDatasetAttributeActionTypeRemoveTrigger: {
                            sd_wrapper.reset(new AlterDatasetAttributeRemoveTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeRemoveTrigger,
                                                                                                                                   Subclass::dataWrapped().getRemoveTriggerAction())));
                            break;
                        }
                        case AlterDatasetAttributeActionTypeConfigureTrigger: {
                            sd_wrapper.reset(new AlterDatasetAttributeConfigureTriggerSDWrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AlterDatasetAttributeConfigureTrigger,
                                                                                                                                      Subclass::dataWrapped().getConfigureTriggerAction())));
                            break;
                        }
                        default:
                            break;
                    }
                    if(sd_wrapper.get()) {
                        result->addCSDataValue(chaos::DataBrokerNodeDomainDefinitionKey::ALTER_DATASET_ATTRIBUTE_ACTION_SER, *sd_wrapper->serialize());
                    }
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__916B7D8_6FEA_430D_AD84_3A954F031D45_DatabrokerAlterDSAttribute_h */
