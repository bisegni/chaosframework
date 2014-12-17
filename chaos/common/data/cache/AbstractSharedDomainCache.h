/*
 *	AbstractDomainCache.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AbstractSharedDomainCache__
#define __CHAOSFramework__AbstractSharedDomainCache__

#include <map>

#include <boost/thread.hpp>

#include <chaos/common/data/cache/AttributeCache.h>

namespace chaos{
	namespace common {
		namespace data {
			namespace cache {
				
				class SharedCacheLockDomain {
				protected:
					boost::shared_ptr<boost::shared_mutex>	mutex;
				public:
					SharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex);
					
					virtual ~SharedCacheLockDomain();
					
					virtual void lock() = 0;
					
					virtual void unlock() = 0;
				};
				
				class WriteSharedCacheLockDomain : public SharedCacheLockDomain {
					boost::unique_lock<boost::shared_mutex> w_lock;
				public:
					WriteSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex);
					
					~WriteSharedCacheLockDomain();
					
					void lock();
					
					void unlock();
				};
				
				class ReadSharedCacheLockDomain : public SharedCacheLockDomain {
					boost::shared_lock<boost::shared_mutex> r_lock;
				public:
					ReadSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex);
					
					~ReadSharedCacheLockDomain();
					
					void lock();
					
					void unlock();
				};
				
				
				typedef enum SharedCacheDomain {
					DOMAIN_INPUT,
					DOMAIN_OUTPUT,
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
					virtual boost::shared_ptr<SharedCacheLockDomain> getLockOnDomain(SharedCacheDomain domain,
																					 bool write_lock) = 0;
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractDomainCache__) */
