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

#ifndef __CHAOSFramework__5E8E92D_4407_4CFC_8AAC_C0CDA5BC3455_DatasetCacheWrapper_h
#define __CHAOSFramework__5E8E92D_4407_4CFC_8AAC_C0CDA5BC3455_DatasetCacheWrapper_h

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace manipulation {
                
                class DatasetCacheWrapper {
                    DatasetElement::DatasetElementPtr dataset_element;
                public:
                    DatasetCacheWrapper(DatasetElement::DatasetElementPtr& _dataset_element);
                    ~DatasetCacheWrapper();
                    
                    
                    //! Set the value for a determinated attribute of a dataset
                    int setAttributeValue(const std::string&  ds_name,
                                          const std::string&  attr_name,
                                          void * value,
                                          uint32_t size);
                    
                    //! Set the value for a determinated attribute of a dataset
                    int setAttributeValue(const std::string& ds_name,
                                          const unsigned int attr_index,
                                          void * value,
                                          uint32_t size);
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    void getReadonlyCachedAttributeValue(const std::string& ds_name,
                                                        const std::string& attr_name,
                                                        const T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    void getReadonlyCachedAttributeValue(const std::string& ds_name,
                                                        unsigned int attr_index,
                                                        const T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingForIndex(attr_index);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    void getCachedCustomAttributeValue(const std::string& ds_name,
                                                      const std::string& attr_name,
                                                      T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    void getCachedCustomAttributeValue(const std::string& ds_name,
                                                      unsigned int attr_index,
                                                      T*** value_ptr) {
                        //we have the dataset
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingForIndex(attr_index);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__5E8E92D_4407_4CFC_8AAC_C0CDA5BC3455_DatasetCacheWrapper_h */
