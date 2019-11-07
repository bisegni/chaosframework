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

#include <chaos/cu_toolkit/control_manager/AttributeSharedCacheWrapper.h>

using namespace chaos::cu::control_manager;
using namespace chaos::common::data::cache;

AttributeSharedCacheWrapper::AttributeSharedCacheWrapper(AbstractSharedDomainCache *_attribute_value_shared_cache):
attribute_value_shared_cache(_attribute_value_shared_cache){
	
}

AttributeSharedCacheWrapper::~AttributeSharedCacheWrapper() {
	
}

void AttributeSharedCacheWrapper::setOutputDomainAsChanged() {
	AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
	attribute_setting.markAllAsChanged();
}

void AttributeSharedCacheWrapper::setInputDomainAsChanged() {
	AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
	attribute_setting.markAllAsChanged();
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setOutputAttributeValue(const std::string& attribute_name,
														  void * value,
														  uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setAttributeValue(DOMAIN_OUTPUT,
													attribute_name,
													value,
													size);
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setOutputAttributeValue(VariableIndexType attribute_index,
																void * value,
																uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setAttributeValue(DOMAIN_OUTPUT,
												   attribute_index,
												   value,
												   size);
}

bool AttributeSharedCacheWrapper::setOutputAttributeNewSize(const std::string& attribute_name,
															uint32_t new_size,
                                                            bool clear_mem) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
	return attribute_setting.setNewSize(attribute_setting.getIndexForName(attribute_name),
                                        new_size,
                                        clear_mem);
}

bool AttributeSharedCacheWrapper::setOutputAttributeNewSize(VariableIndexType attribute_index,
                                                            uint32_t new_size,
                                                            bool clear_mem) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
	return attribute_setting.setNewSize(attribute_index,
                                        new_size,
                                        clear_mem);
}

ChaosSharedPtr<SharedCacheLockDomain> AttributeSharedCacheWrapper::getLockOnOutputAttributeCache(bool write_lock) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	return attribute_value_shared_cache->getLockOnDomain(DOMAIN_OUTPUT, write_lock);
}

// Get the index of the changed attribute
void AttributeSharedCacheWrapper::getChangedInputAttributeIndex(std::vector<VariableIndexType>& changed_index) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->getChangedAttributeIndex(DOMAIN_INPUT,
														   changed_index);
}

void AttributeSharedCacheWrapper::resetChangedInputIndex() {
	AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
	attribute_setting.resetChangedIndex();
}

// Return the names of all variabl einto a determinated domain
void AttributeSharedCacheWrapper::getAttributeNames(SharedCacheDomain domain,
													std::vector<std::string>& names) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->getAttributeNames(domain, names);
}

// Add a new variable
void AttributeSharedCacheWrapper::addCustomAttribute(const std::string&  name,
													 uint32_t max_size,
													 chaos::DataType::DataType type) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->addAttribute(DOMAIN_CUSTOM,
											   name,
											   max_size,
											   type);
}
void AttributeSharedCacheWrapper::setCustomAttributeValue(const std::string& attribute_name,
											 const chaos::common::data::CDataWrapper& value){
												 std::string svalue=value.getCompliantJSONString();
												 setCustomAttributeValue(attribute_name,(void*)svalue.c_str(),(uint32_t)svalue.size());

											 }
chaos::common::data::CDWUniquePtr AttributeSharedCacheWrapper::getCDValue(SharedCacheDomain domain,const std::string& attribute_name){
	AttributeValue *value_setting = attribute_value_shared_cache->getAttributeValue(domain, attribute_name);
	chaos::common::data::CDWUniquePtr dret(new chaos::common::data::CDataWrapper());
	if(value_setting->type==chaos::DataType::TYPE_CLUSTER){
		const char* ret=value_setting->getValuePtr<char>();
		dret->setSerializedJsonData(ret);
		
	}
	return dret;
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setCustomAttributeValue(const std::string& attribute_name,
																void * value,
																uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setAttributeValue(DOMAIN_CUSTOM,
												   attribute_name,
												   value,
												   size);
}

// Set the value for a determinated variable in a determinate domain
void AttributeSharedCacheWrapper::setCustomAttributeValue(VariableIndexType attribute_index,
																  void * value,
																  uint32_t size) {
	CHAOS_ASSERT(attribute_value_shared_cache)
	attribute_value_shared_cache->setAttributeValue(DOMAIN_CUSTOM,
												   attribute_index,
												   value,
												   size);
}

void AttributeSharedCacheWrapper::setCustomDomainAsChanged() {
	AttributeCache& cached = attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM);
	cached.markAllAsChanged();

}

ChaosSharedPtr<SharedCacheLockDomain> AttributeSharedCacheWrapper::getLockOnCustomAttributeCache(bool write_lock) {
	return 	attribute_value_shared_cache->getLockOnDomain(DOMAIN_CUSTOM, write_lock);
}

ChaosSharedPtr<SharedCacheLockDomain> AttributeSharedCacheWrapper::getReadLockOnInputAttributeCache() {
	return 	attribute_value_shared_cache->getLockOnDomain(DOMAIN_INPUT, false);
}
