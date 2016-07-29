/*
 *	DatasetEditor.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#include "DatasetEditor.h"

using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::manipulation;

#define INFO    INFO_LOG(DatasetEditor)
#define DBG     DBG_LOG(DatasetEditor)
#define ERR     ERR_LOG(DatasetEditor)

DatasetEditor::DatasetEditor(DatasetElement::DatasetElementPtr& _dataset_element):
dataset_element(_dataset_element){}

DatasetEditor::~DatasetEditor(){}

const chaos::DataType::DatasetType DatasetEditor::getType() const {
    return dataset_element->dataset->type;
}

int DatasetEditor::addAttributeToDataset(const std::string& attr_name,
                                         const std::string& attr_description,
                                         const chaos::DataType::DataType attr_type,
                                         uint32_t maxSize) {
    int err = 0;
    //we can manipualte dataset adding new attrbiute
    DatasetAttributePtr new_attribute(new DatasetAttribute(attr_name,
                                                           attr_description,
                                                           attr_type));
   //add new attribute
    if((err = dataset_element->dataset->addAttribute(new_attribute))) {
        ERR << CHAOS_FORMAT("Error adding new attribute %1% into the dataset %2%",
                            %attr_name%dataset_element->dataset->name);
    } else {
        //add new cache for attribute
        dataset_element->dataset_value_cache.addAttribute(attr_name,
                                                 maxSize,
                                                 attr_type);
    }
    return err;
}

int DatasetEditor::addBinaryAttributeAsSubtypeToDataSet(const std::string&          attr_name,
                                                        const std::string&          attr_description,
                                                        DataType::BinarySubtype     attr_subtype,
                                                        int32_t                     attr_cardinality) {
    //add the attribute
    int err = addAttributeToDataset(attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    

    DatasetAttributePtr attribute_ptr = dataset_element->dataset->getAttributebyName(attr_name);
    attribute_ptr->binary_subtype_list.push_back(attr_subtype);
    attribute_ptr->binary_cardinality = attr_cardinality;
    return err;
}

int DatasetEditor::addBinaryAttributeAsSubtypeToDataSet(const std::string&           attr_name,
                                                        const std::string&           attr_description,
                                                        const DatasetSubtypeList&    attr_subtype_list,
                                                        int32_t                      attr_cardinality) {
    //add the attribute
    int err = addAttributeToDataset(attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    
    //set other value of the attribute
        DatasetAttributePtr attribute_ptr = dataset_element->dataset->getAttributebyName(attr_name);
    attribute_ptr->binary_subtype_list = attr_subtype_list;
    attribute_ptr->binary_cardinality = attr_cardinality;
    return err;
}

int DatasetEditor::addBinaryAttributeAsMIMETypeToDataSet(const std::string& attr_name,
                                                         const std::string& attr_description,
                                                         const std::string& attr_mime_type) {
    //add the attribute
    int err = addAttributeToDataset(attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    
    //set other value of the attribute
        DatasetAttributePtr attribute_ptr = dataset_element->dataset->getAttributebyName(attr_name);
    return err;
}
