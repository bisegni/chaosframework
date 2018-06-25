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

#ifndef __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h
#define __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

#include <chaos/common/data/structured/DatasetAttribute.h>
#include <chaos_service_common/data/dataset/AlgorithmVariable.h>
#include <chaos_service_common/data/node/Node.h>
namespace chaos {
    namespace service_common {
        namespace data {
            namespace script {
                
#define CHAOS_SBD_NAME          "script_name"
#define CHAOS_SBD_DESCRIPTION   "script_description"

                //! The description of a script
                struct ScriptBaseDescription {
                    uint64_t unique_id;
                    std::string name;
                    std::string description;
                    std::string language;
                    
                    ScriptBaseDescription():
                    unique_id(0),
                    name(),
                    description(),
                    language(){}
                    
                    ScriptBaseDescription(const ScriptBaseDescription& copy_src):
                    unique_id(copy_src.unique_id),
                    name(copy_src.name),
                    description(copy_src.description),
                    language(copy_src.language){}
                    
                    ScriptBaseDescription& operator=(ScriptBaseDescription const &rhs) {
                        unique_id = rhs.unique_id;
                        name = rhs.name;
                        description = rhs.description;
                        language = rhs.language;
                        return *this;
                    };
                };
                
                //!helper for create or read the script description
                CHAOS_OPEN_SDWRAPPER(ScriptBaseDescription)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    dataWrapped().unique_id = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "seq", 0);
                    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_NAME, "");
                    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_DESCRIPTION, "");
                    dataWrapped().language = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE, "");
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                    data_serialized->addInt64Value("seq", dataWrapped().unique_id);
                    data_serialized->addStringValue(CHAOS_SBD_NAME, dataWrapped().name);
                    data_serialized->addStringValue(CHAOS_SBD_DESCRIPTION, dataWrapped().description);
                    data_serialized->addStringValue(chaos::ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LANGUAGE, dataWrapped().language);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(ScriptBaseDescription,
                                                      ScriptBaseDescriptionSDWrapper,
                                                      ScriptBaseDescriptionListWrapper);
                
                
                //! Full script description
                struct Script {
                    //! base script
                    ScriptBaseDescription script_description;
                    
                    //! the sourc ecode of the script
                    std::string script_content;
                    
                    //! contains all classification for a script
                    ChaosStringVector classification_list;
                    
                    //! contains all pool associated to the script
                    ChaosStringVector execution_pool_list;
                    
                    //!variable list
                    chaos::service_common::data::dataset::AlgorithmVariableList variable_list;
                    
                    //!dataset attribute list
                    common::data::structured::DatasetAttributeList dataset_attribute_list;
                    
                    Script():
                    script_description(),
                    script_content(){}
                    
                    Script(const Script& copy_src):
                    script_description(copy_src.script_description),
                    script_content(copy_src.script_content),
                    classification_list(copy_src.classification_list),
                    execution_pool_list(copy_src.execution_pool_list),
                    variable_list(copy_src.variable_list),
                    dataset_attribute_list(copy_src.dataset_attribute_list){}
                    
                    Script& operator=(Script const &rhs) {
                        script_description = rhs.script_description;
                        script_content = rhs.script_content;
                        classification_list = rhs.classification_list;
                        execution_pool_list = rhs.execution_pool_list;
                        variable_list = rhs.variable_list;
                        dataset_attribute_list = rhs.dataset_attribute_list;
                        return *this;
                    };
                };
                
                //!heper for script class
                CHAOS_OPEN_SDWRAPPER(Script)
                    //! deserialization
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        ScriptBaseDescriptionSDWrapper sd_dw;
                        chaos::service_common::data::dataset::AlgorithmVariableSDWrapper algo_var_dw;
                        chaos::common::data::structured::DatasetAttributeSDWrapper ds_attr_dw;
                        
                        //clear all ol data on the list
                        dataWrapped().variable_list.clear();
                        dataWrapped().dataset_attribute_list.clear();
                        dataWrapped().classification_list.clear();
                        dataWrapped().execution_pool_list.clear();
                        
                        
                        const std::string variable_ser_key = "variables";
                        const std::string ds_attr_ser_key = chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION;
                        
                        sd_dw.deserialize(serialized_data);
                        //dcopy deserialize objet into own contained
                        dataWrapped().script_description = sd_dw.dataWrapped();
                        dataWrapped().script_content = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT, "");
                        
                        //deserialize classificaion list
                        if(serialized_data->hasKey("classification_list")) {
                            //encode classification list into array
                            chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue("classification_list");
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                dataWrapped().classification_list.push_back(serialized_array->getStringElementAtIndex(idx));
                            }
                        }
                        
                        //deserialize pool list
                        if(serialized_data->hasKey("execution_pool_list")) {
                            //encode classification list into array
                            chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue("execution_pool_list");
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                dataWrapped().execution_pool_list.push_back(serialized_array->getStringElementAtIndex(idx));
                            }
                        }
                        
                        //deserialize variable
                        if(serialized_data->hasKey(variable_ser_key) &&
                           serialized_data->isVectorValue(variable_ser_key)) {
                            chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue(variable_ser_key);
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                ChaosUniquePtr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                algo_var_dw.deserialize(element.get());
                                dataWrapped().variable_list.push_back(algo_var_dw.dataWrapped());
                            }
                        }
                        
                        //deserialize dataset attribute
                        if(serialized_data->hasKey(ds_attr_ser_key) &&
                           serialized_data->isVectorValue(ds_attr_ser_key)) {
                            chaos::common::data::CMultiTypeDataArrayWrapperSPtr serialized_array = serialized_data->getVectorValue(ds_attr_ser_key);
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                ChaosUniquePtr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                ds_attr_dw.deserialize(element.get());
                                dataWrapped().dataset_attribute_list.push_back(ds_attr_dw.dataWrapped());
                            }
                        }
                    }
                    
                    //serialization
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                        ScriptBaseDescriptionSDWrapper  sd_dw;
                        chaos::service_common::data::dataset::AlgorithmVariableSDWrapper algo_var_dw;
                        chaos::common::data::structured::DatasetAttributeSDWrapper ds_attr_dw;
                        
                        sd_dw() = dataWrapped().script_description;
                        
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized = sd_dw.serialize();
                        
                        //add script content
                        data_serialized->addStringValue(chaos::ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_CONTENT, dataWrapped().script_content);
                        
                        if(dataWrapped().classification_list.size()) {
                            //encode classification list into array
                            for(ChaosStringVectorIterator str_it = dataWrapped().classification_list.begin(),
                                str_end = dataWrapped().classification_list.end();
                                str_it != str_end;
                                str_it++) {
                                data_serialized->appendStringToArray(*str_it);
                            }
                            data_serialized->finalizeArrayForKey("classification_list");
                        }
                        
                        if(dataWrapped().execution_pool_list.size()) {
                            //encode classification list into array
                            for(ChaosStringVectorIterator str_it = dataWrapped().execution_pool_list.begin(),
                                str_end = dataWrapped().execution_pool_list.end();
                                str_it != str_end;
                                str_it++) {
                                data_serialized->appendStringToArray(*str_it);
                            }
                            data_serialized->finalizeArrayForKey("execution_pool_list");
                        }
                        
                        //check for variable
                        if(dataWrapped().variable_list.size()) {
                            //we have some variable defined
                            ChaosUniquePtr<chaos::common::data::CDataWrapper> variable_definition(new chaos::common::data::CDataWrapper());
                            for(chaos::service_common::data::dataset::AlgorithmVariableListIterator it = dataWrapped().variable_list.begin(),
                                end = dataWrapped().variable_list.end();
                                it != end;
                                it++) {
                                algo_var_dw() = *it;
                                data_serialized->appendCDataWrapperToArray(*algo_var_dw.serialize());
                            }
                            data_serialized->finalizeArrayForKey("variables");
                        }
                        
                        //check for dataset attribute
                        if(dataWrapped().dataset_attribute_list.size()) {
                            //we have some attribute for dataset
                            ChaosUniquePtr<chaos::common::data::CDataWrapper> variable_definition(new chaos::common::data::CDataWrapper());
                            for(chaos::common::data::structured::DatasetAttributeListIterator it = dataWrapped().dataset_attribute_list.begin(),
                                end = dataWrapped().dataset_attribute_list.end();
                                it != end;
                                it++) {
                                ds_attr_dw() = *it;
                                data_serialized->appendCDataWrapperToArray(*ds_attr_dw.serialize());
                            }
                            data_serialized->finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
                        }
                        return data_serialized;
                    }
                CHAOS_CLOSE_SDWRAPPER()
                
                
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(Script,
                                                      ScriptSDWrapper,
                                                      ScriptListWrapper);
                
                
                typedef enum ScriptBindType {
                    ScriptBindTypeUndefined = 0,
                    ScriptBindTypeDisable,
                    ScriptBindTypeAuto,
                    ScriptBindTypeUnitServer
                } ScriptBindType;
                
                static const char * const ScriptBindTypeUndefinedDescription = "Bind Undefined";
                static const char * const ScriptBindTypeDisableDescription = "Bind Disable";
                static const char * const ScriptBindTypeAutoDescription = "Bind Automatic";
                static const char * const ScriptBindTypeUnitServerDescription = "Bind to Unit Server";
                
                inline static ScriptBindType scriptBindTypeDecodeDescription(const std::string& description) {
                    if(description.compare(ScriptBindTypeDisableDescription) == 0){
                        return ScriptBindTypeDisable;
                    } else if(description.compare(ScriptBindTypeAutoDescription) == 0){
                        return ScriptBindTypeAuto;
                    } else if(description.compare(ScriptBindTypeUnitServerDescription) == 0){
                        return ScriptBindTypeUnitServer;
                    } else if(description.compare(ScriptBindTypeUndefinedDescription) == 0){
                        return ScriptBindTypeUndefined;
                    }
                    return ScriptBindTypeUndefined;
                }
                
                inline static std::string scriptBindTypeDecodeCode(const ScriptBindType code) {
                    switch(code) {
                        case ScriptBindTypeDisable:
                            return ScriptBindTypeDisableDescription;
                        case ScriptBindTypeAuto:
                            return ScriptBindTypeAutoDescription;
                        case ScriptBindTypeUnitServer:
                            return ScriptBindTypeUnitServerDescription;
                        case ScriptBindTypeUndefined:
                            return ScriptBindTypeUndefinedDescription;
                        default:
                            return ScriptBindTypeUndefinedDescription;
                    }
                }
                
                //! The description of an instance of the script
                struct ScriptInstance:
                public node::NodeInstance {
                    ScriptBindType  bind_type;
                    std::string     bind_node;
                    
                    ScriptInstance():
                    NodeInstance(),
                    bind_type(ScriptBindTypeDisable),
                    bind_node(){}
                    
                    ScriptInstance(const ScriptInstance& copy_src):
                    NodeInstance(copy_src),
                    bind_type(copy_src.bind_type),
                    bind_node(copy_src.bind_node){}
                    
                    ScriptInstance& operator=(ScriptInstance const &rhs) {
                        bind_type = rhs.bind_type;
                        bind_node = rhs.bind_node;
                        NodeInstance::operator=(rhs);
                        return *this;
                    };
                };
                
                //sd wrapper for node instance class
                CHAOS_OPEN_SDWRAPPER(ScriptInstance)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    if(serialized_data == NULL) return;
                    node::NodeInstanceSDWrapper node_instance(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(node::NodeInstance, dataWrapped()));
                    node_instance.deserialize(serialized_data);
                    dataWrapped().bind_type = static_cast<ScriptBindType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, "script_bind_type", ScriptBindTypeDisable));
                    dataWrapped().bind_node = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "script_bind_node", "");
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    node::NodeInstanceSDWrapper node_instance(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(node::NodeInstance, dataWrapped()));
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized = node_instance.serialize();
                    
                    data_serialized->addInt32Value("script_bind_type", static_cast<int32_t>(dataWrapped().bind_type));
                    data_serialized->addStringValue("script_bind_node", dataWrapped().bind_node);
                    return data_serialized;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h */
