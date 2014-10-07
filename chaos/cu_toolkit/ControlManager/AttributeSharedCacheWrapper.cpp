/*
 *	AttributeSharedCacheWrapper.cpp
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

#include <chaos/cu_toolkit/ControlManager/AttributeSharedCacheWrapper.h>
using namespace chaos::cu::control_manager;
using namespace chaos::common::data::cache;

AttributeSharedCacheWrapper::AttributeSharedCacheWrapper(SharedCacheInterface *_attribute_value_shared_cache):
attribute_value_shared_cache(_attribute_value_shared_cache){
	
}

AttributeSharedCacheWrapper::~AttributeSharedCacheWrapper() {
	
}

void AttributeSharedCacheWrapper::setOutputDomainAsChanged() {
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	attribute_setting.markAllAsChanged();
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setOutputAttributeValue(const std::string& attribute_name,
														void * value, uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setVariableValue(AttributeValueSharedCache::SVD_OUTPUT,
												   attribute_name,
												   value,
												   size);
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setOutputAttributeValue(VariableIndexType attribute_index,
																void * value,
																uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setVariableValue(AttributeValueSharedCache::SVD_OUTPUT,
												   attribute_index,
												   value,
												   size);
}

bool AttributeSharedCacheWrapper::setOutputAttributeNewSize(const std::string& attribute_name,
															uint32_t new_size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	return attribute_setting.setNewSize(attribute_setting.getIndexForName(attribute_name), new_size);
}

bool AttributeSharedCacheWrapper::setOutputAttributeNewSize(VariableIndexType attribute_index,
															uint32_t new_size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	return attribute_setting.setNewSize(attribute_index, new_size);
}

// Get the index of the changed attribute
void AttributeSharedCacheWrapper::getChangedInputAttributeIndex(std::vector<VariableIndexType>& changed_index) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->getChangedVariableIndex(AttributeValueSharedCache::SVD_INPUT,
														  changed_index);
}

void AttributeSharedCacheWrapper::resetChangedInputIndex() {
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_INPUT);
	attribute_setting.resetChangedIndex();
}

// Return the names of all variabl einto a determinated domain
void AttributeSharedCacheWrapper::getVariableNames(AttributeValueSharedCache::SharedVariableDomain domain,
										   std::vector<std::string>& names) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->getVariableNames(domain, names);
}

// Add a new variable
void AttributeSharedCacheWrapper::addCustomAttribute(const std::string&  name,
													 uint32_t max_size,
													 chaos::DataType::DataType type) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->addVariable(AttributeValueSharedCache::SVD_CUSTOM,
											  name,
											  max_size,
											  type);
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setCustomAttributeValue(const std::string& attribute_name,
																void * value,
																uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setVariableValue(AttributeValueSharedCache::SVD_CUSTOM,
												   attribute_name,
												   value,
												   size);
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setCustomAttributeValue(VariableIndexType attribute_index,
																  void * value,
																  uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setVariableValue(AttributeValueSharedCache::SVD_CUSTOM,
												   attribute_index,
												   value,
												   size);
}

void AttributeSharedCacheWrapper::setCustomDomainAsChanged() {
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_CUSTOM);
	attribute_setting.markAllAsChanged();

}