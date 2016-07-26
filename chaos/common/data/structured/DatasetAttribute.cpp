/*
 *	DatasetAttribute.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/structured/DatasetAttribute.h>

using namespace chaos::common::data::structured;

#pragma mark DatasetAttribute
DatasetAttribute::DatasetAttribute():
name(),
description(),
direction(chaos::DataType::Bidirectional),
type(chaos::DataType::TYPE_UNDEFINED),
binary_subtype_list(),
binary_cardinality(0){}

DatasetAttribute::DatasetAttribute(const std::string& attr_name,
                                   const std::string& attr_description,
                                   const chaos::DataType::DataType& attr_type):
name(attr_name),
description(attr_description),
direction(chaos::DataType::Bidirectional),
type(attr_type),
binary_subtype_list(),
binary_cardinality(0){
    
}

DatasetAttribute::DatasetAttribute(const DatasetAttribute& copy_src):
name(copy_src.name),
description(copy_src.description),
direction(copy_src.direction),
type(copy_src.type),
binary_subtype_list(copy_src.binary_subtype_list),
binary_cardinality(copy_src.binary_cardinality){}

DatasetAttribute& DatasetAttribute::operator=(DatasetAttribute const &rhs) {
    name = rhs.name;
    description = rhs.description;
    direction = rhs.direction;
    type = rhs.type;
    binary_subtype_list = rhs.binary_subtype_list;
    binary_cardinality = rhs.binary_cardinality;
    return *this;
};

#pragma mark DatasetAttributeSDWrapper
DatasetAttributeSDWrapper::DatasetAttributeSDWrapper():
DatasetAttributeSDWrapperSubclass(){}

DatasetAttributeSDWrapper::DatasetAttributeSDWrapper(const DatasetAttribute& copy_source):
DatasetAttributeSDWrapperSubclass(copy_source){}

DatasetAttributeSDWrapper::DatasetAttributeSDWrapper(chaos::common::data::CDataWrapper *serialized_data):
DatasetAttributeSDWrapperSubclass(serialized_data){
    deserialize(serialized_data);
}

void DatasetAttributeSDWrapper::deserialize(chaos::common::data::CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    dataWrapped().binary_subtype_list.clear();
    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, "");
    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, "");
    dataWrapped().direction = static_cast<chaos::DataType::DataSetAttributeIOAttribute>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, chaos::DataType::Bidirectional));
    dataWrapped().type = static_cast<chaos::DataType::DataType>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, chaos::DataType::TYPE_UNDEFINED));
    dataWrapped().min_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, "");
    dataWrapped().max_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, "");
    dataWrapped().default_value = CDW_GET_SRT_WITH_DEFAULT(serialized_data, chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, "");
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

std::auto_ptr<chaos::common::data::CDataWrapper> DatasetAttributeSDWrapper::serialize(const uint64_t sequence) {
    std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
    data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, dataWrapped().name);
    data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, dataWrapped().description);
    data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, dataWrapped().direction);
    data_serialized->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, dataWrapped().type);
    data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, dataWrapped().min_value);
    data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, dataWrapped().max_value);
    data_serialized->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, dataWrapped().default_value);
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
