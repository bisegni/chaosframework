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

#ifndef __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h
#define __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace dataset {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(unsigned int, DatasetSubtypeList)
                
                //! The description of a n attribute of a CHAOS dataset
                struct DatasetAttribute {
                    std::string                                     name;
                    std::string                                     description;
                    std::string                                     min_value;
                    std::string                                     max_value;
                    std::string                                     inc;
                    std::string                                     unit;
                    std::string                                     conv;
                    std::string                                     off;
                    std::string                                     default_value;
                    chaos::DataType::DataSetAttributeIOAttribute    direction;
                    chaos::DataType::DataType                       type;
                    DatasetSubtypeList                              binary_subtype_list;
                    uint32_t                                        binary_cardinality;
                    
                    DatasetAttribute():
                    name(),
                    description(),
                    direction(chaos::DataType::Bidirectional),
                    type(chaos::DataType::TYPE_UNDEFINED),
                    binary_subtype_list(),
                    binary_cardinality(0),inc("0"){}
                    
                    DatasetAttribute(const DatasetAttribute& copy_src):
                    name(copy_src.name),
                    description(copy_src.description),
                    direction(copy_src.direction),
                    type(copy_src.type),
                    inc(copy_src.inc),
                    unit(copy_src.unit),
                    binary_subtype_list(copy_src.binary_subtype_list),
                    binary_cardinality(copy_src.binary_cardinality){}
                    
                    DatasetAttribute& operator=(DatasetAttribute const &rhs) {
                        name = rhs.name;
                        description = rhs.description;
                        direction = rhs.direction;
                        type = rhs.type;
                        inc=rhs.inc;
                        unit=rhs.unit;
                        binary_subtype_list = rhs.binary_subtype_list;
                        binary_cardinality = rhs.binary_cardinality;
                        return *this;
                    };
                };
                
                
                //!helper for create or read the script description
                CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(DatasetAttribute) {
                public:
                    DatasetAttributeSDWrapper():
                    DatasetAttributeSDWrapperSubclass(){}
                    
                    DatasetAttributeSDWrapper(const DatasetAttribute& copy_source):
                    DatasetAttributeSDWrapperSubclass(copy_source){}
                    
                    DatasetAttributeSDWrapper(chaos::common::data::CDataWrapper *serialized_data):
                    DatasetAttributeSDWrapperSubclass(serialized_data){
                        deserialize(serialized_data);
                    }
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        dataWrapped().binary_subtype_list.clear();
                        dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, "");
                        dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, "");
                        dataWrapped().direction = static_cast<chaos::DataType::DataSetAttributeIOAttribute>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, chaos::DataType::Bidirectional));
                        dataWrapped().type = static_cast<chaos::DataType::DataType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, chaos::DataType::TYPE_UNDEFINED));
                        dataWrapped().min_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, "");
                        dataWrapped().max_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, "");
                        dataWrapped().default_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, "");
                        dataWrapped().inc = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_INCREMENT, "0");
                        dataWrapped().unit = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_UNIT, "NA");
                        dataWrapped().conv = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_CONVFACT, "1");
                        dataWrapped().off = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_OFFSET, "0");

                        dataWrapped().binary_cardinality = CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY, 0);
                        
                        if(dataWrapped().type == DataType::TYPE_BYTEARRAY) {
                            if(serialized_data->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                                if(serialized_data->isVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                                    //multiple subtype
                                    std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE));
                                    for(int idx = 0;
                                        idx < serialized_array->size();
                                        idx++) {
                                        chaos::DataType::BinarySubtype sub_type = static_cast<chaos::DataType::BinarySubtype>(serialized_array->getInt32ElementAtIndex(idx));
                                        dataWrapped().binary_subtype_list.push_back(sub_type);
                                    }
                                } else {
                                    //single subtype
                                    dataWrapped().binary_subtype_list.push_back(static_cast<chaos::DataType::BinarySubtype>(serialized_data->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)));
                                }
                            }
                        }
                    }
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, dataWrapped().name);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, dataWrapped().description);
                        data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, dataWrapped().direction);
                        data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, dataWrapped().type);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, dataWrapped().min_value);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, dataWrapped().max_value);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, dataWrapped().default_value);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_INCREMENT, dataWrapped().inc);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_UNIT, dataWrapped().unit);
                        
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_CONVFACT, dataWrapped().conv);
                        data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_OFFSET, dataWrapped().off);

                        data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY, dataWrapped().binary_cardinality);
                        if((dataWrapped().type == DataType::TYPE_BYTEARRAY) &&
                           dataWrapped().binary_subtype_list.size()) {
                            if(dataWrapped().binary_subtype_list.size() > 1) {
                                //multiple value
                                for(DatasetSubtypeListIterator it = dataWrapped().binary_subtype_list.begin(),
                                    end = dataWrapped().binary_subtype_list.end();
                                    it != end;
                                    it++) {
                                    data_serialized->appendInt32ToArray(*it);
                                }
                                data_serialized->finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE);
                            } else {
                                data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE, dataWrapped().binary_subtype_list[0]);
                            }
                        }
                        
                        return data_serialized;
                    }
                };
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(DatasetAttribute,
                                                      DatasetAttributeSDWrapper,
                                                      DatasetAttributeListWrapper);

                
                //Definition of dataset attribute list
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::service_common::data::dataset::DatasetAttribute,
                                             DatasetAttributeList);
            }
        }
    }
}

#endif /* __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h */
