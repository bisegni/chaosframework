/*
 *	AttributesSetting.cpp
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
#include <chaos/common/data/cache/AttributesSetting.h>

#define CSLAPP_ LAPP_ << "[AttributeSetting-" << "] "
#define CSLDBG_ LDBG_ << "[AttributeSetting-" << "] "
#define CSLERR_ LERR_ << "[AttributeSetting-" << "] "

using namespace std;
using namespace chaos::common::data::cache;
#pragma mark ValueSetting
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ValueSetting::ValueSetting(const std::string& _name,
						   uint32_t _index,
						   uint32_t _size,
						   chaos::DataType::DataType _type):
value_buffer(NULL),
size(_size),
name(_name),
index(_index),
type(_type) {
	if(size) {
		value_buffer = std::calloc(size, 1);
		if(!value_buffer) {
			LERR_ << "error allcoating current_value memory";
		}
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ValueSetting::~ValueSetting() {
    if(value_buffer) free(value_buffer);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool ValueSetting::setValue(const void* value_ptr,
							uint32_t value_size,
							bool tag_has_changed) {
	if(value_size>size || !value_buffer) return false;
	
	//copy the new value
	std::memcpy(value_buffer, value_ptr, value_size);
	
	//set the relative field for set has changed
	if(tag_has_changed) sharedBitmapChangedAttribute->set(index);
	return true;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool ValueSetting::setNewSize(uint32_t _new_size) {
	bool result = true;
	switch(type) {
		case chaos::DataType::TYPE_BYTEARRAY:
		case chaos::DataType::TYPE_STRING:
			value_buffer = (double*)realloc(value_buffer, (size = _new_size));
			result = (value_buffer != NULL);
			break;
		default:
			break;
	}
	return result;
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ValueSetting::markAsChanged() {
	sharedBitmapChangedAttribute->set(index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void ValueSetting::markAsUnchanged() {
	sharedBitmapChangedAttribute->reset(index);
}


#pragma mark AttributesSetting
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributesSetting::AttributesSetting():
index(0),
mutex(new boost::shared_mutex()){
   // bitmapChangedAttribute = new boost::dynamic_bitset<BitBlockDimension>(mapAttributeIndexSettings.size());
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
AttributesSetting::~AttributesSetting() {
   // if(bitmapChangedAttribute) delete(bitmapChangedAttribute);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::init(void *initData) throw(chaos::CException) {
    
    //bitmapChangedAttribute = new boost::dynamic_bitset<BitBlockDimension>(mapAttributeIndexSettings.size());
    /*if(!bitmapChangedAttribute) throw CException(1, "Error allocating memory for map bit", "AttributesSetting::init");
   
    for (map<VariableIndexType, boost::shared_ptr<ValueSetting> >::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        it->second->sharedBitmapChangedAttribute = bitmapChangedAttribute;
    }*/
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::deinit() throw(chaos::CException) {
    //remove all ValueSetting instance
   /* for (map<VariableIndexType, boost::shared_ptr<ValueSetting> >::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        delete(it->second);
    }*/
    index = 0;
    mapAttributeNameIndex.clear();
    mapAttributeIndexSettings.clear();
    bitmapChangedAttribute.clear();
   /* if(bitmapChangedAttribute) {
       delete(bitmapChangedAttribute);
        bitmapChangedAttribute = NULL;
    }*/
   
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::addAttribute(string name,
									uint32_t size,
									chaos::DataType::DataType type) {
    
    //if(InizializableService::serviceState == utility::InizializableServiceType::IS_DEINTIATED)
    //    throw CException(1, "Attribute can be added only in deinitilized state", "AttributesSetting::addAttribute");
    
    if(mapAttributeNameIndex.count(name)) return;
    
    VariableIndexType tmpIndex;
    
    //add name nad his index
    mapAttributeNameIndex.insert(make_pair<string, VariableIndexType>(name, (tmpIndex=index++)));
    
    //add channel setting
	switch(type) {
		case chaos::DataType::TYPE_BOOLEAN:
			size = sizeof(bool);
			break;
		case chaos::DataType::TYPE_DOUBLE:
			size = sizeof(double);
			break;
		case chaos::DataType::TYPE_INT32:
			size = sizeof(int32_t);
			break;
		case chaos::DataType::TYPE_INT64:
			size = sizeof(int64_t);
			break;
			
		default:
			break;
	}
    boost::shared_ptr<ValueSetting> tmpSP(new ValueSetting(name, tmpIndex, size, type));
    
    //add the relative bit
    bitmapChangedAttribute.push_back(false);
    tmpSP->sharedBitmapChangedAttribute = &bitmapChangedAttribute;
    
    mapAttributeIndexSettings.insert(make_pair<VariableIndexType, boost::shared_ptr<ValueSetting> >(tmpIndex, tmpSP));
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::getAttributeNames(std::vector<std::string>& names) {
    
    //get all names
    for(map<string, VariableIndexType>::iterator it = mapAttributeNameIndex.begin();
        it != mapAttributeNameIndex.end();  
        it++) {
        names.push_back(it->first);
    }
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::setValueForAttribute(VariableIndexType n,
												   const void * value,
												   uint32_t size) {
    mapAttributeIndexSettings[n]->setValue(value, size);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VariableIndexType AttributesSetting::getIndexForName(const std::string& name ) {
	if(!mapAttributeNameIndex.count(name)) {
		throw chaos::CException(-1, "No name present in Attribute setting", __PRETTY_FUNCTION__);
	}
    return mapAttributeNameIndex[name];
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ValueSetting *AttributesSetting::getValueSettingForIndex(VariableIndexType index) {
	if(!mapAttributeIndexSettings.count(index)) {
		throw chaos::CException(-1, "No index present in Attribute setting", __PRETTY_FUNCTION__);
	}
    return mapAttributeIndexSettings[index].get();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VariableIndexType AttributesSetting::getNumberOfAttributes() {
	return index;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::getChangedIndex(std::vector<VariableIndexType>& changed_index) {
    size_t index = 0;
    index = bitmapChangedAttribute.find_first();
    while(index != boost::dynamic_bitset<BitBlockDimension>::npos) {
        changed_index.push_back(index);
        index = bitmapChangedAttribute.find_next(index);
    }
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::resetChangedIndex() {
	bitmapChangedAttribute.reset();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributesSetting::markAllAsChanged() {
	bitmapChangedAttribute.set();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool AttributesSetting::hasChanged() {
	return bitmapChangedAttribute.any();
}
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool AttributesSetting::setNewSize(VariableIndexType attribute_index, uint32_t new_size) {
	return mapAttributeIndexSettings[attribute_index]->setNewSize(new_size);
}
#pragma mark AttributeValueSharedCache

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
AttributesSetting& AttributeValueSharedCache::getSharedDomain(SharedVariableDomain domain) {
    switch(domain) {
        case SVD_INPUT:
            return input_set;
            break;
            
        case SVD_OUTPUT:
            return output_set;
            break;
			
		case SVD_SYSTEM:
			return system_set;
			break;
			
        case SVD_CUSTOM:
            return custom_set;
            break;
    }
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::getChangedVariableIndex(AttributeValueSharedCache::SharedVariableDomain domain,
													  std::vector<VariableIndexType>& changed_index) {
    return getSharedDomain(domain).getChangedIndex(changed_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ValueSetting *AttributeValueSharedCache::getVariableValue(AttributeValueSharedCache::SharedVariableDomain domain,
														VariableIndexType variable_index) {
    return getSharedDomain(domain).getValueSettingForIndex(variable_index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ValueSetting *AttributeValueSharedCache::getVariableValue(AttributeValueSharedCache::SharedVariableDomain domain,
														const string& variable_name) {
	AttributesSetting& attribute_setting = getSharedDomain(domain);
    VariableIndexType index = attribute_setting.getIndexForName(variable_name);
    return attribute_setting.getValueSettingForIndex(index);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::setVariableValue(AttributeValueSharedCache::SharedVariableDomain domain,
												 const string& variable_name,
												 void * value,
												 uint32_t size) {
    VariableIndexType index = getSharedDomain(domain).getIndexForName(variable_name);
    getSharedDomain(domain).setValueForAttribute(index, value, size);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::setVariableValue(SharedCacheInterface::SharedVariableDomain domain,
												 VariableIndexType variable_index,
												 void * value,
												 uint32_t size) {
	 getSharedDomain(domain).setValueForAttribute(variable_index, value, size);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::getVariableNames(AttributeValueSharedCache::SharedVariableDomain domain,
											   std::vector<std::string>& names) {
    getSharedDomain(domain).getAttributeNames(names);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void AttributeValueSharedCache::addVariable(SharedCacheInterface::SharedVariableDomain domain,
										  const string&  name,
										  uint32_t max_size,
										  chaos::DataType::DataType type) {
    getSharedDomain(domain).addAttribute(name, max_size, type);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
boost::shared_ptr<SharedCacheLockDomain> AttributeValueSharedCache::getLockOnDomain(SharedVariableDomain domain, bool write_lock) {
	boost::shared_ptr<SharedCacheLockDomain> result;
	if(write_lock) {
		result.reset(new WriteSharedCacheLockDomain(getSharedDomain(domain).mutex));
	} else {
		result.reset(new ReadSharedCacheLockDomain(getSharedDomain(domain).mutex));
	}
	return result;
}
