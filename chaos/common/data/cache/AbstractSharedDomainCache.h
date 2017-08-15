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

#ifndef __CHAOSFramework__AbstractSharedDomainCache__
#define __CHAOSFramework__AbstractSharedDomainCache__

#include <map>

#include <boost/thread.hpp>

#include <chaos/common/data/cache/AttributeCache.h>

namespace chaos{
	namespace common {
		namespace data {
			namespace cache {
				
				typedef enum SharedCacheDomain {
					DOMAIN_OUTPUT = 0,
					DOMAIN_INPUT,
					DOMAIN_CUSTOM,
					DOMAIN_SYSTEM
				} SharedCacheDomain;
				
				
				//-----------------------------------------------------------------------------------------------------------------------------
				class AbstractSharedDomainCache {
				public:
					AbstractSharedDomainCache();
					virtual ~AbstractSharedDomainCache();
					//! Return the value object for the domain and the string key
					/*!
					 \param domain a domain identified by a value of @AbstractSharedDomainCacheDomain
					 \key_name a name that identify the variable
					 */
					virtual AttributeValue *getAttributeValue(SharedCacheDomain domain,
															 const std::string& variable_name) = 0;
					
					//! Return the value object for the domain and the index of the variable
					virtual AttributeValue *getAttributeValue(SharedCacheDomain domain,
															  VariableIndexType variable_index) = 0;
					
					//! Set the value for a determinated variable in a determinate domain
					virtual void setAttributeValue(SharedCacheDomain domain,
												  const std::string& variable_name,
												  void * value, uint32_t size) = 0;
					
					//! Set the value for a determinated variable in a determinate domain
					virtual void setAttributeValue(SharedCacheDomain domain,
												  VariableIndexType variable_index,
												  void * value, uint32_t size) = 0;
					
					//! Get the index of the changed attribute
					virtual void getChangedAttributeIndex(SharedCacheDomain domain,
														 std::vector<VariableIndexType>& changed_index) = 0;
					
					//! Return the names of all variabl einto a determinated domain
					virtual void getAttributeNames(SharedCacheDomain domain,
												  std::vector<std::string>& names) = 0;
					
					//! Add a new Attribute
					virtual void addAttribute(SharedCacheDomain domain,
											 const std::string&  name,
											 uint32_t max_size,
											 chaos::DataType::DataType type) = 0;
					
                    virtual bool hasAttribute(SharedCacheDomain domain,
                                              const std::string&  name) = 0;
                    
					//! Get a specified type of shared domain
					/*!
					 Return a specified type of shared domain , identified by input parameter
					 \param the type of the domain to return
					 */
					virtual AttributeCache& getSharedDomain(SharedCacheDomain domain) = 0;
					
					//! return a class that implemnt the read o write lock on the specified domain
					/*!
					 \param domain the target that need to lock
					 \param write_lock false for read and true for write lock
					 */
					virtual ChaosSharedPtr<SharedCacheLockDomain> getLockOnDomain(SharedCacheDomain domain,
																					 bool write_lock) = 0;
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractDomainCache__) */
