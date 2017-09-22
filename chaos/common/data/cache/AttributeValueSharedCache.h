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

#ifndef __CHAOSFramework__AttributesSetting__
#define __CHAOSFramework__AttributesSetting__

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/data/cache/AbstractSharedDomainCache.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos{
	namespace common {
		namespace data {
			namespace cache {
				
#define READWRITE_ATTRIBUTE_HANDLE(x) x * *
#define READONLY_ATTRIBUTE_HANDLE(x) const x * *
#define ATTRIBUTE_HANDLE_GET_VALUE(x) **x
#define ATTRIBUTE_HANDLE_GET_PTR(x) *x
				
				/*!
				 Convenient class for grupping toghether the three different
				 cache for the attirbute (input, output, custom).
				 */
				class AttributeValueSharedCache :
				public utility::InizializableService,
				public AbstractSharedDomainCache {
					AttributeCache input_set;
					AttributeCache output_set;
					AttributeCache system_set;
					AttributeCache custom_set;
					
				public:
					
					AttributeValueSharedCache();
					~AttributeValueSharedCache();
					
					//! This field point to a custom memory shared by cu and all command
					void *customData;
					
					//! Initialize instance
					void init(void *initData) throw(chaos::CException);
					
					//! Deinit the implementation
					void deinit() throw(chaos::CException);
					
					//! Return the value object for the domain and the string key
					AttributeValue *getAttributeValue(SharedCacheDomain domain,
													  const std::string& variable_name);
					
					//! Return the value object for the domain and the index of the variable
					AttributeValue *getAttributeValue(SharedCacheDomain domain,
													  VariableIndexType variable_index);
					
					//! Set the value for a determinated variable in a determinate domain
					void setAttributeValue(SharedCacheDomain domain,
										   const std::string& attribute_name,
										   void * value,
										   uint32_t size);
					
					//! Set the value for a determinated Attribute in a determinate domain
					void setAttributeValue(SharedCacheDomain domain,
										   VariableIndexType attribute_index,
										   void * value,
										   uint32_t size);
					
					//! Get the index of the changed attribute
					void getChangedAttributeIndex(SharedCacheDomain domain,
												  std::vector<VariableIndexType>& changed_index);
					
					//! Return the names of all variabl einto a determinated domain
					void getAttributeNames(SharedCacheDomain domain,
										   std::vector<std::string>& names);
					
					//! Add a new Attribute
					void addAttribute(SharedCacheDomain domain,
									  const std::string& name,
									  uint32_t max_size,
									  chaos::DataType::DataType type);
					
                    bool hasAttribute(SharedCacheDomain domain,
                                      const std::string&  name);
                    
					//! Get a specified type of shared domain
					AttributeCache& getSharedDomain(SharedCacheDomain domain);
					
					//! return a class that implemnt the read o write lock on the specified domain
					ChaosSharedPtr<SharedCacheLockDomain> getLockOnDomain(SharedCacheDomain domain,
																			 bool write_lock);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__AttributesSetting__) */
