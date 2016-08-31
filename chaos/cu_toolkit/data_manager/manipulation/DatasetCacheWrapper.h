/*
 *	DatasetCacheWrapper.h
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
                    int setAttributeValue(const std::string&  attr_name,
                                          void * value,
                                          uint32_t size);
                    
                    //! Set the value for a determinated attribute of a dataset
                    int setAttributeValue(const unsigned int attr_index,
                                          void * value,
                                          uint32_t size);
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    int getReadonlyCachedAttributeValue(const std::string& attr_name,
                                                        const T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    int getReadonlyCachedAttributeValue(unsigned int attr_index,
                                                        const T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingForIndex(attr_index);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the string key
                    template<typename T>
                    int getCachedCustomAttributeValue(const std::string& attr_name,
                                                      T*** value_ptr) {
                        chaos::common::data::cache::AttributeValue *value_setting = dataset_element->dataset_value_cache.getValueSettingByName(attr_name);
                        if(value_setting) {
                            *value_ptr = (const T**)&value_setting->value_buffer;
                        }
                    }
                    
                    //! Return the value object for the domain and the index of the variable
                    template<typename T>
                    int getCachedCustomAttributeValue(unsigned int attr_index,
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
