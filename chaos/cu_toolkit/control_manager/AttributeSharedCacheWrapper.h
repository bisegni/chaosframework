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
#ifndef __CHAOSFramework__AttributeSharedCahe__
#define __CHAOSFramework__AttributeSharedCahe__
#include <chaos/common/global.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>

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
				AbstractSharedDomainCache * const attribute_value_shared_cache;
			public:
				AttributeSharedCacheWrapper(AbstractSharedDomainCache * const _attribute_value_shared_cache);
				~AttributeSharedCacheWrapper();

				//! return the typed const ptr associated at a name for a determinate domain
				template<typename T>
				T * getRWPtr(SharedCacheDomain domain,
							 const std::string& attribute_name) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					switch(domain) {
						case DOMAIN_INPUT:
						  /*	throw CException(-1, "Input variable domain can't be used as read write pointer", __PRETTY_FUNCTION__);
						   */
							break;
						case DOMAIN_SYSTEM:
							throw CException(-1, "System variable domain can't be used as read write pointer", __PRETTY_FUNCTION__);
							break;
						default:
							break;
					}
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, attribute_name);
					return value_setting->getValuePtr<T>();
				}
				
				//! return the typed const ptr associated at a name for a determinate domain
				template<typename T>
				const T * getROPtr(SharedCacheDomain domain,
								   const std::string& attribute_name) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, attribute_name);
					
					return (const T *)value_setting->getValuePtr<T>();
				}
				
				//! return the typed value associated at a name for a determinate domain
				template<typename T>
				T getValue(SharedCacheDomain domain,
						   const std::string& attribute_name) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, attribute_name);
					return *value_setting->getValuePtr<T>();
				}
				
				chaos::common::data::CDWUniquePtr getCDValue(SharedCacheDomain domain,const std::string& attribute_name);
				//! Return the value object for the domain and the string key
				template<typename T>
				void getCachedOutputAttributeValue(const std::string& attribute_name,
												   T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(DOMAIN_OUTPUT, attribute_name);
					if(value_setting) {
						*value_ptr = (T**)&value_setting->value_buffer;
					}
				}
				
				//! Return the value object for the domain and the index of the variable
				template<typename T>
				void getCachedOutputAttributeValue(VariableIndexType variable_index,
												   T*** value_ptr) {
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(DOMAIN_OUTPUT, variable_index);
					if(value_setting) {
						*value_ptr = (T**)&value_setting->value_buffer;
					}
				}
				
				//! Return the value object for the domain and the string key
				template<typename T>
				void getCachedCustomAttributeValue(const std::string& attribute_name,
												   T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(DOMAIN_CUSTOM, attribute_name);
					if(value_setting) {
						*value_ptr = (T**)&value_setting->value_buffer;
					}
				}
				
				//! Return the value object for the domain and the index of the variable
				template<typename T>
				void getCachedCustomAttributeValue(VariableIndexType variable_index,
												   T*** value_ptr) {
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(DOMAIN_CUSTOM, variable_index);
					if(value_setting) {
						*value_ptr = (T**)&value_setting->value_buffer;
					}
				}
				
				//! Return the value object for the domain and the string key
				template<typename T>
				void getReadonlyCachedAttributeValue(SharedCacheDomain domain,
													 const std::string& attribute_name,
													 const T*** value_ptr) {
					CHAOS_ASSERT(attribute_value_shared_cache)
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, attribute_name);
					if(value_setting) {
						*value_ptr = (const T**)&value_setting->value_buffer;
					}
				}
				
				//! Return the value object for the domain and the index of the variable
				template<typename T>
				void getReadonlyCachedAttributeValue(SharedCacheDomain domain,
													 VariableIndexType variable_index,
													 const T*** value_ptr) {
					AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, variable_index);
					if(value_setting) {
						*value_ptr = (const T**)&value_setting->value_buffer;
					}
				}
				
				template <typename T>
				void setOutputAttributeValue(const std::string& attribute_name,
											 T value){
												 setOutputAttributeValue(attribute_name,&value,sizeof(T));
											 }

				void setOutputAttributeValue(const std::string& attribute_name,
											 const std::string& value){
												 setOutputAttributeValue(attribute_name,(void*)value.c_str(),value.size()+1);
											 }
				//! Set the value for a determinated variable in a determinate domain
				void setOutputAttributeValue(const std::string& attribute_name,
											 void * value,
											 uint32_t size);
				//! Set the value for a determinated variable in a determinate domain
				void setOutputAttributeValue(VariableIndexType attribute_index,
													void * value,
													uint32_t size);
				//! set output domain changed
				void setOutputDomainAsChanged();

				//! set input domain changed
				void setInputDomainAsChanged();
				//!
				bool setOutputAttributeNewSize(const std::string& attribute_name,
											   uint32_t new_size,
                                               bool clear_mem = false);
				
				//!
				bool setOutputAttributeNewSize(VariableIndexType attribute_index,
											   uint32_t new_size,
                                               bool clear_mem = false);
				
				//!
				ChaosSharedPtr<SharedCacheLockDomain> getLockOnOutputAttributeCache(bool write_lock = true);
				
				//! Get the index of the changed attribute
				void getChangedInputAttributeIndex(std::vector<VariableIndexType>& changed_index);
				
				//!
				void resetChangedInputIndex();
				
				//! Return the names of all variabl einto a determinated domain
				void getAttributeNames(SharedCacheDomain domain,
									   std::vector<std::string>& names);
				
				//! Add a new variable
				void addCustomAttribute(const std::string&  name,
										uint32_t max_size,
										chaos::DataType::DataType type);
				//! Add a new variable
				void addCustomAttribute(const std::string&  name,
										 const chaos::common::data::CDataWrapper& value);
				//! Set the value for a determinated variable in a determinate domain
				void setCustomAttributeValue(const std::string& attribute_name,
											 void * value,
											 uint32_t size);

				void setCustomAttributeValue(const std::string& attribute_name,
											 const chaos::common::data::CDataWrapper& value);							 
				//! Set the value for a determinated variable in a determinate domain
				void setCustomAttributeValue(VariableIndexType attribute_index,
											 void * value,
											 uint32_t size);
				//!
				void setCustomDomainAsChanged();
				
				//!
				ChaosSharedPtr<SharedCacheLockDomain> getLockOnCustomAttributeCache(bool write_lock = true);
				
				//!
				ChaosSharedPtr<SharedCacheLockDomain> getReadLockOnInputAttributeCache();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AttributeSharedCahe__) */
