/*
 *	AttributesSetting.h
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

#ifndef __CHAOSFramework__AttributesSetting__
#define __CHAOSFramework__AttributesSetting__

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos{
	namespace common {
		namespace data {
			namespace cache {
				
				//froward declaration
				class AttributeSetting;
				
				//typedef
				typedef void * SettingValuePtr;
				
				//! the dimensio of the block for the boost::dynamic_bitset class
				typedef  uint8_t BitBlockDimension;
				
				//! the range of index
				typedef  uint16_t VariableIndexType;
				
#define READWRITE_ATTRIBUTE_HANDLE(x) x * *
#define READONLY_ATTRIBUTE_HANDLE(x) const x * *
#define ATTRIBUTE_HANDLE_GET_VALUE(x) **x
#define ATTRIBUTE_HANDLE_GET_PTR(x) *x
				
				//! manage the update of a value
				/*!
					A valueSetting is a class that help to understand when a value is changed and updated
				 */
				struct ValueSetting {
					friend class AttributeSetting;
					const std::string					name;
					//! the index of this value
					const uint32_t                      index;
					
					//! the size of value
					uint32_t                      size;
					
					//! is the datatype that represent the value
					const chaos::DataType::DataType     type;
					
					//main buffer
					void								*value_buffer;
					
					
					//global index bitmap for infom that this value(using index) has been changed
					boost::dynamic_bitset<BitBlockDimension> * sharedBitmapChangedAttribute;
					
					//!pirvate constructor
					ValueSetting(const std::string& _name,
								 uint32_t _index,
								 uint32_t _size,
								 chaos::DataType::DataType type);
					
					//!private destrucotr
					~ValueSetting();
					
					//!
					bool setNewSize(uint32_t _new_size);
					
					//! set a new value in buffer
					/*!
					 the memory on value_ptr is copied on the internal memory buffer
					 \param value_ptr the memory ptr that contains the new value to copy into internal memory
					 \param value_size the sie of the new value
					 */
					bool setValue(const void* value_ptr, uint32_t value_size, bool tag_has_changed = true);
					
					void markAsChanged();
					
					void markAsUnchanged();
					
					//! the value is returned has handle because the pointer can change it size ans so
					//! the pointer can be relocated
					template<typename T>
					T* getValuePtr() {
						return reinterpret_cast<T*>(value_buffer);
					}
				};
				
				//-----------------------------------------------------------------------------------------------------------------------------
				
				//! this class is a set of key with a ValueSetting class associated
				/*!
				 This class collect a set on key with the repsective ValueSetting creating a domain o
				 values.
				 */
				class AttributesSetting : public chaos::utility::InizializableService {
					//!global index for this set
					VariableIndexType index;
					
					boost::dynamic_bitset<BitBlockDimension> bitmapChangedAttribute;
					
					map<string, VariableIndexType> mapAttributeNameIndex;
					map<VariableIndexType, boost::shared_ptr<ValueSetting> > mapAttributeIndexSettings;
					

				public:
					boost::shared_ptr<boost::shared_mutex>	mutex;
					
					AttributesSetting();
					
					~AttributesSetting();
					
					void addAttribute(string name,
									  uint32_t size,
									  chaos::DataType::DataType type);
					
					void setValueForAttribute(VariableIndexType n,
											  const void * value,
											  uint32_t size);
					
					
					VariableIndexType getIndexForName(const std::string& name );
					
					//! get all attribute name in this set
					void getAttributeNames(std::vector<std::string>& names);
					
					//! Initialize instance
					void init(void *initData) throw(chaos::CException);
					
					//! Deinit the implementation
					void deinit() throw(chaos::CException);
					
					//! return the changed id into the vector
					void getChangedIndex(std::vector<VariableIndexType>& changed_index);
					
					//! get the ValueSetting for the index
					ValueSetting *getValueSettingForIndex(VariableIndexType index);
					
					//!fill the CDataWrapper representig the set
					void fillDataWrapper(CDataWrapper& data_wrapper);
					
					//! return the number of the attribute into the domain
					VariableIndexType getNumberOfAttributes();
					
					//! reset the chagned index array
					void resetChangedIndex();
					
					//! mark all attribute as changed
					void markAllAsChanged();
					
					//! return true if some attribute has change it's value
					bool hasChanged();
					
					//! set new size on attribute by index
					bool setNewSize(VariableIndexType attribute_index, uint32_t new_size);
				};
				
				//-----------------------------------------------------------------------------------------------------------------------------
				
				class SharedCacheLockDomain {
				protected:
					boost::shared_ptr<boost::shared_mutex>	mutex;
				public:
					SharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
					mutex(_mutex){
					}
					virtual ~SharedCacheLockDomain(){}
					
					virtual void lock() = 0;
					
					virtual void unlock() = 0;
				};
				
				class WriteSharedCacheLockDomain : public SharedCacheLockDomain {
					boost::unique_lock<boost::shared_mutex> w_lock;
				public:
					WriteSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
					SharedCacheLockDomain(_mutex),
					w_lock(*mutex.get(), boost::defer_lock){}
					
					~WriteSharedCacheLockDomain() {}
					
					void lock() {
						w_lock.lock();
					}
					
					void unlock() {
						w_lock.unlock();
					}
				};
				
				class ReadSharedCacheLockDomain : public SharedCacheLockDomain {
					boost::shared_lock<boost::shared_mutex> r_lock;
				public:
					ReadSharedCacheLockDomain(boost::shared_ptr<boost::shared_mutex> _mutex):
					SharedCacheLockDomain(_mutex),
					r_lock(*mutex.get(), boost::defer_lock){}
					
					~ReadSharedCacheLockDomain() {}
					
					void lock() {
						r_lock.lock();
					}
					
					void unlock() {
						r_lock.unlock();
					}
				};
				//-----------------------------------------------------------------------------------------------------------------------------
				class SharedCacheInterface {
				public:
					
					typedef enum SharedVariableDomain {
						SVD_INPUT,
						SVD_OUTPUT,
						SVD_SYSTEM,
						SVD_CUSTOM
					}SharedVariableDomain;
					
					SharedCacheInterface(){};
					virtual ~SharedCacheInterface(){};
					
					//! Return the value object for the domain and the string key
					/*!
					 \param domain a domain identified by a value of @AttributeValueSharedCache::SharedVariableDomain
					 \key_name a name that identify the variable
					 */
					virtual ValueSetting *getVariableValue(SharedVariableDomain domain,
														   const std::string& variable_name) = 0;
					
					//! Return the value object for the domain and the index of the variable
					virtual ValueSetting *getVariableValue(SharedVariableDomain domain,
														   VariableIndexType variable_index) = 0;
					
					//! Set the value for a determinated variable in a determinate domain
					virtual void setVariableValue(SharedVariableDomain domain,
												  const std::string& variable_name,
												  void * value, uint32_t size) = 0;
					//! Set the value for a determinated variable in a determinate domain
					virtual void setVariableValue(SharedVariableDomain domain,
												  VariableIndexType variable_index,
												  void * value, uint32_t size) = 0;
					//! Get the index of the changed attribute
					virtual void getChangedVariableIndex(SharedVariableDomain domain,
														 std::vector<VariableIndexType>& changed_index) = 0;
					
					//! Return the names of all variabl einto a determinated domain
					virtual void getVariableNames(SharedVariableDomain domain,
												  std::vector<std::string>& names) = 0;
					
					//! Add a new variable
					virtual void addVariable(SharedCacheInterface::SharedVariableDomain domain,
											 const std::string&  name,
											 uint32_t max_size,
											 chaos::DataType::DataType type) = 0;
					
					//! Get a specified type of shared domain
					/*!
					 Return a specified type of shared domain , identified by input parameter
					 \param the type of the domain to return
					 */
					virtual AttributesSetting& getSharedDomain(SharedVariableDomain domain) = 0;
					
					//! return a class that implemnt the read o write lock on the specified domain
					/*!
					 \param domain the target that need to lock
					 \param write_lock false for read and true for write lock
					 */
					virtual boost::shared_ptr<SharedCacheLockDomain> getLockOnDomain(SharedVariableDomain domain, bool write_lock) = 0;
				};
				
				//-----------------------------------------------------------------------------------------------------------------------------
				
				/*!
				 Convenient class for grupping toghether the three different
				 cache for the attirbute (input, output, custom).
				 */
				class AttributeValueSharedCache : public chaos::utility::InizializableService, public SharedCacheInterface {
					AttributesSetting input_set;
					AttributesSetting output_set;
					AttributesSetting system_set;
					AttributesSetting custom_set;
					
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
					/*!
					 \param domain a domain identified by a value of @SharedCacheInterface::SharedVariableDomain
					 \key_name a name that identify the variable
					 */
					ValueSetting *getVariableValue(SharedCacheInterface::SharedVariableDomain domain,
												   const std::string& variable_name);
					
					//! Return the value object for the domain and the index of the variable
					ValueSetting *getVariableValue(SharedCacheInterface::SharedVariableDomain domain,
												   VariableIndexType variable_index);
					
					//! Set the value for a determinated variable in a determinate domain
					void setVariableValue(SharedCacheInterface::SharedVariableDomain domain,
										  const std::string& variable_name,
										  void * value,
										  uint32_t size);
					//! Set the value for a determinated variable in a determinate domain
					void setVariableValue(SharedCacheInterface::SharedVariableDomain domain,
										  VariableIndexType variable_index,
										  void * value,
										  uint32_t size);
					//! Get the index of the changed attribute
					void getChangedVariableIndex(SharedCacheInterface::SharedVariableDomain domain,
												 std::vector<VariableIndexType>& changed_index);
					
					//! Return the names of all variabl einto a determinated domain
					void getVariableNames(SharedCacheInterface::SharedVariableDomain domain,
										  std::vector<std::string>& names);
					
					//! Add a new variable
					void addVariable(SharedCacheInterface::SharedVariableDomain domain,
									 const std::string& name,
									 uint32_t max_size,
									 chaos::DataType::DataType type);
					
					//! Get a specified type of shared domain
					AttributesSetting& getSharedDomain(SharedCacheInterface::SharedVariableDomain domain);
					
					//! return a class that implemnt the read o write lock on the specified domain
					boost::shared_ptr<SharedCacheLockDomain> getLockOnDomain(SharedVariableDomain domain, bool write_lock);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__AttributesSetting__) */
