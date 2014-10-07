/*
 *	AttributeSharedCacheWrapper.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__AttributeSharedCahe__
#define __CHAOSFramework__AttributeSharedCahe__

#include <chaos/common/data/cache/AttributesSetting.h>

namespace chaos{
	namespace cu {
		namespace control_manager {
			
			using namespace chaos::common::data::cache;
			
			class AttributeSharedCacheWrapper {
				//! attributed value shared cache
				/*!
				 The data in every attribute is published automatically on the chaos data service
				 with somne triggering logic according to the domain one.
				 */
				SharedCacheInterface * const attribute_value_shared_cache;
			public:
				AttributeSharedCacheWrapper(SharedCacheInterface * const _attribute_value_shared_cache);
				~AttributeSharedCacheWrapper();
				// Return the value object for the domain and the string key
				template<typename T>
				void getCachedOutputAttributeValue(const std::string& variable_name,
												   T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(AttributeValueSharedCache::SVD_OUTPUT,
																								 variable_name);
					if(value_setting) {
						*value_ptr = value_setting->getValueHandle<T>();
					}
				}
				
				// Return the value object for the domain and the index of the variable
				template<typename T>
				void getCachedOutputAttributeValue(VariableIndexType variable_index,
												   T*** value_ptr) {
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(AttributeValueSharedCache::SVD_OUTPUT,
																								 variable_index);
					if(value_setting) {
						*value_ptr = value_setting->getValueHandle<T>();
					}
				}
				
				// Return the value object for the domain and the string key
				template<typename T>
				void getCachedCustomAttributeValue(const std::string& variable_name,
												   T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(AttributeValueSharedCache::SVD_CUSTOM,
																								 variable_name);
					if(value_setting) {
						*value_ptr = value_setting->getValueHandle<T>();
					}
				}
				
				// Return the value object for the domain and the index of the variable
				template<typename T>
				void getCachedCustomAttributeValue(VariableIndexType variable_index,
												   T*** value_ptr) {
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(AttributeValueSharedCache::SVD_CUSTOM,
																								 variable_index);
					if(value_setting) {
						*value_ptr = value_setting->getValueHandle<T>();
					}
				}
				
				// Return the value object for the domain and the string key
				template<typename T>
				void getReadonlyCachedAttributeValue(AttributeValueSharedCache::SharedVariableDomain domain,
													 const std::string& variable_name,
													 const T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(domain, variable_name);
					if(value_setting) {
						*value_ptr = value_setting->getValueHandle<T>();
					}
				}
				
				// Return the value object for the domain and the index of the variable
				template<typename T>
				void getReadonlyCachedAttributeValue(AttributeValueSharedCache::SharedVariableDomain domain,
													 VariableIndexType variable_index,
													 const T*** value_ptr) {
					ValueSetting *value_setting = attribute_value_shared_cache->getVariableValue(domain, variable_index);
					if(value_setting) {
						*value_ptr = (const T**)value_setting->getValueHandle<T>();
					}
				}
				
				// Set the value for a determinated variable in a determinate domain
				void setOutputAttributeValue(const std::string& attribute_name,
											 void * value,
											 uint32_t size);
				// Set the value for a determinated variable in a determinate domain
				void setOutputAttributeValue(VariableIndexType attribute_index,
													void * value,
													uint32_t size);
				void setOutputDomainAsChanged();
				
				bool setOutputAttributeNewSize(const std::string& attribute_name,
											   uint32_t new_size);
				
				bool setOutputAttributeNewSize(VariableIndexType attribute_index,
											   uint32_t new_size);
				
				// Get the index of the changed attribute
				void getChangedInputAttributeIndex(std::vector<VariableIndexType>& changed_index);
				
				void resetChangedInputIndex();
				
				// Return the names of all variabl einto a determinated domain
				void getVariableNames(AttributeValueSharedCache::SharedVariableDomain domain,
									  std::vector<std::string>& names);
				
				// Add a new variable
				void addCustomAttribute(const std::string&  name,
										uint32_t max_size,
										chaos::DataType::DataType type);
				
				// Set the value for a determinated variable in a determinate domain
				void setCustomAttributeValue(const std::string& attribute_name,
											 void * value,
											 uint32_t size);
				// Set the value for a determinated variable in a determinate domain
				void setCustomAttributeValue(VariableIndexType attribute_index,
											 void * value,
											 uint32_t size);
				
				void setCustomDomainAsChanged();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AttributeSharedCahe__) */
