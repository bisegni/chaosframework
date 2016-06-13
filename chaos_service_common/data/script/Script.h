/*
 *	Script.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h
#define __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/TemplatedDataSDWrapper.h>
#include <chaos_service_common/data/dataset/DatasetAttribute.h>
#include <chaos_service_common/data/dataset/AlgorithmVariable.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace script {
                
#define CHAOS_SBD_NAME          "script_name"
#define CHAOS_SBD_DESCRIPTION   "script_description"
#define CHAOS_SBD_LANGUAGE      "script_language"
                
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
                CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(ScriptBaseDescription) {
                public:
                    ScriptBaseDescriptionSDWrapper():
                    ScriptBaseDescriptionSDWrapperSubclass(){}
                    
                    ScriptBaseDescriptionSDWrapper(const ScriptBaseDescription& copy_source):
                    ScriptBaseDescriptionSDWrapperSubclass(copy_source){}
                    
                    ScriptBaseDescriptionSDWrapper(common::data::CDataWrapper *serialized_data):
                    ScriptBaseDescriptionSDWrapperSubclass(serialized_data){
                        deserialize(serialized_data);
                    }
                    
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        dataWrapped().unique_id = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "seq", 0);
                        dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_NAME, "");
                        dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_DESCRIPTION, "");
                        dataWrapped().language = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_LANGUAGE, "");
                    }
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                        data_serialized->addInt64Value("seq", (sequence?sequence:dataWrapped().unique_id));
                        data_serialized->addStringValue(CHAOS_SBD_NAME, dataWrapped().name);
                        data_serialized->addStringValue(CHAOS_SBD_DESCRIPTION, dataWrapped().description);
                        data_serialized->addStringValue(CHAOS_SBD_LANGUAGE, dataWrapped().language);
                        return data_serialized;
                    }
                };
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(ScriptBaseDescription,
                                                      ScriptBaseDescriptionSDWrapper,
                                                      ScriptBaseDescriptionListWrapper);

                
#define CHAOS_SBD_SCRIPT_CONTENT "script_content"
                
                //! Full script description
                struct Script {
                    //! base script
                    ScriptBaseDescription script_description;
                    
                    //! the sourc ecode of the script
                    std::string script_content;
                    
                    //! contains all classification for a script
                    ChaosStringList classification_list;
                    
                    //! contains all pool associated to the script
                    ChaosStringList execution_pool_lis;
                    
                    //!variable list
                    chaos::service_common::data::dataset::AlgorithmVariableList variable_list;
                    
                    //!dataset attribute list
                    chaos::service_common::data::dataset::DatasetAttributeList dataset_attribute_list;
                    
                    Script():
                    script_description(),
                    script_content(){}
                    
                    Script(const Script& copy_src):
                    script_description(copy_src.script_description),
                    script_content(copy_src.script_content),
                    classification_list(copy_src.classification_list),
                    variable_list(copy_src.variable_list),
                    dataset_attribute_list(copy_src.dataset_attribute_list){}
                    
                    Script& operator=(Script const &rhs) {
                        script_description = rhs.script_description;
                        script_content = rhs.script_content;
                        classification_list = rhs.classification_list;
                        variable_list = rhs.variable_list;
                        dataset_attribute_list = rhs.dataset_attribute_list;
                        return *this;
                    };
                };
                
                //!heper for script class
                CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(Script) {
                public:
                    ScriptSDWrapper():
                    ScriptSDWrapperSubclass(){}
                    
                    ScriptSDWrapper(const Script& copy_source):
                    ScriptSDWrapperSubclass(copy_source){}
                    
                    ScriptSDWrapper(chaos::common::data::CDataWrapper *serialized_data):
                    ScriptSDWrapperSubclass(serialized_data){
                        deserialize(serialized_data);
                    }
                    
                    //! deserialization
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        CHAOS_DECLARE_SD_WRAPPER_VAR(ScriptBaseDescription, sd_dw);
                        CHAOS_DECLARE_SD_WRAPPER_VAR(chaos::service_common::data::dataset::AlgorithmVariable, algo_var_dw);
                        CHAOS_DECLARE_SD_WRAPPER_VAR(chaos::service_common::data::dataset::DatasetAttribute, ds_attr_dw);

                        //clear all ol data on the list
                        dataWrapped().variable_list.clear();
                        dataWrapped().dataset_attribute_list.clear();

                        const std::string variable_ser_key = "variables";
                        const std::string ds_attr_ser_key = chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION;
                        
                        sd_dw.deserialize(serialized_data);
                        //dcopy deserialize objet into own contained
                        dataWrapped().script_description = sd_dw.dataWrapped();
                        dataWrapped().script_content = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_SCRIPT_CONTENT, "");
                        
                        //deserialize classificaion list
                        if(serialized_data->hasKey("classification_list")) {
                            //encode classification list into array
                            std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue("classification_list"));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                dataWrapped().classification_list.push_back(serialized_array->getStringElementAtIndex(idx));
                            }
                        }
                        
                        //deserialize pool list
                        if(serialized_data->hasKey("execution_pool_lis")) {
                            //encode classification list into array
                            std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue("execution_pool_lis"));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                dataWrapped().execution_pool_lis.push_back(serialized_array->getStringElementAtIndex(idx));
                            }
                        }
                        
                        //deserialize variable
                        if(serialized_data->hasKey(variable_ser_key) &&
                           serialized_data->isVectorValue(variable_ser_key)) {
                            std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(variable_ser_key));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                std::auto_ptr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                algo_var_dw.deserialize(element.get());
                                dataWrapped().variable_list.push_back(algo_var_dw.dataWrapped());
                            }
                        }
                        
                        //deserialize dataset attribute
                        if(serialized_data->hasKey(ds_attr_ser_key) &&
                           serialized_data->isVectorValue(ds_attr_ser_key)) {
                            std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ds_attr_ser_key));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                std::auto_ptr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                ds_attr_dw.deserialize(element.get());
                                dataWrapped().dataset_attribute_list.push_back(ds_attr_dw.dataWrapped());
                            }
                        }
                    }
                    
                    //serialization
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0) {
                        CHAOS_DECLARE_SD_WRAPPER_VAR(ScriptBaseDescription, sd_dw)(dataWrapped().script_description);
                        CHAOS_DECLARE_SD_WRAPPER_VAR(chaos::service_common::data::dataset::AlgorithmVariable, algo_var_dw);
                        CHAOS_DECLARE_SD_WRAPPER_VAR(chaos::service_common::data::dataset::DatasetAttribute, ds_attr_dw);
                        
                        std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized = sd_dw.serialize(sequence);
                        
                        //add script content
                        data_serialized->addStringValue(CHAOS_SBD_SCRIPT_CONTENT, dataWrapped().script_content);
                        
                        if(dataWrapped().classification_list.size()) {
                            //encode classification list into array
                            for(ChaosStringListIterator str_it = dataWrapped().classification_list.begin(),
                                str_end = dataWrapped().classification_list.end();
                                str_it != str_end;
                                str_it++) {
                                data_serialized->appendStringToArray(*str_it);
                            }
                            data_serialized->finalizeArrayForKey("classification_list");
                        }
                        
                        if(dataWrapped().execution_pool_lis.size()) {
                            //encode classification list into array
                            for(ChaosStringListIterator str_it = dataWrapped().execution_pool_lis.begin(),
                                str_end = dataWrapped().classification_list.end();
                                str_it != str_end;
                                str_it++) {
                                data_serialized->appendStringToArray(*str_it);
                            }
                            data_serialized->finalizeArrayForKey("execution_pool_lis");
                        }
                        
                        //check for variable
                        if(dataWrapped().variable_list.size()) {
                            //we have some variable defined
                            std::auto_ptr<chaos::common::data::CDataWrapper> variable_definition(new chaos::common::data::CDataWrapper());
                            for(chaos::service_common::data::dataset::AlgorithmVariableListIterator it = dataWrapped().variable_list.begin(),
                                end = dataWrapped().variable_list.end();
                                it != end;
                                it++) {
                                algo_var_dw = *it;
                                data_serialized->appendCDataWrapperToArray(*algo_var_dw.serialize());
                            }
                            data_serialized->finalizeArrayForKey("variables");
                        }
                        
                        //check for dataset attribute
                        if(dataWrapped().dataset_attribute_list.size()) {
                            //we have some attribute for dataset
                            std::auto_ptr<chaos::common::data::CDataWrapper> variable_definition(new chaos::common::data::CDataWrapper());
                            for(chaos::service_common::data::dataset::DatasetAttributeListIterator it = dataWrapped().dataset_attribute_list.begin(),
                                end = dataWrapped().dataset_attribute_list.end();
                                it != end;
                                it++) {
                                ds_attr_dw = *it;
                                data_serialized->appendCDataWrapperToArray(*ds_attr_dw.serialize());
                            }
                            data_serialized->finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
                        }
                        return data_serialized;
                    }
                };
                
                
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(Script,
                                                      ScriptSDWrapper,
                                                      ScriptListWrapper);
            }
        }
    }
}

#endif /* __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h */
