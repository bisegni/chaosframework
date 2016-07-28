/*
 *	DataBrokerEditor.h
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

#ifndef __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h
#define __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            
            //!forward decalration
            class DataBroker;
            
            namespace manipulation {
                
                //! root class for the editing of the dataset
                class DataBrokerEditor {
                    friend class chaos::cu::data_manager::DataBroker;
                    
                    //reference to root dataset element
                    DatasetElementContainer container_dataset;
                    
                    DECNameIndex&        ds_index_name;
                    DECOrderedIndex&     ds_index_ordered;
                    DECTypeNameIndex&    ds_index_type_name;
                    
                    //add new dataset
                    /*!
                     New dataset is added checking if no other dataset has the same name and type.
                     In case dataset already exists it attribute will be updated with new values
                     */
                    int addNewDataset(chaos::common::data::structured::Dataset& new_dataset);
                    
                    void updateAttributePorertyForDataset(chaos::common::data::structured::DatasetAttribute attr_src,
                                                          chaos::common::data::structured::DatasetAttribute attr_dst);
                protected:
                    DataBrokerEditor();
                    ~DataBrokerEditor();
                    
                    //! add new dataset
                    /*!
                     Create a new empty dataset
                     \param name is the name of the newly create dataset
                     \param shared_key is the key used to publish the dataset
                     \param type is the type of the newly created dataset
                     \ingroup Dataset_Editor_Api
                     */
                    int addNewDataset(const std::string& name,
                                      const chaos::DataType::DatasetType type,
                                      const std::string& shared_key);
                    
                    
                    
                    //!Add a new attribute
                    /*!
                     Add the new attribute to a dataset
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     \param ds_name the name of the dataset that is parent of the attribute
                     \param attr_name the name of the new attribute
                     \param attribute_description the description of the attribute
                     \param attr_type the type of the new attribute
                     */
                    int addAttributeToDataset(const std::string& ds_name,
                                              const std::string& attr_name,
                                              const std::string& attr_description,
                                              const chaos::DataType::DataType attr_type,
                                              uint32_t maxSize = 0);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsSubtypeToDataSet(const std::string&        ds_name,
                                                             const std::string&        attr_name,
                                                             const std::string&        attr_description,
                                                             DataType::BinarySubtype   attr_subtype,
                                                             int32_t                   attr_cardinality);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsSubtypeToDataSet(const std::string&                             ds_name,
                                                             const std::string&                             attr_name,
                                                             const std::string&                             attr_description,
                                                             const std::vector<DataType::BinarySubtype>&    attr_subtype_list,
                                                             int32_t                                        attr_cardinality);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsMIMETypeToDataSet(const std::string& ds_name,
                                                              const std::string& attr_name,
                                                              const std::string& attr_description,
                                                              const std::string& attr_mime_type);
                    
                    //! Set the value for a determinated attribute of a dataset
                    int setOutputAttributeValue(const std::string&  ds_name,
                                                const std::string&  attr_name,
                                                void * value,
                                                uint32_t size);
                    
                    //! Set the value for a determinated attribute of a dataset
                    int setOutputAttributeValue(const std::string& ds_name,
                                                const unsigned int attr_index,
                                                void * value,
                                                uint32_t size);
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    int getReadonlyCachedAttributeValue(const std::string& ds_name,
                                                        const std::string& attr_name,
                                                        const T*** value_ptr) {
                        DECNameIndexIterator nit = ds_index_name.find(ds_name);
                        if(nit == ds_index_name.end()) return -1;
                        //we have the dataset
                        chaos::common::data::cache::AttributeValue *value_setting = (*nit)->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    int getReadonlyCachedAttributeValue(const std::string& ds_name,
                                                        unsigned int attr_index,
                                                        const T*** value_ptr) {
                        DECNameIndexIterator nit = ds_index_name.find(ds_name);
                        if(nit == ds_index_name.end()) return -1;
                        
                        //we have the dataset
                        chaos::common::data::cache::AttributeValue *value_setting = (*nit)->dataset_value_cache.getValueSettingForIndex(attr_index);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    int getCachedCustomAttributeValue(const std::string& ds_name,
                                                      const std::string& attr_name,
                                                      T*** value_ptr) {
                        DECNameIndexIterator nit = ds_index_name.find(ds_name);
                        if(nit == ds_index_name.end()) return -1;
                        //we have the dataset
                        chaos::common::data::cache::AttributeValue *value_setting = (*nit)->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    int getCachedCustomAttributeValue(const std::string& ds_name,
                                                      unsigned int attr_index,
                                                      T*** value_ptr) {
                        DECNameIndexIterator nit = ds_index_name.find(ds_name);
                        if(nit == ds_index_name.end()) return -1;
                        
                        //check if dataset is of an input type
                        if((*nit)->dataset->type != chaos::DataType::DatasetTypeInput) return -2;
                        
                        //we have the dataset
                        chaos::common::data::cache::AttributeValue *value_setting = (*nit)->dataset_value_cache.getValueSettingForIndex(attr_index);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! return a CDataWrapper pointr with the dataset serialization
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize();
                    
                    //! fille curren databrocker with serialization
                    void deserialize(chaos::common::data::CDataWrapper& serialization);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h */
