/*
 *	DataBrokerEditor.cpp
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

#include <chaos/cu_toolkit/data_manager/manipulation/DataBrokerEditor.h>

using namespace chaos::DataType;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::manipulation;

#define INFO INFO_LOG(DataBrokerEditor)
#define DBG DBG_LOG(DataBrokerEditor)
#define ERR ERR_LOG(DataBrokerEditor)

DataBrokerEditor::DataBrokerEditor():
index_name(container_dataset.get<DatasetElementTagName>()),
index_ordered(container_dataset.get<DatasetElementTagOrderedId>()),
index_type_name(container_dataset.get<DatasetElementTagTypeName>()){}

DataBrokerEditor::~DataBrokerEditor() {}

int DataBrokerEditor::addNewDataset(const std::string& name,
                                    const DatasetType type,
                                    const std::string& shared_key) {
    DECNameIndexIterator nit = index_name.find(name);
    if(nit != index_name.end()) {
        //a dataset with name already exists
        ERR << CHAOS_FORMAT("A dataset with name %1% already exists", %name);
        return -1;
    }
    
    //add new dataset
    container_dataset.insert(DatasetElement::DatasetElementPtr(new DatasetElement((unsigned int)container_dataset.size(),
                                                                                  DatasetPtr(new Dataset(name, type)))));
    return 0;
}

int DataBrokerEditor::addAttributeToDataset(const std::string& ds_name,
                                            const std::string& attr_name,
                                            const std::string& attr_description,
                                            const chaos::DataType::DataType attr_type,
                                            uint32_t maxSize) {
    int err = 0;
    DECNameIndexIterator nit = index_name.find(ds_name);
    if(nit != index_name.end()) {
        //a dataset with name already exists
        ERR << CHAOS_FORMAT("No dataset with name %1% found", %ds_name);
        err = -1;
    } else {
        //we can manipualte dataset adding new attrbiute
        DatasetAttributePtr new_attribute(new DatasetAttribute(attr_name,
                                                               attr_description,
                                                               attr_type));
        //add new attribute
        if((err = (*nit)->dataset->addAttribute(new_attribute))) {
            ERR << CHAOS_FORMAT("Error adding new attribute %1% into the dataset %2%", %attr_name%ds_name);
        } else {
            //add new cache for attribute
            (*nit)->dataset_value_cache.addAttribute(attr_name,
                                                     maxSize,
                                                     attr_type);
        }
    }
    return err;
}

int DataBrokerEditor::addBinaryAttributeAsSubtypeToDataSet(const std::string&          ds_name,
                                                           const std::string&          attr_name,
                                                           const std::string&          attr_description,
                                                           DataType::BinarySubtype     attr_subtype,
                                                           int32_t                     attr_cardinality) {
    //add the attribute
    int err = addAttributeToDataset(ds_name,
                                    attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    
    //set other value of the attribute
    DECNameIndexIterator nit = index_name.find(ds_name);
    DatasetAttributePtr attribute_ptr = (*nit)->dataset->getAttributebyName(attr_name);
    attribute_ptr->binary_subtype_list.push_back(attr_subtype);
    attribute_ptr->binary_cardinality = attr_cardinality;
    return err;
}

int DataBrokerEditor::addBinaryAttributeAsSubtypeToDataSet(const std::string&                               ds_name,
                                                           const std::string&                               attr_name,
                                                           const std::string&                               attr_description,
                                                           const std::vector<DataType::BinarySubtype>&      attr_subtype_list,
                                                           int32_t                                          attr_cardinality) {
    //add the attribute
    int err = addAttributeToDataset(ds_name,
                                    attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    
    //set other value of the attribute
    DECNameIndexIterator nit = index_name.find(ds_name);
    DatasetAttributePtr attribute_ptr = (*nit)->dataset->getAttributebyName(attr_name);
//    for(DatasetSubtypeListIterator it = attribute_ptr->binary_subtype_list.begin();
//        it != )
  //  attribute_ptr->binary_subtype_list = attr_subtype_list;
    attribute_ptr->binary_cardinality = attr_cardinality;
    return err;
}

int DataBrokerEditor::addBinaryAttributeAsMIMETypeToDataSet(const std::string& ds_name,
                                                            const std::string& attr_name,
                                                            const std::string& attr_description,
                                                            const std::string& attr_mime_type) {
    //add the attribute
    int err = addAttributeToDataset(ds_name,
                                    attr_name,
                                    attr_description,
                                    DataType::TYPE_BYTEARRAY);
    if(err) {return err;}
    
    //set other value of the attribute
    DECNameIndexIterator nit = index_name.find(ds_name);
    DatasetAttributePtr attribute_ptr = (*nit)->dataset->getAttributebyName(attr_name);
 
    return err;
}
