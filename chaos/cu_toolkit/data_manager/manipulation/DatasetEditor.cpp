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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/data_manager/manipulation/DatasetEditor.h>

using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::manipulation;

#define INFO    INFO_LOG(DatasetEditor)
#define DBG     DBG_LOG(DatasetEditor)
#define ERR     ERR_LOG(DatasetEditor)

DatasetEditor::DatasetEditor(DatasetElement::DatasetElementPtr& _dataset_element):
dataset_element(_dataset_element){}

DatasetEditor::~DatasetEditor(){}

const chaos::DataType::DatasetType DatasetEditor::getType() const {
    return dataset_element->dataset->getDatasetType();
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
                            %attr_name%dataset_element->dataset->getDatasetName());
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
