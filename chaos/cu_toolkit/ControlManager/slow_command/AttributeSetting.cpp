/*
 *	AttributeSetting.cpp
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
#include <chaos/cu_toolkit/ControlManager/slow_command/AttributeSetting.h>

#define CSLAPP_ LAPP_ << "[AttributeSetting-" << "] "
#define CSLDBG_ LDBG_ << "[AttributeSetting-" << "] "
#define CSLERR_ LERR_ << "[AttributeSetting-" << "] "

using namespace std;
using namespace chaos::cu::control_manager::slow_command;

ValueSetting::ValueSetting(uint32_t _size, uint32_t _index, chaos::DataType::DataType _type):currentValue(NULL), nextValue(NULL), size(_size),index(_index), type(_type) {
    currentValue = malloc(size);
    nextValue = malloc(size);
}

ValueSetting::~ValueSetting() {
    if(currentValue) free(currentValue);
    if(nextValue) free(nextValue);
}

void ValueSetting::completed() {
    std::memcpy(currentValue, nextValue, size);
    std::memset(nextValue, 0, size);
    sharedBitmapChangedAttribute->reset(index);
}

void ValueSetting::completedWithError() {
    std::memset(nextValue, 0, size);
    sharedBitmapChangedAttribute->reset(index);
}

bool ValueSetting::setNextValue(const void* valPtr, uint32_t _size) {
    if(_size>size) return false;
    
    //copy the new value
    std::memcpy(nextValue, valPtr, _size);
    
    //set the relative field for set has changed
    sharedBitmapChangedAttribute->set(index);
    return true;
}

bool ValueSetting::setDefaultValue(const void* valPtr, uint32_t _size) {
    if(_size>size) return false;
    
    //copy the new value
    std::memcpy(currentValue, valPtr, _size);
    
    //set the relative field for set has changed
    sharedBitmapChangedAttribute->reset(index);
    return true;
}

//-----------------------------------------------------------------------------------------------------------------------------

AttributeSetting::AttributeSetting() {
   // bitmapChangedAttribute = new boost::dynamic_bitset<BitBlockDimension>(mapAttributeIndexSettings.size());
}

AttributeSetting::~AttributeSetting() {
   // if(bitmapChangedAttribute) delete(bitmapChangedAttribute);
}

//! Initialize instance
void AttributeSetting::init(void *initData) throw(chaos::CException) {
    index = 0;
    //bitmapChangedAttribute = new boost::dynamic_bitset<BitBlockDimension>(mapAttributeIndexSettings.size());
    /*if(!bitmapChangedAttribute) throw CException(1, "Error allocating memory for map bit", "AttributeSetting::init");
   
    for (map<VariableIndexType, boost::shared_ptr<ValueSetting> >::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        it->second->sharedBitmapChangedAttribute = bitmapChangedAttribute;
    }*/
}

//! Deinit the implementation
void AttributeSetting::deinit() throw(chaos::CException) {
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

//!
void AttributeSetting::addAttribute(string name, uint32_t size, chaos::DataType::DataType type) {
    
    //if(InizializableService::serviceState == utility::InizializableServiceType::IS_DEINTIATED)
    //    throw CException(1, "Attribute can be added only in deinitilized state", "AttributeSetting::addAttribute");
    
    if(mapAttributeNameIndex.count(name)) return;
    
    VariableIndexType tmpIndex;
    
    //add name nad his index
    mapAttributeNameIndex.insert(make_pair<string, VariableIndexType>(name, (tmpIndex=index++)));
    
    //add channel setting
    boost::shared_ptr<ValueSetting> tmpSP(new ValueSetting(size, tmpIndex, type));
    
    //add the relative bit
    bitmapChangedAttribute.push_back(false);
    tmpSP->sharedBitmapChangedAttribute = &bitmapChangedAttribute;
    
    mapAttributeIndexSettings.insert(make_pair<VariableIndexType, boost::shared_ptr<ValueSetting> >(tmpIndex, tmpSP));
    
    //if(mapAttributeIndexSettings.size()!=index) {
        //error inserting the row;
        //LDBG_ << "error inserting the row";
    //}
}

void AttributeSetting::getAttributeNames(std::vector<std::string>& names) {
    
    //get all names
    for(map<string, VariableIndexType>::iterator it = mapAttributeNameIndex.begin();
        it != mapAttributeNameIndex.end();
        it++) {
        names.push_back(it->first);
    }
}

//! set the value for the index
void AttributeSetting::setDefaultValueForAttribute(VariableIndexType n, const void * value, uint32_t size) {
    mapAttributeIndexSettings[n]->setDefaultValue(value, size);
}

//! set the value for the index
void AttributeSetting::setValueForAttribute(VariableIndexType n, const void * value, uint32_t size) {
    mapAttributeIndexSettings[n]->setNextValue(value, size);
}

VariableIndexType AttributeSetting::getIndexForName( string name ) {
    return mapAttributeNameIndex[name];
}

ValueSetting *AttributeSetting::getValueSettingForIndex(VariableIndexType index) {
    if(!mapAttributeIndexSettings.count(index)) return NULL;
        
    return mapAttributeIndexSettings[index].get();
}

//!
void AttributeSetting::getChangedIndex(std::vector<VariableIndexType>& changedIndex) {
    size_t index = 0;
    index = bitmapChangedAttribute.find_first();
    while(index != boost::dynamic_bitset<BitBlockDimension>::npos) {
        changedIndex.push_back(index);
        /* do something */
        index = bitmapChangedAttribute.find_next(index);
    }
}

//------------------------------------------------------------------------------------------------------------

IOCAttributeShareCache::IOCAttributeShareCache() {
	
}

IOCAttributeShareCache::~IOCAttributeShareCache() {
	
}

//! Initialize instance
void IOCAttributeShareCache::init(void *initData) throw(chaos::CException) {
	//initialize the input channel setting
    utility::InizializableService::initImplementation(inputAttribute, initData, "IOCAttributeShareCache[Input Attribute]", "SlowCommandSandbox::init");
	//initialize the output channel setting
    utility::InizializableService::initImplementation(outputAttribute, initData, "IOCAttributeShareCache[Output Attribute]", "SlowCommandSandbox::init");
	//initialize the custom channel setting
    utility::InizializableService::initImplementation(customAttribute, initData, "IOCAttributeShareCache[Custom Attribute]", "SlowCommandSandbox::init");
}

//! Deinit the implementation
void IOCAttributeShareCache::deinit() throw(chaos::CException) {
	//initialize the input channel setting
    utility::InizializableService::deinitImplementation(inputAttribute, "IOCAttributeShareCache[Input Attribute]", "SlowCommandSandbox::deinit");
	//initialize the output channel setting
    utility::InizializableService::deinitImplementation(outputAttribute, "IOCAttributeShareCache[Output Attribute]", "SlowCommandSandbox::deinit");
	//initialize the custom channel setting
    utility::InizializableService::deinitImplementation(customAttribute, "IOCAttributeShareCache[Custom Attribute]", "SlowCommandSandbox::deinit");
}
