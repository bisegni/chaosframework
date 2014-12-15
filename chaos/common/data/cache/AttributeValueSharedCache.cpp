/*
 *	AttributeValueSharedCache.cpp
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

#include <string>

#include <chaos/common/global.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>

#define AVSC_LAPP_ LAPP_ << "[AttributeSetting-" << "] "
#define AVSC_LDBG_ LDBG_ << "[AttributeValue -" << "] " << __PRETTY_FUNCTION__ << " - "
#define AVSC_LERR_ LERR_ << "[AttributeValue -" << "] " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

using namespace std;
using namespace chaos::common::data::cache;


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributeValueSharedCache::AttributeValueSharedCache() {
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributeValueSharedCache::~AttributeValueSharedCache() {
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::init(void *initData) throw(chaos::CException) {
	//initialize the input channel setting
    utility::InizializableService::initImplementation(input_set,
													  initData,
													  "AttributeValueSharedCache[Input set]",
													  __PRETTY_FUNCTION__);
	//initialize the output channel setting
    utility::InizializableService::initImplementation(output_set,
													  initData,
													  "AttributeValueSharedCache[Output set]",
													  __PRETTY_FUNCTION__);
	//initialize the custom channel setting
	utility::InizializableService::initImplementation(system_set,
													  initData,
													  "AttributeValueSharedCache[System set]",
													  __PRETTY_FUNCTION__);
	//initialize the custom channel setting
    utility::InizializableService::initImplementation(custom_set,
													  initData,
													  "AttributeValueSharedCache[Custom set]",
													  __PRETTY_FUNCTION__);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::deinit() throw(chaos::CException) {
	//initialize the input channel setting
    utility::InizializableService::deinitImplementation(input_set,
														"AttributeValueSharedCache[Input Set]",
														__PRETTY_FUNCTION__);
	//initialize the output channel setting
    utility::InizializableService::deinitImplementation(output_set,
														"AttributeValueSharedCache[Output Set]",
														__PRETTY_FUNCTION__);
	//initialize the output channel setting
	utility::InizializableService::deinitImplementation(system_set,
														"AttributeValueSharedCache[System Set]",
														__PRETTY_FUNCTION__);
	//initialize the custom channel setting
    utility::InizializableService::deinitImplementation(custom_set,
														"AttributeValueSharedCache[Custom Set]",
														__PRETTY_FUNCTION__);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributeCache& AttributeValueSharedCache::getSharedDomain(SharedCacheDomain domain) {
    switch(domain) {
        case DOMAIN_INPUT:
            return input_set;
            break;
            
        case DOMAIN_OUTPUT:
            return output_set;
            break;
			
		case DOMAIN_SYSTEM:
			return system_set;
			break;
			
        case DOMAIN_CUSTOM:
            return custom_set;
            break;
    }
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::getChangedAttributeIndex(SharedCacheDomain domain,
														std::vector<VariableIndexType>& changed_index) {
    return getSharedDomain(domain).getChangedIndex(changed_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributeValue *AttributeValueSharedCache::getAttributeValue(SharedCacheDomain domain,
														  VariableIndexType variable_index) {
    return getSharedDomain(domain).getValueSettingForIndex(variable_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributeValue *AttributeValueSharedCache::getAttributeValue(SharedCacheDomain domain,
															const string& attribute_name) {
	AttributeCache& attribute_setting = getSharedDomain(domain);
    VariableIndexType index = attribute_setting.getIndexForName(attribute_name);
    return attribute_setting.getValueSettingForIndex(index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::setAttributeValue(SharedCacheDomain domain,
												 const string& attribute_name,
												 void * value,
												 uint32_t size) {
    VariableIndexType index = getSharedDomain(domain).getIndexForName(attribute_name);
    getSharedDomain(domain).setValueForAttribute(index, value, size);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::setAttributeValue(SharedCacheDomain domain,
												 VariableIndexType attribute_index,
												 void * value,
												 uint32_t size) {
	 getSharedDomain(domain).setValueForAttribute(attribute_index, value, size);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::getAttributeNames(SharedCacheDomain domain,
												std::vector<std::string>& names) {
    getSharedDomain(domain).getAttributeNames(names);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::addAttribute(SharedCacheDomain domain,
											const string&  name,
											uint32_t max_size,
											chaos::DataType::DataType type) {
    getSharedDomain(domain).addAttribute(name, max_size, type);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
boost::shared_ptr<SharedCacheLockDomain> AttributeValueSharedCache::getLockOnDomain(SharedCacheDomain domain,
																					bool write_lock) {
	boost::shared_ptr<SharedCacheLockDomain> result;
	if(write_lock) {
		result.reset(new WriteSharedCacheLockDomain(getSharedDomain(domain).mutex));
	} else {
		result.reset(new ReadSharedCacheLockDomain(getSharedDomain(domain).mutex));
	}
	return result;
}
